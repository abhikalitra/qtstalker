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

#include <QtDebug>
#include <QApplication>
#include <QSettings>
#include <QStatusBar>
#include <QShortcut>
#include <QTimer>

#include "Qtstalker.h"
#include "Setup.h"
#include "GlobalParent.h"
#include "GlobalPlotGroup.h"
#include "GlobalSidePanel.h"
#include "GlobalControlPanel.h"
#include "GlobalInfoPanel.h"
#include "Global.h"
#include "GlobalSymbol.h"
#include "DockWidget.h"
#include "ChartLoad.h"
#include "KeySymbol.h"
#include "WindowTitle.h"

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

  WindowTitle wt;
  setWindowTitle(wt.mainWindowTitle(-1));

  QTimer::singleShot(100, this, SLOT(afterStartup()));
}


void QtstalkerApp::shutDown ()
{
  // save everything that needs to be saved before app quits
  // some classes cannot save in their destructors due to SQL services
  // shutting down before the destructor is called
  save();
  emit signalShutDown();
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

  KeySymbol keys;
  Data td;
  g_currentSymbol.toData(keys.indexToString(KeySymbol::_SYMBOL), td);
  settings.setValue("current_symbol", td.toString());

  settings.sync();
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

  ChartLoad cl;
  cl.run(_clAsset);
}

void QtstalkerApp::afterStartup ()
{
  // run startup scripts
  g_sidePanel->scriptPanel()->runStartupScripts();
  
  // check if we are going to display a chart from the command line
  if (! _clAsset.isEmpty())
    commandLineAsset();
  else
  {
    // display last viewed chart
    QSettings settings(g_localSettings);
    QString symbol = settings.value("current_symbol").toString();
    if (symbol.isEmpty())
      return;

    ChartLoad cl;
    cl.run(symbol);
  }
}
