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
#include <QSettings>
#include <QCoreApplication>

#include "Qtstalker.h"
#include "Setup.h"
#include "HelpButton.h"
#include "Globals.h"
#include "CommandThread.h"
#include "ChartBackgroundColorButton.h"
#include "ChartFontButton.h"
#include "DataWindowButton.h"
#include "GridButton.h"
#include "CrossHairsButton.h"
#include "AboutButton.h"
#include "NewIndicatorButton.h"
#include "RefreshButton.h"
#include "QuoteDataBase.h"

#include "../pics/qtstalker.xpm"

QtstalkerApp::QtstalkerApp(QString session, QString asset)
{
  setWindowIcon(QIcon(qtstalker));

  QCoreApplication::setOrganizationName("QtStalker");
  QCoreApplication::setOrganizationDomain("QtStalker.com");
  QCoreApplication::setApplicationName("QtStalker");

  Setup setup;
  setup.setup(session);

  createStatusToolBar();

  createToolBar();

  createGUI();

  loadSettings();

  // check if we are going to display a chart from the command line
  if (asset.length())
  {
    _clAsset = asset;
    QTimer::singleShot(500, this, SLOT(commandLineAsset()));
  }

  _statusBar->showMessage(tr("Ready"), 2000);

  setWindowTitle(getWindowCaption());
}

QtstalkerApp::~QtstalkerApp ()
{
}

