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
#include "DataWindowButton.h"
#include "GridButton.h"
#include "CrossHairsButton.h"
#include "AboutButton.h"
#include "NewIndicatorButton.h"
#include "RefreshButton.h"
#include "QuoteDataBase.h"
#include "InfoPanel.h"
#include "DockWidget.h"
#include "IndicatorDataBase.h"
#include "ConfigureButton.h"

#include "../pics/qtstalker.xpm"

QtstalkerApp::QtstalkerApp(QString session, QString asset)
{
  setWindowIcon(QIcon(qtstalker_xpm));

  QCoreApplication::setOrganizationName("QtStalker");
  QCoreApplication::setOrganizationDomain("QtStalker.com");
  QCoreApplication::setApplicationName("QtStalker");

  Setup setup;
  setup.setup(session);

  createStatusToolBar();

  createToolBar();

  createGUI();

  loadSettings();

  _statusBar->showMessage(tr("Ready"), 2000);

  setWindowTitle(getWindowCaption());

  // expose hidden charts so they plot properly (simple hack)
  QTimer::singleShot(500, this, SLOT(fixDockTabs()));

  // check if we are going to display a chart from the command line
  if (asset.length())
  {
    _clAsset = asset;
    QTimer::singleShot(500, this, SLOT(commandLineAsset()));
  }
}

