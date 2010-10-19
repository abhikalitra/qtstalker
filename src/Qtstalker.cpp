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
#include <QDir>
#include <QFileDialog>
#include <QFont>
#include <QCursor>
#include <QColor>
#include <QStringList>
#include <QDateTime>
#include <QtDebug>
#include <QToolButton>
#include <QApplication>

#include "Qtstalker.h"
#include "DataWindow.h"
#include "Setup.h"
#include "QuoteServerRequestThread.h"
#include "DocsAction.h"
#include "Globals.h"
#include "QuoteServerButton.h"

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
//  setup.setupQuoteServer(config);
  
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

  setWindowTitle(getWindowCaption());
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


  _navSplitter = new Splitter((int) Config::NavAreaSize);
  connect(this, SIGNAL(signalLoadSettings()), _navSplitter, SLOT(load()));
  connect(this, SIGNAL(signalSaveSettings()), _navSplitter, SLOT(save()));
  _navSplitter->setOrientation(Qt::Horizontal);
  hbox->addWidget(_navSplitter);

  _chartLayout = new ChartLayout;
  _chartLayout->setOrientation(Qt::Vertical);
  _navSplitter->addWidget(_chartLayout);

  _sidePanel = new SidePanel;
  _plotSlider = _sidePanel->slider();
  _infoPanel = _sidePanel->info();
  connect(_sidePanel, SIGNAL(signalLoadChart(BarData)), this, SLOT(loadChart(BarData)));
  connect(_sidePanel, SIGNAL(signalRecentChart(BarData)), _recentCharts, SLOT(addRecentChart(BarData)));
  connect(_sidePanel, SIGNAL(signalReloadChart()), this, SLOT(chartUpdated()));
  connect(_sidePanel, SIGNAL(signalStatusMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(this, SIGNAL(signalSaveSettings()), _sidePanel, SLOT(save()));
  connect(this, SIGNAL(signalLoadSettings()), _sidePanel, SLOT(load()));
  connect(_sidePanelButton, SIGNAL(signalChanged(bool)), _sidePanel, SLOT(toggleStatus(bool)));
  _navSplitter->addWidget(_sidePanel);
  _sidePanel->toggleStatus(_sidePanelButton->isChecked());
  
  // delay chart layout signals until all objects are created
  
//  connect(_chartLayout, SIGNAL(signalRefresh()), this, SLOT(chartUpdated()));
  connect(this, SIGNAL(signalSaveSettings()), _chartLayout, SLOT(save()));
  connect(this, SIGNAL(signalLoadSettings()), _chartLayout, SLOT(load()));
//  connect(_chartLayout, SIGNAL(signalZoom(int, int)), _zoomButtons, SLOT(addZoom(int, int)));
  connect(_chartLayout, SIGNAL(signalInfo(Setting)), _infoPanel, SLOT(showInfo(Setting)));
  connect(_chartLayout, SIGNAL(signalMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(this, SIGNAL(signalSaveSettings()), _chartLayout, SLOT(saveSettings()));
//  connect(_chartLayout, SIGNAL(signalIndexChanged(int)), _plotSlider, SLOT(setValue(int)));
  connect(_sidePanel, SIGNAL(signalSliderChanged(int)), _chartLayout, SLOT(setIndex(int)));
//  connect(_barLengthButtons, SIGNAL(signalBarLengthChanged(int)), _chartLayout, SLOT(setInterval(int)));
  connect(_barLengthButtons, SIGNAL(signalBarLengthChanged(int)), this, SLOT(chartUpdated()));
  connect(_zoomButtons, SIGNAL(signalPixelSpace(int)), _chartLayout, SLOT(setBarSpacing(int)));
//  connect(_zoomButtons, SIGNAL(signalZoom(int, int)), _chartLayout, SLOT(setZoom(int, int)));
  connect(_gridAction, SIGNAL(signalChanged(bool)), _chartLayout, SLOT(setGrid(bool)));
  connect(_gridAction, SIGNAL(signalColorChanged(QColor)), _chartLayout, SLOT(setGridColor(QColor)));
  connect(_newIndicatorAction, SIGNAL(activated()), _chartLayout, SLOT(newIndicator()));
  connect(_crossHairsButton, SIGNAL(signalChanged(bool)), _chartLayout, SLOT(setCrossHairs(bool)));
  connect(_crossHairsButton, SIGNAL(signalColor(QColor)), _chartLayout, SLOT(setCrossHairsColor(QColor)));
  
  // end chart layout signals
  
  setUnifiedTitleAndToolBarOnMac(TRUE);

  // catch any kill signals and try to save config
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));
}

void QtstalkerApp::createToolBars ()
{
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

  // toggle side panel button
  _sidePanelButton = new SidePanelButton;
  toolbar->addWidget(_sidePanelButton);

  // toggle crosshairs button
  _crossHairsButton = new CrossHairsButton;
  toolbar->addWidget(_crossHairsButton);

  a = new QAction(QIcon(datawindow), tr("&Data Window"), this);
  a->setShortcut(QKeySequence(Qt::ALT+Qt::Key_1));
  a->setStatusTip(tr("Show the data window"));
  a->setToolTip(tr("Show the data window"));
  connect(a, SIGNAL(activated()), this, SLOT(dataWindow()));
  toolbar->addAction(a);

  _newIndicatorAction = new QAction(QIcon(indicator_xpm), tr("New &Indicator..."), this);
  _newIndicatorAction->setShortcut(QKeySequence(Qt::ALT+Qt::Key_I));
  _newIndicatorAction->setStatusTip(tr("Add a new indicator..."));
  _newIndicatorAction->setToolTip(tr("Add a new indicator..."));
  toolbar->addAction(_newIndicatorAction);

  // docs button
  DocsAction *da = new DocsAction;
  toolbar->addWidget(da);

  QuoteServerButton *qsb = new QuoteServerButton;
  toolbar->addWidget(qsb);

  toolbar->addSeparator();

  // create the zoom button box on the main toolbar
  _zoomButtons = new ZoomButtons(toolbar);

  // create 2nd tool bar
//  toolbar = addToolBar("buttonToolBar2");
//  toolbar->setIconSize(QSize(18, 18));

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

  // create recent charts combobox
  _recentCharts = new RecentCharts(toolbar);
  connect(_recentCharts, SIGNAL(signalChartSelected(BarData)), this, SLOT(loadChart(BarData)));
  connect(this, SIGNAL(signalSaveSettings()), _recentCharts, SLOT(save()));
  connect(this, SIGNAL(signalLoadSettings()), _recentCharts, SLOT(load()));
}

void QtstalkerApp::loadSettings (Config &config)
{
  // restore the size of the app
  QSize sz;
  config.transaction();
  
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

  config.commit();

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

  QStringList c;
  c <<  _currentChart.getExchange() << _currentChart.getSymbol();
  QString chart = c.join(",");
  Config config;
//  config.transaction();
  config.setData((int) Config::CurrentChart, chart);
//  config.commit();

  // construct a QuoteServer command to get the quotes
  QStringList l;
  l << "Quotes" << "Date" << _currentChart.getExchange() << _currentChart.getSymbol();
  QString s;
  _barLengthButtons->getCurrentText(s);
  l << s;
  if (_dateRangeButton->isChecked())
  {
    l << _dateRangeButton->startDate().toString("yyyyMMddHHmmss");
    l << _dateRangeButton->endDate().toString("yyyyMMddHHmmss");
    l << "-1";
  }
  else
    l << "0" << "0" << QString::number(_dateRange->dateRange());
  
  QString command = l.join(",") + "\n";

  // create a thread and not block until results are returned
  QuoteServerRequestThread *r = new QuoteServerRequestThread(this, command);
  connect(r, SIGNAL(signalDone(QString)), this, SLOT(loadChart2(QString)), Qt::QueuedConnection);
  connect(r, SIGNAL(finished()), r, SLOT(deleteLater()));
  r->start();
}

void QtstalkerApp::loadChart2 (QString data)
{
  // we got results from a QuoteServer thread command
  // set plots to empty
  _chartLayout->clearIndicator();

  // create and populate the quote data
  g_barData.clear();
  g_barData = _currentChart;
  g_barData.setBarLength((BarData::BarLength) _barLengthButtons->length());
  g_barData.setBars(data);
  
  // adjust the starting position of the displayed chart to show a full page of the most recent bars
  setSliderStart(g_barData.count());

  // set the window title to update what symbol we are displaying
  setWindowTitle(getWindowCaption());

  // clear the status bar of any messages
  statusMessage(QString());

  // tell the charts to update with the new bar data
  _chartLayout->loadPlots(_plotSlider->getValue());
}

QString QtstalkerApp::getWindowCaption ()
{
  // update the main window text

  QString caption = tr("Qtstalker");
  caption.append(g_session);
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
  if (_currentChart.getSymbol().isEmpty())
    return;
  
  // show the datawindow dialog
  DataWindow *dw = new DataWindow(this);
  connect(dw, SIGNAL(finished(int)), dw, SLOT(deleteLater()));
  dw->setWindowTitle(getWindowCaption() + " - " + tr("Indicators"));
  dw->setData(_chartLayout->plots());
  dw->show();
  dw->scrollToBottom();
}

void QtstalkerApp::chartUpdated ()
{
  // we are here because something wants us to reload the chart with fresh bars
  _sidePanel->updateChartTab();

  if (_currentChart.getSymbol().isEmpty())
    return;

  loadChart(_currentChart);
}

void QtstalkerApp::statusMessage (QString d)
{
  // update the status bar with a new message from somewhere
  _statusBar->showMessage(d);
  wakeup();
}

void QtstalkerApp::wakeup ()
{
  // force app to process the event que so we keep from any blocking
  qApp->processEvents();
}

void QtstalkerApp::appFont (QFont d)
{
  // app font changed, update the app with it and NO BLOCKING
  qApp->setFont(d, 0);
  wakeup();
}

/**********************************************************************/
/************************ TOOLBAR FUNCTIONS ***************************/
/**********************************************************************/

void QtstalkerApp::setSliderStart (int count)
{
  // figure out a nice spot to show the most recent bars in the charts
  if (_currentChart.getSymbol().isEmpty())
    return;

  int width = _chartLayout->plotWidth();

//  _plotSlider->setStart(count, width, _zoomButtons->getPixelSpace());
  _plotSlider->setStart(count, width, 8);
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

