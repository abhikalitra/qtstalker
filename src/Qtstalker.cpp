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
#include <QStatusBar>

#include "Qtstalker.h"
#include "Setup.h"
#include "Globals.h"
#include "CommandThread.h"
#include "QuoteDataBase.h"
#include "InfoPanel.h"
#include "DockWidget.h"
#include "IndicatorDataBase.h"
#include "RefreshButton.h"
#include "RecentCharts.h"
#include "BarLengthButton.h"
#include "BarSpaceButton.h"
#include "DateRangeControl.h"

#include "../pics/qtstalker.xpm"

QtstalkerApp::QtstalkerApp (QString session, QString asset)
{
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutDown()));

  setWindowIcon(QIcon(qtstalker_xpm));

  QCoreApplication::setOrganizationName("QtStalker");
  QCoreApplication::setOrganizationDomain("QtStalker.com");
  QCoreApplication::setApplicationName("QtStalker");

  Setup setup;
  setup.setup(session);

  createGUI();

  loadSettings();

  setWindowTitle(getWindowCaption());

  // expose hidden charts so they plot properly (simple hack)
//  QTimer::singleShot(500, this, SLOT(fixDockTabs()));

  // check if we are going to display a chart from the command line
  if (asset.length())
  {
    _clAsset = asset;
//    QTimer::singleShot(500, this, SLOT(commandLineAsset()));
    commandLineAsset();
  }
  else
  {
    // display last viewed chart
    QSettings settings(g_localSettings);
    BarData bd;
    bd.setExchange(settings.value("last_symbol_exchange").toString());
    bd.setSymbol(settings.value("last_symbol_symbol").toString());
    loadChart(bd);
  }

  // expose hidden charts so they plot properly (simple hack)
  QTimer::singleShot(100, this, SLOT(fixDockTabs()));
}

void QtstalkerApp::shutDown ()
{
  // save everything that needs to be saved before app quits
  // some classes cannot save in their destructors due to SQL services
  // shutting down before the destructor is called
  save();
  emit signalShutDown();

  // delete all the non-parented objects
  delete g_barData;
  delete g_middleMan;
}