void QtstalkerApp::createGUI ()
{
  setDockNestingEnabled(TRUE);
  setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::East);
  
  connect(g_middleMan, SIGNAL(signalIndicatorNew(QString)), this, SLOT(addNewPlot(QString)));
  connect(g_middleMan, SIGNAL(signalIndicatorDelete(QStringList)), this, SLOT(deletePlot(QStringList)));
  connect(g_middleMan, SIGNAL(signalPlotTabPosition(int)), this, SLOT(setPlotTabPosition(int)));
  connect(g_middleMan, SIGNAL(signalPlotUpdate(QString)), this, SLOT(updatePlot(QString)));

  // side panel dock
  _sidePanel = new SidePanel;
  connect(_sidePanel, SIGNAL(signalLoadChart(BarData)), this, SLOT(loadChart(BarData)));
  connect(_sidePanel, SIGNAL(signalRecentChart(BarData)), _recentCharts, SLOT(addRecentChart(BarData)));
  connect(_sidePanel, SIGNAL(signalReloadChart()), this, SLOT(chartUpdated()));
  connect(_sidePanel, SIGNAL(signalStatusMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(this, SIGNAL(signalLoadSettings()), _sidePanel, SLOT(loadSettings()));
  connect(_quitButton, SIGNAL(signalShutdown()), _sidePanel, SLOT(saveSettings()));

  DockWidget *dock = new DockWidget(QString(), this);
  dock->setObjectName("sidePanelDock");
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dock->setWidget(_sidePanel);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  connect(dock, SIGNAL(signalLockStatus(bool)), _sidePanel, SLOT(setLockStatus(bool)));
  connect(_sidePanel, SIGNAL(signalLockStatus(bool)), dock, SLOT(statusChanged(bool)));

  // plot slider dock
  _plotSlider = new PlotSlider;
  connect(this, SIGNAL(signalLoadSettings()), _plotSlider, SLOT(loadSettings()));
  connect(_quitButton, SIGNAL(signalShutdown()), _plotSlider, SLOT(saveSettings()));
  
  dock = new DockWidget(QString(), this);
  dock->setObjectName("plotSliderDock");
  dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dock->setWidget(_plotSlider);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  connect(dock, SIGNAL(signalLockStatus(bool)), _plotSlider, SLOT(setLockStatus(bool)));
  connect(_plotSlider, SIGNAL(signalLockStatus(bool)), dock, SLOT(statusChanged(bool)));

  // info panel dock
  _infoPanel = new InfoPanel;
  connect(this, SIGNAL(signalLoadSettings()), _infoPanel, SLOT(loadSettings()));
  connect(_quitButton, SIGNAL(signalShutdown()), _infoPanel, SLOT(saveSettings()));
  
  dock = new DockWidget(QString(), this);
  dock->setObjectName("infoPanelDock");
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dock->setWidget(_infoPanel);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  connect(dock, SIGNAL(signalLockStatus(bool)), _infoPanel, SLOT(setLockStatus(bool)));
  connect(_infoPanel, SIGNAL(signalLockStatus(bool)), dock, SLOT(statusChanged(bool)));

  // delay chart layout signals until all objects are created
  connect(_barLengthButtons, SIGNAL(signalBarLengthChanged(int)), this, SLOT(chartUpdated()));
  
  // we have to load the plots before app is shown otherwise
  // dock widgets do not restore properly
  IndicatorDataBase db;
  QStringList l;
  db.indicators(l);
  int loop = 0;
  for (; loop < l.count(); loop++)
    addPlot(l.at(loop));

  setUnifiedTitleAndToolBarOnMac(TRUE);

/*  
  // test start

  QWidget *testw = new QWidget(this);
  testw->setFixedHeight(50);

  QHBoxLayout *hbox = new QHBoxLayout;
  testw->setLayout(hbox);

  dock = new DockWidget(QString(), this);
  dock->setObjectName("testDock");
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dock->setWidget(testw);
  addDockWidget(Qt::LeftDockWidgetArea, dock);

  // test end
*/
}

void QtstalkerApp::createToolBar ()
{
  //construct main toolbar
  QToolBar *toolbar = addToolBar("buttonToolBar");
  toolbar->setObjectName("buttonToolBar");
  toolbar->setIconSize(QSize(16, 16));

  // add this to right justify everything after on the toolbar
  QWidget *spacerWidget = new QWidget;
  spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  spacerWidget->setVisible(true);
  toolbar->addWidget(spacerWidget);

  // create the zoom button box on the main toolbar
  _zoomButtons = new ZoomButtons(toolbar);

//  toolbar->addSeparator();
  
  // create the bar length button group
  _barLengthButtons = new BarLengthButtons(toolbar);

  toolbar->addSeparator();

  // date range controls
  _dateRange = new DateRangeControl(toolbar);
  connect(_dateRange, SIGNAL(signalDateRangeChanged()), this, SLOT(chartUpdated()));

//  toolbar->addSeparator();

  // create recent charts combobox
  _recentCharts = new RecentCharts(toolbar);
  connect(_recentCharts, SIGNAL(signalChartSelected(BarData)), this, SLOT(loadChart(BarData)));
  connect(_quitButton, SIGNAL(signalShutdown()), _recentCharts, SLOT(save()));
}

void QtstalkerApp::createStatusToolBar ()
{
  _statusBar = statusBar();

  _statusToolBar = addToolBar("statusToolBar");
  _statusToolBar->setIconSize(QSize(16, 16));

  // add the toolbar actions
  _quitButton = new QuitButton;
  connect(_quitButton, SIGNAL(signalShutdown()), this, SLOT(save()));
  _statusToolBar->addWidget(_quitButton);

  // grid button
  GridButton *gb = new GridButton;
  _statusToolBar->addWidget(gb);

  // refresh button
  RefreshButton *rb = new RefreshButton;
  connect(rb, SIGNAL(signalRefresh()), this, SLOT(chartUpdated()));
  _statusToolBar->addWidget(rb);

  // toggle crosshairs button
  CrossHairsButton *chb = new CrossHairsButton;
  _statusToolBar->addWidget(chb);

  DataWindowButton *dwb = new DataWindowButton;
  _statusToolBar->addWidget(dwb);

  // new indicator button
  NewIndicatorButton *nib = new NewIndicatorButton;
  _statusToolBar->addWidget(nib);

  // date range button
  _dateRangeButton = new DateRangeButton();
  connect(_dateRangeButton, SIGNAL(signalDateRangeChanged()), this, SLOT(chartUpdated()));
  _statusToolBar->addWidget(_dateRangeButton);

  // docs button
  HelpButton *hb = new HelpButton;
  _statusToolBar->addWidget(hb);

  AboutButton *ab = new AboutButton;
  _statusToolBar->addWidget(ab);

  ConfigureButton *configb = new ConfigureButton;
  _statusToolBar->addWidget(configb);

  _statusBar->addPermanentWidget(_statusToolBar);
}

void QtstalkerApp::loadSettings ()
{
  QSettings settings(g_settingsFile);
  
  restoreGeometry(settings.value("main_window_geometry").toByteArray());
  restoreState(settings.value("main_window_state").toByteArray());

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
  settings.setValue("main_window_geometry", saveGeometry());
  settings.setValue("main_window_state", saveState());
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

//  QSettings settings(g_settingsFile);
//  settings.setValue("current_chart", g_barData->key());
//  settings.setValue("current_chart_length", g_barData->barLength());
//  settings.setValue("current_chart_range", g_barData->range());
//  settings.sync();

  QuoteDataBase db;
  db.getBars(g_barData);

//  _chartLayout->clearIndicator();
  emit signalClearPlot();
  setSliderStart(g_barData->count());
  setWindowTitle(getWindowCaption());
  statusMessage(QString());
//  _chartLayout->loadPlots(_plotSlider->getValue());
  emit signalPlot();
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

  // find the best plot width for either < 1 column or > 1 column
  int width = 0;
  QHashIterator<QString, Plot *> it(_plots);
  while (it.hasNext())
  {
     it.next();
     int t = it.value()->width();
     if (t > width)
       width = t;
  }

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

void QtstalkerApp::addPlot (QString indicator)
{
  Plot *plot = new Plot(indicator, this);
  plot->setIndicator();
  plot->setBarSpacing(_zoomButtons->getPixelSpace());
  plot->loadSettings();

  connect(plot, SIGNAL(signalInfoMessage(Setting)), _infoPanel, SLOT(showInfo(Setting)));
  connect(plot, SIGNAL(signalMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(_zoomButtons, SIGNAL(signalPixelSpace(int)), plot, SLOT(setBarSpacing(int)));
  connect(_plotSlider, SIGNAL(signalValueChanged(int)), plot, SLOT(setStartIndex(int)));
  connect(this, SIGNAL(signalClearPlot()), plot, SLOT(clear()));
//  connect(this, SIGNAL(signalDraw()), plot, SLOT(replot()));
  connect(_quitButton, SIGNAL(signalShutdown()), plot, SLOT(clear()));
  connect(this, SIGNAL(signalPlot()), plot->indicator(), SLOT(calculate()));
  connect(plot, SIGNAL(signalIndex(int)), _plotSlider, SLOT(setStartValue(int)));
  
  connect(g_middleMan, SIGNAL(signalPlotBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));
  connect(g_middleMan, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setFont(QFont)));
  connect(g_middleMan, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));
  connect(g_middleMan, SIGNAL(signalGrid(bool)), plot, SLOT(setGrid(bool)));
  connect(g_middleMan, SIGNAL(signalCrosshairsColor(QColor)), plot, SLOT(setCrossHairsColor(QColor)));
  connect(g_middleMan, SIGNAL(signalCrosshairs(bool)), plot, SLOT(setCrossHairs(bool)));
  connect(g_middleMan, SIGNAL(signalChartObjectDelete(QStringList)), plot, SLOT(deleteChartObject(QStringList)));
  connect(g_middleMan, SIGNAL(signalChartObjectUpdate(QString)), plot, SLOT(updateChartObject(QString)));

  _plots.insert(indicator, plot);
}

void QtstalkerApp::addNewPlot (QString indicator)
{
  addPlot(indicator);
  Plot *p = _plots.value(indicator);
  if (! p)
    return;
  p->indicator()->calculate();
}

void QtstalkerApp::deletePlot (QStringList l)
{
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    if (! _plots.contains(l.at(loop)))
      continue;
    
    Plot *plot = _plots.value(l.at(loop));

    removeDockWidget(plot->dockWidget());
    
    delete plot;
    
    _plots.remove(l.at(loop));
  }
}

void QtstalkerApp::setPlotTabPosition (int position)
{
  setTabPosition(Qt::LeftDockWidgetArea, (QTabWidget::TabPosition) position);
}

void QtstalkerApp::fixDockTabs ()
{
  // we have to expose the non-visible docked tabs before we can draw plots
  // this is just a hack to expose each tab before user notices on startup
  
  int loop = 0;
  QList<QTabBar *> tabList = findChildren<QTabBar *>();
  for (; loop < tabList.count(); loop++)
  {
    QTabBar *tabBar = tabList.at(loop);

    // ignore side panel tabs
    if (! tabBar->objectName().isEmpty())
      continue;

    int currentIndex = tabBar->currentIndex();
    int loop2 = 0;
    for (; loop2 < tabBar->count(); loop2++)
      tabBar->setCurrentIndex(loop2);
    
    tabBar->setCurrentIndex(currentIndex);
  }
}

void QtstalkerApp::updatePlot (QString d)
{
  Plot *p = _plots.value(d);
  if (! p)
    return;

  p->clear();
  
  Indicator *i = p->indicator();
  i->load();
  i->calculate();
}
