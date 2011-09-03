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
#include <QShortcut>
#include <QBuffer>
#include <QDataStream>

#include "Qtstalker.h"
#include "Setup.h"
#include "Globals.h"
#include "QuoteDataBase.h"
#include "ScriptDataBase.h"
#include "InfoPanel.h"
#include "DockWidget.h"
#include "RefreshButton.h"
#include "RecentCharts.h"
#include "BarLengthButton.h"
#include "BarSpaceButton.h"
#include "DateRangeControl.h"
#include "ChartObjectDataBase.h"
#include "CommandAdaptor.h"
#include "CommandInterface.h"
#include "CommandFactory.h"
#include "IPCMessage.h"
#include "SharedMemory.h"
#include "PlotGroup.h"
#include "BarLength.h"
#include "CommandMessage.h"
#include "Symbol.h"

#include "../pics/qtstalker.xpm"

QtstalkerApp::QtstalkerApp (QString session, QString asset)
{
  _clAsset = asset;

  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutDown()));

  setWindowIcon(QIcon(qtstalker_xpm));

  QCoreApplication::setOrganizationName("QtStalker");
  QCoreApplication::setOrganizationDomain("QtStalker.com");
  QCoreApplication::setApplicationName("QtStalker");

  Setup setup;
  setup.setup(this, session);
  g_parent = this;

  createGUI();

  loadSettings();

  setWindowTitle(getWindowCaption());

  // add our D-Bus interface and connect to D-Bus
  new CommandAdaptor(this);
  QDBusConnection::sessionBus().registerObject("/", this);

  com::qtstalker::message *iface;
  iface = new com::qtstalker::message(QString(), QString(), QDBusConnection::sessionBus(), this);
  QDBusConnection::sessionBus().connect(QString(), QString(), "com.qtstalker.message", "message", this, SLOT(messageSlot(QString,QString)));
  connect(iface, SIGNAL(action(QString,QString)), this, SLOT(actionSlot(QString,QString)));

  QTimer::singleShot(100, this, SLOT(afterStartup()));
}

void QtstalkerApp::shutDown ()
{
  // save everything that needs to be saved before app quits
  // some classes cannot save in their destructors due to SQL services
  // shutting down before the destructor is called
  save();
  emit signalShutDown();
  delete g_currentSymbol;
}

