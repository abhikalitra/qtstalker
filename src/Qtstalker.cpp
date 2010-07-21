/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include <QLayout>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QFont>
#include <QCursor>
#include <QColor>
#include <QStringList>
#include <QDateTime>
#include <QVBoxLayout>
#include <QtDebug>
#include <QToolButton>
#include <QApplication>
#include <QHBoxLayout>
#include <QTimer>

#include "Qtstalker.h"
#include "DataWindow.h"
#include "ChartPage.h"
#include "PlotLine.h"
#include "PrefDialog.h"
#include "Indicator.h"
#include "Setup.h"
#include "IndicatorPage.h"
#include "PluginFactory.h"
#include "IndicatorPlugin.h"
#include "IndicatorPluginFactory.h"
#include "IndicatorDataBase.h"
#include "COFactory.h"
#include "DateRange.h"
#include "QuoteServerRequestThread.h"

#include "../pics/done.xpm"
#include "../pics/datawindow.xpm"
#include "../pics/indicator.xpm"
#include "../pics/qtstalker.xpm"

QtstalkerApp::QtstalkerApp(QString session, QString asset)
{
  setWindowIcon(QIcon(qtstalker));
  QApplication::setOrganizationName("Qtstalker");

  Config config;
  Setup setup;
  setup.setup(config, session);

  // start the quote server
  setup.setupQuoteServer(config);
  
  createToolBars();

  createGUI(config);

  loadSettings(config);

  // check if we are going to display a chart from the command line
  if (asset.length())
  {
    _clAsset = asset;
    QTimer::singleShot(500, this, SLOT(commandLineAsset()));
  }

  _statusBar->showMessage(tr("Ready"), 2000);
}

void QtstalkerApp::createGUI (Config &)
{
  _statusBar = statusBar();

  _baseWidget = new QWidget;
  setCentralWidget (_baseWidget);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(0);
  _baseWidget->setLayout(hbox);

  // add the side toolBar first
//  hbox->addWidget(_toolBar2);

  _navSplitter = new Splitter((int) Config::NavAreaSize);
  connect(this, SIGNAL(signalLoadSettings()), _navSplitter, SLOT(load()));
  connect(this, SIGNAL(signalSaveSettings()), _navSplitter, SLOT(save()));
  _navSplitter->setOrientation(Qt::Horizontal);
  hbox->addWidget(_navSplitter);

  _chartLayout = new ChartLayout;
  _chartLayout->setOrientation(Qt::Vertical);
  _navSplitter->addWidget(_chartLayout);

  _navSplitter->addWidget(_toolBar2);

  _sidePanel = new SidePanel;
  _plotSlider = _sidePanel->slider();
  _infoPanel = _sidePanel->info();
  connect(_sidePanel, SIGNAL(signalLoadChart(BarData)), this, SLOT(loadChart(BarData)));
  connect(_sidePanel, SIGNAL(signalRecentChart(BarData)), _recentCharts, SLOT(addRecentChart(BarData)));
  connect(_sidePanel, SIGNAL(signalReloadChart()), this, SLOT(chartUpdated()));
  connect(_sidePanel, SIGNAL(signalStatusMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(this, SIGNAL(signalSaveSettings()), _sidePanel, SLOT(save()));
  connect(this, SIGNAL(signalLoadSettings()), _sidePanel, SLOT(load()));
  _navSplitter->addWidget(_sidePanel);


  // delay chart layout signals until all objects are created
  
  connect(_chartLayout, SIGNAL(signalRefresh()), this, SLOT(chartUpdated()));
  connect(this, SIGNAL(signalSaveSettings()), _chartLayout, SLOT(save()));
  connect(this, SIGNAL(signalLoadSettings()), _chartLayout, SLOT(load()));
  connect(_chartLayout, SIGNAL(signalZoom(int, int)), _zoomButtons, SLOT(addZoom(int, int)));
  connect(_chartLayout, SIGNAL(signalInfo(Setting *)), _infoPanel, SLOT(showInfo(Setting *)));
  connect(_chartLayout, SIGNAL(signalStatus(QString)), this, SLOT(statusMessage(QString)));
  connect(this, SIGNAL(signalSaveSettings()), _chartLayout, SLOT(saveSettings()));
  connect(_chartLayout, SIGNAL(signalIndexChanged(int)), _plotSlider, SLOT(setValue(int)));
  connect(_sidePanel, SIGNAL(signalSliderChanged(int)), _chartLayout, SLOT(setIndex(int)));
  connect(_barLengthButtons, SIGNAL(signalBarLengthChanged(int)), _chartLayout, SLOT(setInterval(int)));
  connect(_gridAction, SIGNAL(signalChanged(bool)), _chartLayout, SLOT(setGrid(bool)));
  connect(_zoomButtons, SIGNAL(signalPixelSpace(int)), _chartLayout, SLOT(setPixelSpace(int)));
  connect(_zoomButtons, SIGNAL(signalZoom(int, int)), _chartLayout, SLOT(setZoom(int, int)));
  connect(_gridAction, SIGNAL(signalColorChanged(QColor)), _chartLayout, SLOT(setGridColor(QColor)));
  connect(_newIndicatorAction, SIGNAL(activated()), _chartLayout, SLOT(newIndicator()));
  connect(_toolBar2, SIGNAL(signalCursorButtonClicked(int)), _chartLayout, SLOT(cursorChanged(int)));
  connect(_toolBar2, SIGNAL(signalCOButtonClicked(QString)), _chartLayout, SLOT(newExternalChartObject(QString)));
  
  // end chart layout signals
  
  setUnifiedTitleAndToolBarOnMac(TRUE);

  // catch any kill signals and try to save config
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));
}

