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
#include "ChartPage.h"
#include "GroupPage.h"
#include "ScriptPage.h"

#include "../pics/group.xpm"
#include "../pics/chart.xpm"
#include "../pics/script.xpm"

#include <QDebug>
#include <QSettings>

SidePanel::SidePanel ()
{
  _lockStatus = TRUE;
  createTabs();
  setTabPosition(QTabWidget::South);
}

void SidePanel::createTabs ()
{
  // chart tab
  ChartPage *cp = new ChartPage;
  connect(cp, SIGNAL(fileSelected(BarData)), this, SIGNAL(signalLoadChart(BarData)));
  connect(cp, SIGNAL(addRecentChart(BarData)), this, SIGNAL(signalRecentChart(BarData)));
  connect(cp, SIGNAL(signalReloadChart()), this, SIGNAL(signalReloadChart()));
  connect(cp, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));
  connect(g_middleMan, SIGNAL(signalChartPanelRefresh()), cp, SLOT(updateList()));
  connect(g_middleMan, SIGNAL(signalChartPanelSearch(QString, QString)), cp, SLOT(setSearch(QString, QString)));
  addTab(cp, QIcon(chart_xpm), QString());
  setTabToolTip(0, tr("Charts"));

  // group tab
  GroupPage *gp = new GroupPage;
  connect(gp, SIGNAL(fileSelected(BarData)), this, SIGNAL(signalLoadChart(BarData)));
  connect(cp, SIGNAL(signalAddToGroup()), gp, SLOT(updateGroups()));
  connect(gp, SIGNAL(addRecentChart(BarData)), this, SIGNAL(signalRecentChart(BarData)));
  connect(gp, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));
  connect(g_middleMan, SIGNAL(signalGroupPanelRefresh()), gp, SLOT(updateGroups()));
  addTab(gp, QIcon(group_xpm), QString());
  setTabToolTip(1, tr("Groups"));

  // script tab
  ScriptPage *sp = new ScriptPage;
//  connect(_scriptTab, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));
  connect(g_middleMan, SIGNAL(signalScriptRun(QString)), sp, SLOT(runScript(QString)));
  addTab(sp, QIcon(script_xpm), QString());
  setTabToolTip(2, tr("Scripts"));
}

void SidePanel::loadSettings ()
{
  QSettings settings(g_settingsFile);
  _lockStatus = settings.value("side_panel_lock_status", TRUE).toBool();
  emit signalLockStatus(_lockStatus);
}

void SidePanel::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("side_panel_lock_status", _lockStatus);
}

void SidePanel::setLockStatus (bool status)
{
  _lockStatus = status;
}