void QtstalkerApp::createGUI ()
{
  g_plotGroup = new PlotGroup(this);
  setCentralWidget(g_plotGroup);
  connect(this, SIGNAL(signalLoadSettings()), g_plotGroup, SLOT(loadSettings()));
  connect(this, SIGNAL(signalShutDown()), g_plotGroup, SLOT(saveSettings()));

  setDockNestingEnabled(TRUE);
  setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::East);

  // side panel dock
  g_sidePanel = new SidePanel(this);
  connect(g_sidePanel, SIGNAL(signalLoadChart(QString)), this, SLOT(loadChart(QString)));
  connect(g_sidePanel, SIGNAL(signalReloadChart()), this, SLOT(chartUpdated()));
  connect(g_sidePanel, SIGNAL(signalStatusMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(this, SIGNAL(signalLoadSettings()), g_sidePanel, SLOT(loadSettings()));
  connect(this, SIGNAL(signalShutDown()), g_sidePanel, SLOT(saveSettings()));

  DockWidget *dock = new DockWidget(QString(), this);
  dock->setObjectName("sidePanelDock");
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dock->setWidget(g_sidePanel);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  connect(dock, SIGNAL(signalLockStatus(bool)), g_sidePanel, SLOT(setLockStatus(bool)));
  connect(g_sidePanel, SIGNAL(signalLockStatus(bool)), dock, SLOT(statusChanged(bool)));

  // control panel dock
  g_controlPanel = new ControlPanel;
  connect(this, SIGNAL(signalLoadSettings()), g_controlPanel, SLOT(loadSettings()));
  connect(this, SIGNAL(signalShutDown()), g_controlPanel, SLOT(saveSettings()));
  connect(g_controlPanel->barLengthButton(), SIGNAL(signalBarLengthChanged(int)), this, SLOT(chartUpdated()));
  connect(g_controlPanel->dateRangeControl(), SIGNAL(signalDateRangeChanged()), this, SLOT(chartUpdated()));
  connect(g_controlPanel->recentCharts(), SIGNAL(signalChartSelected(QString)), this, SLOT(loadChart(QString)));
  connect(g_sidePanel, SIGNAL(signalRecentChart(QString)), g_controlPanel->recentCharts(), SLOT(addRecentChart(QString)));
//  connect(g_controlPanel->refreshButton(), SIGNAL(signalRefresh()), this, SLOT(chartUpdated()));
  connect(g_controlPanel->configureButton(), SIGNAL(signalRefresh()), this, SLOT(chartUpdated()));

  dock = new DockWidget(QString(), this);
  dock->setObjectName("controlPanelDock");
  dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock->setWidget(g_controlPanel);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  connect(dock, SIGNAL(signalLockStatus(bool)), g_controlPanel, SLOT(setLockStatus(bool)));
  connect(g_controlPanel, SIGNAL(signalLockStatus(bool)), dock, SLOT(statusChanged(bool)));

  // info panel dock
  g_infoPanel = new InfoPanel;
  connect(this, SIGNAL(signalLoadSettings()), g_infoPanel, SLOT(loadSettings()));
  connect(this, SIGNAL(signalShutDown()), g_infoPanel, SLOT(saveSettings()));

  dock = new DockWidget(QString(), this);
  dock->setObjectName("infoPanelDock");
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dock->setWidget(g_infoPanel);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  connect(dock, SIGNAL(signalLockStatus(bool)), g_infoPanel, SLOT(setLockStatus(bool)));
  connect(g_infoPanel, SIGNAL(signalLockStatus(bool)), dock, SLOT(statusChanged(bool)));

  statusBar()->showMessage(tr("Ready"), 2000);

  setUnifiedTitleAndToolBarOnMac(TRUE);

  // trap page up key
  QShortcut *sc = new QShortcut(this);
  sc->setKey(QKeySequence(Qt::Key_PageUp));
  connect(sc, SIGNAL(activated()), g_controlPanel, SLOT(pageUp()));

  // trap page down key
  sc = new QShortcut(this);
  sc->setKey(QKeySequence(Qt::Key_PageDown));
  connect(sc, SIGNAL(activated()), g_controlPanel, SLOT(pageDown()));
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
//  setTabPosition(Qt::LeftDockWidgetArea, (QTabWidget::TabPosition) settings.value("plot_tab_position", 0).toInt());

  // load gui class settings that need to now
  emit signalLoadSettings();
}

void QtstalkerApp::save()
{
  QSettings settings(g_localSettings);
  settings.setValue("main_window_geometry", saveGeometry());
  settings.setValue("main_window_state", saveState());
  settings.setValue("main_window_size", size());
  settings.setValue("main_window_position", pos());

  if (g_currentSymbol->dataKeyCount())
  {
    QString s = g_currentSymbol->get(Symbol::_EXCHANGE).toString() + ":" + g_currentSymbol->get(Symbol::_SYMBOL).toString();
    settings.setValue("current_symbol", s);
  }

  settings.sync();
}

void QtstalkerApp::loadChart (QString symbol)
{
  // do all the stuff we need to do to load a chart

  QStringList tl = symbol.split(":");
  if (tl.count() != 2)
    return;

  g_currentSymbol->clear();
  g_currentSymbol->set(Symbol::_EXCHANGE, QVariant(tl.at(0)));
  g_currentSymbol->set(Symbol::_SYMBOL, QVariant(tl.at(1)));
  g_currentSymbol->set(Symbol::_LENGTH, QVariant(g_controlPanel->barLengthButton()->length()));
  g_currentSymbol->set(Symbol::_START_DATE, QVariant(QDateTime()));
  g_currentSymbol->set(Symbol::_END_DATE, QVariant(QDateTime()));
  g_currentSymbol->set(Symbol::_RANGE, QVariant(g_controlPanel->dateRangeControl()->dateRange()));

  QuoteDataBase db;
  if (db.getBars(g_currentSymbol))
  {
    qDebug() << "QtstalkerApp::loadChart: QuoteDataBase error";
    return;
  }

//qDebug() << g_currentSymbol->toString();

  SharedMemory sm;
  if (sm.setData(g_sharedCurrentSymbol, g_currentSymbol->toString()))
  {
    qDebug() << "QtstalkerApp::loadChart: error creating shared memory";
    return;
  }

  // update settings file
  QSettings settings(g_localSettings);
  settings.setValue("current_symbol", symbol);
  settings.sync();

  setWindowTitle(getWindowCaption());
  statusMessage(QString());

  g_controlPanel->setStart(g_currentSymbol->barKeyCount(), 0, 0);

  // run indicators
  QStringList il = settings.value("indicators").toStringList();
  int loop = 0;
  for (; loop < il.count(); loop++)
  {
    Script script(this);
    script.setFile(il.at(loop));
    script.setSession(g_session);
    script.run();
  }

  g_sidePanel->setBusyFlag(0);
}

QString QtstalkerApp::getWindowCaption ()
{
  // update the main window text
  QStringList l;
  l << "QtStalker" + g_session + ":";

  if (! g_currentSymbol->dataKeyCount())
    return l.join(" ");

  QString name = g_currentSymbol->get(Symbol::_NAME).toString();
  if (! name.isEmpty())
    l << name;

  QString symbol = g_currentSymbol->get(Symbol::_SYMBOL).toString();
  if (! symbol.isEmpty())
    l << "(" + symbol + ")";

  QStringList bl;
  BarLength b;
  bl = b.list();
  l << bl.at(g_controlPanel->barLengthButton()->length());

  return l.join(" ");
}

void QtstalkerApp::chartUpdated ()
{
  // we are here because something wants us to reload the chart with fresh bars
  QString symbol = g_currentSymbol->get(Symbol::_EXCHANGE).toString();
  if (symbol.isEmpty())
    return;

  symbol.append(":" + g_currentSymbol->get(Symbol::_SYMBOL).toString());
  loadChart(symbol);
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

  QString s = l.at(0) + ":" + l.at(1);
  loadChart(s);
}

/*
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
*/

void QtstalkerApp::afterStartup ()
{
  // expose hidden charts so they plot properly (simple hack)
//  fixDockTabs();

  // check if we are going to display a chart from the command line
  if (! _clAsset.isEmpty())
    commandLineAsset();
  else
  {
    // display last viewed chart
    QSettings settings(g_localSettings);
    loadChart(settings.value("current_symbol").toString());
  }
}

void QtstalkerApp::messageSlot (QString m, QString d)
{
  qDebug() << "QtstalkerApp::messageSlot:" << m;
//  qDebug() << "QtstalkerApp::messageSlot:" << d;

  IPCMessage ipcm;
  if (ipcm.fromString(m))
  {
    qDebug() << "QtstalkerApp::messageSlot: invalid message" << m;
    return;
  }

  // ignore message from another session
  if (ipcm.session() != g_session)
    return;

  CommandMessage cm;
  if (cm.message(ipcm, d))
  {
    qDebug() << "QtstalkerApp::messageSlot: CommandMessage error" << d;
    return;
  }
}

/*
void QtstalkerApp::messageSlot (QString m, QString d)
{
  qDebug() << "QtstalkerApp::messageSlot:" << m;

  IPCMessage ipcm;
  if (ipcm.fromString(m))
  {
    qDebug() << "QtstalkerApp::messageSlot: invalid message" << m;
    return;
  }

  // ignore message from another session
  if (ipcm.session() != g_session)
    return;

  CommandFactory fac;
  Command *com = fac.command(this, ipcm.command());
  if (! com)
  {
    qDebug() << "QtstalkerApp::messageSlot: invalid command" << ipcm.command();
    return;
  }

  com->message(ipcm, d);

  delete com;
}
*/

void QtstalkerApp::actionSlot (QString command, QString d)
{
  qDebug() << "QtstalkerApp::actionSlot:" << command << d;
}
