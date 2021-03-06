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
#include "Global.h"

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
  _chartPanel = new ChartPage(this);
  addTab(_chartPanel, QIcon(chart_xpm), QString());
  setTabToolTip(0, tr("Charts"));

  // group tab
  _groupPanel = new GroupPage(this);
  addTab(_groupPanel, QIcon(group_xpm), QString());
  setTabToolTip(1, tr("Groups"));

  // script tab
  _scriptPanel = new ScriptPage(this);
  addTab(_scriptPanel, QIcon(script_xpm), QString());
  setTabToolTip(2, tr("Scripts"));

  // create shortcut keys for the tabs
  // chart panel
  QShortcut *sc = new QShortcut(this);
  sc->setKey(QKeySequence(Qt::CTRL + Qt::Key_1));
  connect(sc, SIGNAL(activated()), this, SLOT(setChartPanelFocus()));

  // group panel shortcut
  sc = new QShortcut(this);
  sc->setKey(QKeySequence(Qt::CTRL + Qt::Key_2));
  connect(sc, SIGNAL(activated()), this, SLOT(setGroupPanelFocus()));

  // chart panel shortcut
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
//  _scriptPanel->shutDown();
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
  _scriptPanel->list()->setFocus();
}

ChartPage * SidePanel::chartPanel ()
{
  return _chartPanel;
}

GroupPage * SidePanel::groupPanel ()
{
  return _groupPanel;
}

ScriptPage * SidePanel::scriptPanel ()
{
  return _scriptPanel;
}
