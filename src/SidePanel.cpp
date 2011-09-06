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

#include "SidePanel.h"
#include "Globals.h"
#include "ScriptPage.h"
#include "ChartPage.h"
#include "GroupPage.h"

#include "../pics/group.xpm"
#include "../pics/chart.xpm"
#include "../pics/script.xpm"

#include <QDebug>
#include <QSettings>
#include <QShortcut>

SidePanel::SidePanel (QMainWindow *p) : QTabWidget (p)
{
  _lockStatus = TRUE;
  createTabs();
  setTabPosition(QTabWidget::South);
  setMaximumWidth(200);
}

void SidePanel::createTabs ()
{
  // chart tab
  g_chartPanel = new ChartPage(this);
  connect(g_chartPanel, SIGNAL(fileSelected(QString)), this, SIGNAL(signalLoadChart(QString)));
  connect(g_chartPanel, SIGNAL(addRecentChart(QString)), this, SIGNAL(signalRecentChart(QString)));
//  connect(g_chartPanel, SIGNAL(signalReloadChart()), this, SIGNAL(signalReloadChart()));
//  connect(g_chartPanel, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));
//  connect(g_middleMan, SIGNAL(signalChartPanelRefresh()), g_chartPanel, SLOT(updateList()));
//  connect(g_middleMan, SIGNAL(signalChartPanelSearch(QString, QString)), g_chartPanel, SLOT(setSearch(QString, QString)));
  addTab(g_chartPanel, QIcon(chart_xpm), QString());
  setTabToolTip(0, tr("Charts"));

  // group tab
  g_groupPanel = new GroupPage(this);
  connect(g_groupPanel, SIGNAL(fileSelected(QString)), this, SIGNAL(signalLoadChart(QString)));
//  connect(g_chartPanel, SIGNAL(signalAddToGroup()), g_groupPanel, SLOT(updateGroups()));
  connect(g_groupPanel, SIGNAL(addRecentChart(QString)), this, SIGNAL(signalRecentChart(QString)));
  connect(g_groupPanel, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));
//  connect(g_middleMan, SIGNAL(signalGroupPanelRefresh()), g_groupPanel, SLOT(updateGroups()));
  addTab(g_groupPanel, QIcon(group_xpm), QString());
  setTabToolTip(1, tr("Groups"));

  // script tab
  g_scriptPanel = new ScriptPage(this);
//  connect(g_middleMan, SIGNAL(signalScriptRun(QString)), g_scriptPanel, SLOT(runScript(QString)));
  addTab(g_scriptPanel, QIcon(script_xpm), QString());
  setTabToolTip(2, tr("Scripts"));

  // create shortcut keys for the tabs
  // chart panel
  QShortcut *sc = new QShortcut(this);
  sc->setKey(QKeySequence(Qt::CTRL + Qt::Key_1));
  connect(sc, SIGNAL(activated()), this, SLOT(setChartPanelFocus()));

  // group panel
  sc = new QShortcut(this);
  sc->setKey(QKeySequence(Qt::CTRL + Qt::Key_2));
  connect(sc, SIGNAL(activated()), this, SLOT(setGroupPanelFocus()));

  // chart panel
  sc = new QShortcut(this);
  sc->setKey(QKeySequence(Qt::CTRL + Qt::Key_3));
  connect(sc, SIGNAL(activated()), this, SLOT(setScriptPanelFocus()));
}

void SidePanel::loadSettings ()
{
  QSettings settings(g_localSettings);
  _lockStatus = settings.value("side_panel_lock_status", TRUE).toBool();
  emit signalLockStatus(_lockStatus);

  setCurrentIndex(settings.value("side_panel_last_tab", 0).toInt());
}

void SidePanel::saveSettings ()
{
  QSettings settings(g_localSettings);
  settings.setValue("side_panel_lock_status", _lockStatus);
  settings.setValue("side_panel_last_tab", currentIndex());

  // kill any running scripts
  g_scriptPanel->shutDown();
}

void SidePanel::setLockStatus (bool status)
{
  _lockStatus = status;
}

void  SidePanel::setChartPanelFocus ()
{
  setCurrentIndex(0);
//  g_chartPanel->list()->setFocus();
}

void SidePanel::setGroupPanelFocus ()
{
  setCurrentIndex(1);
//  g_groupPanel->list()->setFocus();
}

void SidePanel::setScriptPanelFocus ()
{
  setCurrentIndex(2);
  g_scriptPanel->list()->setFocus();
}

void SidePanel::setBusyFlag (int d)
{
  g_chartPanel->setBusyFlag(d);
  g_groupPanel->setBusyFlag(d);
}

void SidePanel::reloadChart ()
{
  emit signalReloadChart();
}