void QtstalkerApp::createGUI ()
{
  setDockNestingEnabled(TRUE);
  setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::East);
  
  connect(g_middleMan, SIGNAL(signalIndicatorNew(QString)), this, SLOT(addNewPlot(QString)));
  connect(g_middleMan, SIGNAL(signalIndicatorDelete(QStringList)), this, SLOT(deletePlot(QStringList)));
  connect(g_middleMan, SIGNAL(signalPlotTabPosition(int)), this, SLOT(setPlotTabPosition(int)));
  connect(g_middleMan, SIGNAL(signalPlotUpdate(QString)), this, SLOT(updatePlot(QString)));
  connect(g_middleMan, SIGNAL(signalStatusMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(g_middleMan, SIGNAL(signalChartObjectNew(QString, QString, QString)), this, SLOT(newChartObject(QString, QString, QString)));

  // side panel dock
  _sidePanel = new SidePanel(this);
  connect(_sidePanel, SIGNAL(signalLoadChart(BarData)), this, SLOT(loadChart(BarData)));
  connect(_sidePanel, SIGNAL(signalReloadChart()), this, SLOT(chartUpdated()));
  connect(_sidePanel, SIGNAL(signalStatusMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(this, SIGNAL(signalLoadSettings()), _sidePanel, SLOT(loadSettings()));
  connect(this, SIGNAL(signalShutDown()), _sidePanel, SLOT(saveSettings()));

  DockWidget *dock = new DockWidget(QString(), this);
  dock->setObjectName("sidePanelDock");
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dock->setWidget(_sidePanel);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  connect(dock, SIGNAL(signalLockStatus(bool)), _sidePanel, SLOT(setLockStatus(bool)));
  connect(_sidePanel, SIGNAL(signalLockStatus(bool)), dock, SLOT(statusChanged(bool)));

  // control panel dock
  _controlPanel = new ControlPanel;
  connect(this, SIGNAL(signalLoadSettings()), _controlPanel, SLOT(loadSettings()));
  connect(this, SIGNAL(signalShutDown()), _controlPanel, SLOT(saveSettings()));
  connect(_controlPanel->barLengthButton(), SIGNAL(signalBarLengthChanged(int)), this, SLOT(chartUpdated()));
  connect(_controlPanel->dateRangeControl(), SIGNAL(signalDateRangeChanged()), this, SLOT(chartUpdated()));
  connect(_controlPanel->recentCharts(), SIGNAL(signalChartSelected(BarData)), this, SLOT(loadChart(BarData)));
  connect(_sidePanel, SIGNAL(signalRecentChart(BarData)), _controlPanel->recentCharts(), SLOT(addRecentChart(BarData)));
  connect(_controlPanel->refreshButton(), SIGNAL(signalRefresh()), this, SLOT(chartUpdated()));
  
  dock = new DockWidget(QString(), this);
  dock->setObjectName("controlPanelDock");
  dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock->setWidget(_controlPanel);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  connect(dock, SIGNAL(signalLockStatus(bool)), _controlPanel, SLOT(setLockStatus(bool)));
  connect(_controlPanel, SIGNAL(signalLockStatus(bool)), dock, SLOT(statusChanged(bool)));

  // info panel dock
  _infoPanel = new InfoPanel;
  connect(this, SIGNAL(signalLoadSettings()), _infoPanel, SLOT(loadSettings()));
  connect(this, SIGNAL(signalShutDown()), _infoPanel, SLOT(saveSettings()));
  
  dock = new DockWidget(QString(), this);
  dock->setObjectName("infoPanelDock");
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dock->setWidget(_infoPanel);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  connect(dock, SIGNAL(signalLockStatus(bool)), _infoPanel, SLOT(setLockStatus(bool)));
  connect(_infoPanel, SIGNAL(signalLockStatus(bool)), dock, SLOT(statusChanged(bool)));

  // we have to load the plots before app is shown otherwise
  // dock widgets do not restore properly
  IndicatorDataBase db;
  QStringList l;
  db.indicators(l);
  int loop = 0;
  for (; loop < l.count(); loop++)
    addPlot(l.at(loop));

  statusBar()->showMessage(tr("Ready"), 2000);

  setUnifiedTitleAndToolBarOnMac(TRUE);
}

void QtstalkerApp::loadSettings ()
{
  QSettings settings(g_localSettings);
  
  restoreGeometry(settings.value("main_window_geometry").toByteArray());
  restoreState(settings.value("main_window_state").toByteArray());

  // restore the size of the app
  QSize sz = settings.value("main_window_size", QSize(800,600)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("main_window_position", QPoint(0,0)).toPoint();
  move(p);

  QStringList l = settings.value("app_font").toString().split(",", QString::SkipEmptyParts);
  if (l.count())
  {
    QFont font;
    font.fromString(l.join(","));
    qApp->setFont(font);
  }

  // set the last used tab position
  setPlotTabPosition(settings.value("plot_tab_position", 0).toInt());

  // load gui class settings that need to now
  emit signalLoadSettings();

  // display last viewed chart
//  BarData bd;
//  bd.setExchange(settings.value("last_symbol_exchange").toString());
//  bd.setSymbol(settings.value("last_symbol_symbol").toString());
//  loadChart(bd);
}

void QtstalkerApp::save()
{
  QSettings settings(g_localSettings);
  settings.setValue("main_window_geometry", saveGeometry());
  settings.setValue("main_window_state", saveState());
  settings.setValue("main_window_size", size());
  settings.setValue("main_window_position", pos());
  settings.setValue("last_symbol_exchange", g_barData->exchange());
  settings.setValue("last_symbol_symbol", g_barData->symbol());
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
  g_barData->setBarLength((BarData::BarLength) _controlPanel->barLengthButton()->length());
  g_barData->setStartDate(QDateTime());
  g_barData->setEndDate(QDateTime());
  g_barData->setRange(_controlPanel->dateRangeControl()->dateRange());

  QuoteDataBase db;
  db.getBars(g_barData);

  emit signalClearPlot();
  setSliderStart(g_barData->count());
  setWindowTitle(getWindowCaption());
  statusMessage(QString());
  emit signalPlot();
}

QString QtstalkerApp::getWindowCaption ()
{
  // update the main window text
  QStringList l;
  l << "QtStalker" + g_session + ":";
  if (! g_barData->name().isEmpty())
    l << g_barData->name();
  if (! g_barData->symbol().isEmpty())
    l << "(" + g_barData->symbol() + ")";

  QStringList bl;
  g_barData->barLengthList(bl);
  l << bl.at(_controlPanel->barLengthButton()->length());

  return l.join(" ");
}

void QtstalkerApp::chartUpdated ()
{
  // we are here because something wants us to reload the chart with fresh bars
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
  statusBar()->showMessage(d, 2000);
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

  _controlPanel->setStart(count, width, 8);
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
  plot->setBarSpacing(_controlPanel->barSpaceButton()->getPixelSpace());
  plot->setBarSpacing(8);
  plot->loadSettings();

  connect(plot, SIGNAL(signalInfoMessage(Setting)), _infoPanel, SLOT(showInfo(Setting)));
  connect(plot, SIGNAL(signalMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(_controlPanel->barSpaceButton(), SIGNAL(signalPixelSpace(int)), plot, SLOT(setBarSpacing(int)));
  connect(_controlPanel, SIGNAL(signalValueChanged(int)), plot, SLOT(setStartIndex(int)));
  connect(this, SIGNAL(signalClearPlot()), plot, SLOT(clear()));
//  connect(this, SIGNAL(signalShutDown()), plot, SLOT(clear()));
  connect(this, SIGNAL(signalPlot()), plot->indicator(), SLOT(calculate()));
  connect(plot, SIGNAL(signalIndex(int)), _controlPanel, SLOT(setStartValue(int)));
  
  connect(g_middleMan, SIGNAL(signalPlotBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));
  connect(g_middleMan, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));
  connect(g_middleMan, SIGNAL(signalGrid(bool)), plot, SLOT(setGrid(bool)));
  connect(g_middleMan, SIGNAL(signalCrosshairsColor(QColor)), plot, SLOT(setCrossHairsColor(QColor)));
  connect(g_middleMan, SIGNAL(signalCrosshairs(bool)), plot, SLOT(setCrossHairs(bool)));
  connect(g_middleMan, SIGNAL(signalChartObjectDelete(QStringList)), plot, SLOT(deleteChartObject(QStringList)));
  connect(g_middleMan, SIGNAL(signalChartObjectUpdate(QString)), plot, SLOT(updateChartObject(QString)));
  connect(g_middleMan, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setFont(QFont)));

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

void QtstalkerApp::newChartObject (QString ind, QString name, QString type)
{
  Plot *p = _plots.value(ind);
  if (! p)
    return;

  p->chartObjectNew(type, name);
}