void QtstalkerApp::createToolBars ()
{
  // construct the left side toolbar
  _toolBar2 = new COToolBar;
  addToolBar(_toolBar2);
  _toolBar2->setOrientation(Qt::Vertical);

  //construct main toolbar
  QToolBar *toolbar = addToolBar("buttonToolBar");
  toolbar->setIconSize(QSize(18, 18));

  // add this to right justify everything after on the toolbar
  QWidget *spacerWidget = new QWidget;
  spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  spacerWidget->setVisible(true);
  toolbar->addWidget(spacerWidget);

  // add the toolbar actions
  QAction *a = new QAction(QIcon(done_xpm), tr("E&xit"), this);
  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
  a->setStatusTip(tr("Quit Qtstalker"));
  a->setToolTip(tr("Quit Qtstalker"));
  connect(a, SIGNAL(activated()), qApp, SLOT(quit()));
  toolbar->addAction(a);

  // grid button
  _gridAction = new GridAction;
  toolbar->addWidget(_gridAction);

  // refresh button
  _refreshAction = new RefreshAction;
  connect(_refreshAction, SIGNAL(signalRefresh()), this, SLOT(chartUpdated()));
  toolbar->addWidget(_refreshAction);

  a = new QAction(QIcon(datawindow), tr("&Data Window"), this);
  a->setShortcut(QKeySequence(Qt::ALT+Qt::Key_1));
  a->setStatusTip(tr("Show the data window"));
  a->setToolTip(tr("Show the data window"));
  connect(a, SIGNAL(activated()), this, SLOT(dataWindow()));
  toolbar->addAction(a);

  _newIndicatorAction = new QAction(QIcon(indicator_xpm), tr("New &Indicator"), this);
  _newIndicatorAction->setShortcut(QKeySequence(Qt::ALT+Qt::Key_I));
  _newIndicatorAction->setStatusTip(tr("Add a new indicator"));
  _newIndicatorAction->setToolTip(tr("Add a new indicator"));
  toolbar->addAction(_newIndicatorAction);

  // docs button
  DocsAction *da = new DocsAction;
  toolbar->addWidget(da);
  
  toolbar->addSeparator();

  // create the bar length button group
  _barLengthButtons = new BarLengthButtons(toolbar);

  toolbar->addSeparator();

  // date range controls
  _dateRange = new DateRangeControl(toolbar);
  connect(_dateRange, SIGNAL(signalDateRangeChanged()), this, SLOT(chartUpdated()));

  // date range button
  _dateRangeButton = new DateRangeButton();
  connect(_dateRangeButton, SIGNAL(signalDateRangeChanged()), this, SLOT(chartUpdated()));
  toolbar->addWidget(_dateRangeButton);

  toolbar->addSeparator();

  // create the zoom button box on the main toolbar
  _zoomButtons = new ZoomButtons(toolbar);

  toolbar->addSeparator();

  // create recent charts combobox
  _recentCharts = new RecentCharts(toolbar);
  connect(_recentCharts, SIGNAL(signalChartSelected(BarData)), this, SLOT(loadChart(BarData)));
  connect(this, SIGNAL(signalSaveSettings()), _recentCharts, SLOT(save()));
  connect(this, SIGNAL(signalLoadSettings()), _recentCharts, SLOT(load()));

  // add this to right justify everything after on the toolbar
//  spacerWidget = new QWidget;
//  spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
//  spacerWidget->setVisible(true);
//  toolbar->addWidget(spacerWidget);
}