void QtstalkerApp::createGUI ()
{
  _baseWidget = new QWidget;
  setCentralWidget (_baseWidget);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(0);
  _baseWidget->setLayout(hbox);

  _navSplitter = new Splitter("nav_area_size");
  connect(this, SIGNAL(signalLoadSettings()), _navSplitter, SLOT(load()));
  connect(_quitButton, SIGNAL(signalShutdown()), _navSplitter, SLOT(save()));
  _navSplitter->setOrientation(Qt::Horizontal);
  hbox->addWidget(_navSplitter);

  _chartLayout = new ChartLayout;
  connect(_quitButton, SIGNAL(signalShutdown()), _chartLayout, SLOT(save()));
  connect(_quitButton, SIGNAL(signalShutdown()), _chartLayout, SLOT(saveSettings()));
  _chartLayout->setOrientation(Qt::Vertical);
  _navSplitter->addWidget(_chartLayout);

  _sidePanel = new SidePanel;
  _plotSlider = _sidePanel->slider();
  _infoPanel = _sidePanel->info();
  connect(_sidePanel, SIGNAL(signalLoadChart(BarData)), this, SLOT(loadChart(BarData)));
  connect(_sidePanel, SIGNAL(signalRecentChart(BarData)), _recentCharts, SLOT(addRecentChart(BarData)));
  connect(_sidePanel, SIGNAL(signalReloadChart()), this, SLOT(chartUpdated()));
  connect(_sidePanel, SIGNAL(signalStatusMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(_quitButton, SIGNAL(signalShutdown()), _sidePanel, SLOT(save()));
  connect(this, SIGNAL(signalLoadSettings()), _sidePanel, SLOT(load()));
  connect(_sidePanelButton, SIGNAL(signalChanged(bool)), _sidePanel, SLOT(toggleStatus(bool)));
  _navSplitter->addWidget(_sidePanel);
  _sidePanel->toggleStatus(_sidePanelButton->isChecked());

  
  // delay chart layout signals until all objects are created
  
  connect(this, SIGNAL(signalLoadSettings()), _chartLayout, SLOT(load()));
  connect(_chartLayout, SIGNAL(signalInfo(Setting)), _infoPanel, SLOT(showInfo(Setting)));
  connect(_chartLayout, SIGNAL(signalMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(_sidePanel, SIGNAL(signalSliderChanged(int)), _chartLayout, SLOT(setIndex(int)));
  connect(_barLengthButtons, SIGNAL(signalBarLengthChanged(int)), this, SLOT(chartUpdated()));
  connect(_zoomButtons, SIGNAL(signalPixelSpace(int)), _chartLayout, SLOT(setBarSpacing(int)));

  connect(g_middleMan, SIGNAL(signalIndicatorNew(QString)), _chartLayout, SLOT(addNewTab(QString)));
  connect(g_middleMan, SIGNAL(signalIndicatorDelete(QStringList)), _chartLayout, SLOT(removeTab(QStringList)));
  
  // end chart layout signals
  
  setUnifiedTitleAndToolBarOnMac(TRUE);
}

void QtstalkerApp::createToolBar ()
{
  //construct main toolbar
  QToolBar *toolbar = addToolBar("buttonToolBar");
  toolbar->setIconSize(QSize(16, 16));

  // add this to right justify everything after on the toolbar
  QWidget *spacerWidget = new QWidget;
  spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  spacerWidget->setVisible(true);
  toolbar->addWidget(spacerWidget);

  // create the zoom button box on the main toolbar
  _zoomButtons = new ZoomButtons(toolbar);

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
  connect(_quitButton, SIGNAL(signalShutdown()), _recentCharts, SLOT(save()));
}

void QtstalkerApp::createStatusToolBar ()
{
  _statusBar = statusBar();

  QToolBar *toolBar = addToolBar("statusToolBar");
  toolBar->setIconSize(QSize(16, 16));

  // add the toolbar actions
  _quitButton = new QuitButton;
  connect(_quitButton, SIGNAL(signalShutdown()), this, SLOT(save()));
  toolBar->addWidget(_quitButton);

  // grid button
  GridButton *gb = new GridButton;
  toolBar->addWidget(gb);

  // refresh button
  RefreshButton *rb = new RefreshButton;
  connect(rb, SIGNAL(signalRefresh()), this, SLOT(chartUpdated()));
  toolBar->addWidget(rb);

  // toggle side panel button
  _sidePanelButton = new SidePanelButton;
  toolBar->addWidget(_sidePanelButton);

  // toggle crosshairs button
  CrossHairsButton *chb = new CrossHairsButton;
  toolBar->addWidget(chb);

  DataWindowButton *dwb = new DataWindowButton;
  toolBar->addWidget(dwb);

  // new indicator button
  NewIndicatorButton *nib = new NewIndicatorButton;
  toolBar->addWidget(nib);

  // docs button
  HelpButton *hb = new HelpButton;
  toolBar->addWidget(hb);

  AboutButton *ab = new AboutButton;
  toolBar->addWidget(ab);

  ChartBackgroundColorButton *cbcb = new ChartBackgroundColorButton;
  toolBar->addWidget(cbcb);
  
  ChartFontButton *cfb= new ChartFontButton;
  toolBar->addWidget(cfb);

  _statusBar->addPermanentWidget(toolBar);
}

void QtstalkerApp::loadSettings ()
{
  QSettings settings(g_settingsFile);
  
  // restore the size of the app
  QSize sz = settings.value("main_window_size", QSize(800,600)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("main_window_position", QPoint(0,0)).toPoint();
  move(p);

  // load gui class settings that need to now
  emit signalLoadSettings();
}

void QtstalkerApp::save()
{
  QSettings settings(g_settingsFile);
  settings.setValue("main_window_size", size());
  settings.setValue("main_window_position", pos());
  settings.sync();
}

void QtstalkerApp::loadChart (BarData symbol)
{
  // do all the stuff we need to do to load a chart
  if (symbol.symbol().length() == 0)
    return;

  delete g_barData;
  g_barData = new BarData;
  
  g_barData->setExchange(symbol.exchange());
  g_barData->setSymbol(symbol.symbol());
  g_barData->setName(symbol.name());
  g_barData->setBarLength((BarData::BarLength) _barLengthButtons->length());

  QSettings settings(g_settingsFile);
  settings.setValue("current_chart", g_barData->key());
  settings.sync();

  if (_dateRangeButton->isChecked())
  {
    g_barData->setStartDate(_dateRangeButton->startDate());
    g_barData->setEndDate(_dateRangeButton->endDate());
    g_barData->setRange(-1);
  }
  else
  {
    g_barData->setStartDate(QDateTime());
    g_barData->setEndDate(QDateTime());
    g_barData->setRange(_dateRange->dateRange());
  }
  
  QuoteDataBase db;
  db.getBars(g_barData);

//  _chartLayout->clearIndicator();
  setSliderStart(g_barData->count());
  setWindowTitle(getWindowCaption());
  statusMessage(QString());
  _chartLayout->loadPlots(_plotSlider->getValue());
}

QString QtstalkerApp::getWindowCaption ()
{
  // update the main window text

  QString caption = tr("QtStalker");
  caption.append(g_session);
  caption.append(": " + g_barData->symbol());
  if (! g_barData->name().isEmpty())
    caption.append(" (" + g_barData->name() + ")");

  QStringList l;
  g_barData->barLengthList(l);
  caption.append(" " + l[_barLengthButtons->length()]);

  return caption;
}

void QtstalkerApp::chartUpdated ()
{
  // we are here because something wants us to reload the chart with fresh bars
//  _sidePanel->updateChartTab();

  if (g_barData->symbol().isEmpty())
    return;

  BarData bd;
  bd.setExchange(g_barData->exchange());
  bd.setSymbol(g_barData->symbol());
  loadChart(bd);
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

/**********************************************************************/
/************************ TOOLBAR FUNCTIONS ***************************/
/**********************************************************************/

void QtstalkerApp::setSliderStart (int count)
{
  // figure out a nice spot to show the most recent bars in the charts
  if (g_barData->symbol().isEmpty())
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
