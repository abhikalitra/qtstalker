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
#include "../pics/dirclosed.xpm"
#include "../pics/plainitem.xpm"
#include "../pics/script.xpm"

#include <QDebug>
#include <QSettings>

SidePanel::SidePanel ()
{
  setOrientation(Qt::Vertical);

  // setup the panel tabs
  _tabs = new QTabWidget;
  addWidget(_tabs);

  // setup the plot slider
  _slider = new PlotSlider;
  addWidget(_slider);
  connect (_slider, SIGNAL(signalValueChanged(int)), this, SIGNAL(signalSliderChanged(int)));

  // setup the info panel area
  _info = new InfoPanel;
  addWidget(_info);

  // fill in the panel tabs
  createTabs();
}

SidePanel::~SidePanel ()
{
//  save();
}

// create the chart panel
void SidePanel::createTabs ()
{
  // chart tab
  _chartPanel = new ChartPage;
  connect(_chartPanel, SIGNAL(fileSelected(BarData)), this, SIGNAL(signalLoadChart(BarData)));
  connect(_chartPanel, SIGNAL(addRecentChart(BarData)), this, SIGNAL(signalRecentChart(BarData)));
  connect(_chartPanel, SIGNAL(signalReloadChart()), this, SIGNAL(signalReloadChart()));
  connect(_chartPanel, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));
  connect(g_middleMan, SIGNAL(signalChartPanelRefresh()), _chartPanel, SLOT(updateList()));
  connect(g_middleMan, SIGNAL(signalChartPanelSearch(QString, QString)), _chartPanel, SLOT(setSearch(QString, QString)));
  _tabs->addTab(_chartPanel, QIcon(plainitem), QString());
  _tabs->setTabToolTip(0, tr("Charts"));

  // group tab
  _groupPanel = new GroupPage;
  connect(_groupPanel, SIGNAL(fileSelected(BarData)), this, SIGNAL(signalLoadChart(BarData)));
  connect(_chartPanel, SIGNAL(signalAddToGroup()), _groupPanel, SLOT(updateGroups()));
  connect(_groupPanel, SIGNAL(addRecentChart(BarData)), this, SIGNAL(signalRecentChart(BarData)));
  connect(_groupPanel, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));
  connect(g_middleMan, SIGNAL(signalGroupPanelRefresh()), _groupPanel, SLOT(updateGroups()));
  _tabs->addTab(_groupPanel, QIcon(dirclosed), QString());
  _tabs->setTabToolTip(1, tr("Groups"));

  // script tab
  _scriptPanel = new ScriptPage;
//  connect(_scriptTab, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));
  connect(g_middleMan, SIGNAL(signalScriptRun(QString)), _scriptPanel, SLOT(runScript(QString)));
  _tabs->addTab(_scriptPanel, QIcon(script_xpm), QString());
  _tabs->setTabToolTip(2, tr("Scripts"));
}

void SidePanel::load ()
{
  // we need to load our splitter sizes
  QSettings settings(g_settingsFile);
  QStringList l = settings.value("data_panel_size").toStringList();
  if (! l.count())
  {
    QList<int> l2;
    l2 << 331 << 58 << 85;
    setSizes(l2);
  }
  else
  {
    QList<int> sizeList = sizes();

    int loop = 0;
    for (; loop < l.count(); loop++)
    {
      if (loop >= sizeList.count())
        break;

      if (l[loop].toInt() < 25)
        sizeList[loop] = 25;
      else
        sizeList[loop] = l[loop].toInt();
    }
 
    setSizes(sizeList);
  }

  // set our width
  QSize size = _tabs->size();
  size.setWidth(settings.value("side_panel_tab_width", 100).toInt());
  _tabs->resize(size);

  // set last displayed tab
  _tabs->setCurrentIndex(settings.value("side_panel_last_page", 0).toInt());
}

void SidePanel::save ()
{
  QSettings settings(g_settingsFile);

  QStringList l;
  QList<int> sizeList = sizes();
  int loop;
  for (loop = 0; loop < (int) sizeList.count(); loop++)
    l.append(QString::number(sizeList[loop]));
  settings.setValue("data_panel_size", l);

  QSize size = _tabs->size();
  settings.setValue("side_panel_tab_width", size.width());

  settings.setValue("side_panel_last_page", _tabs->currentIndex());

  settings.sync();
}

PlotSlider * SidePanel::slider ()
{
  return _slider;
}

InfoPanel * SidePanel::info ()
{
  return _info;
}

void SidePanel::toggleStatus (bool status)
{
  if (status)
    this->show();
  else
    this->hide();
}