void QtstalkerApp::loadSettings (Config &config)
{
  // restore the size of the app
  QSize sz;
  config.getData(Config::MainWindowSize, sz);
  if (! sz.isValid())
  {
    sz = QSize(800, 600);
    config.setData(Config::MainWindowSize, sz);
  }
  resize(sz);

  // restore the position of the app
  QPoint p;
  config.getData((int) Config::MainWindowPos, p);
  if (p.isNull())
  {
    p = QPoint(0, 0); // 30,30 ?
    config.setData(Config::MainWindowPos, p);
  }
  move(p);

  // load gui class settings that need to now
  emit signalLoadSettings();
}

void QtstalkerApp::quit()
{
  // save everything that needs to be saved before app quits
  // classes cannot save in their destructors due to SQL services
  // shutting down before the destructor is called
  emit signalSaveSettings();
  
  Config config;
  config.transaction();

  // save app size and position
  QSize sz = size();
  config.setData((int) Config::MainWindowSize, sz);
  QPoint pt = pos();
  config.setData((int) Config::MainWindowPos, pt);

  config.commit();
}

void QtstalkerApp::loadChart (BarData symbol)
{
  // do all the stuff we need to do to load a chart
  if (symbol.getSymbol().length() == 0)
    return;

  _currentChart = symbol;

  QStringList l;
  l << "Quotes";
  if (_dateRangeButton->isChecked())
    l << "Date";
  else
    l << "Recent";
  l << _currentChart.getExchange() << _currentChart.getSymbol();
  QString s;
  _barLengthButtons->getCurrentText(s);
  l << s;
  if (_dateRangeButton->isChecked())
  {
    l << _dateRangeButton->startDate().toString("yyyyMMddHHmmss");
    l << _dateRangeButton->endDate().toString("yyyyMMddHHmmss");
  }
  else
    l << QString::number(_dateRange->dateRange());
  
  QString command = l.join(",") + "\n";
  
  QuoteServerRequestThread *r = new QuoteServerRequestThread(this, command);
  connect(r, SIGNAL(signalDone(QString)), this, SLOT(loadChart2(QString)), Qt::QueuedConnection);
  connect(r, SIGNAL(finished()), r, SLOT(deleteLater()));
  r->start();
}

void QtstalkerApp::loadChart2 (QString data)
{
  // set plots to empty
  _chartLayout->clearIndicator();

  // create and populate the quote data
  BarData bd = _currentChart;
  bd.setBarLength((BarData::BarLength) _barLengthButtons->length());
  bd.setBars(data);
  
  setSliderStart(bd.count());

  setWindowTitle(getWindowCaption());
  statusMessage(QString());

  _chartLayout->loadPlots(bd, _plotSlider->getValue());
}

QString QtstalkerApp::getWindowCaption ()
{
  // update the main window text

  QString caption = tr("Qtstalker");
  caption.append(": " + _currentChart.getSymbol());
  if (! _currentChart.getName().isEmpty())
    caption.append(" (" + _currentChart.getName() + ")");

  QStringList l;
  _currentChart.getBarLengthList(l);
  caption.append(" " + l[_barLengthButtons->length()]);

  return caption;
}

void QtstalkerApp::dataWindow ()
{
  // show the datawindow dialog
  DataWindow *dw = new DataWindow(this);
  dw->setWindowTitle(getWindowCaption());
  dw->setData(_chartLayout->plotList());
  dw->show();
  dw->scrollToBottom();
}

void QtstalkerApp::chartUpdated ()
{
  _sidePanel->updateChartTab();

  if (_currentChart.getSymbol().isEmpty())
    return;

  loadChart(_currentChart);
}

void QtstalkerApp::statusMessage (QString d)
{
  _statusBar->showMessage(d);
  wakeup();
}

void QtstalkerApp::wakeup ()
{
  qApp->processEvents();
}

void QtstalkerApp::appFont (QFont d)
{
  qApp->setFont(d, 0);
  wakeup();
}

/**********************************************************************/
/************************ TOOLBAR FUNCTIONS ***************************/
/**********************************************************************/

void QtstalkerApp::setSliderStart (int count)
{
  if (_currentChart.getSymbol().isEmpty())
    return;

  int width = _chartLayout->plotWidth();
  
  _plotSlider->setStart(count, width, _zoomButtons->getPixelSpace());
}

// ******************************************************************************
// ******************* Command Line Asset ***************************************
// ******************************************************************************
// if the -asset command was issued from the command line, then we will load
// a chart on startup

void QtstalkerApp::commandLineAsset ()
{
  QStringList l = _clAsset.split(":");
  if (l.count() != 2)
  {
    qDebug() << "QtstalkerApp::commandLineAsset: invalid command line asset string" << _clAsset;
    return;
  }
  
  BarData bd;
  bd.setExchange(l[0]);
  bd.setSymbol(l[1]);
  loadChart(bd);
}

