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
#include "Config.h"
#include "../pics/dirclosed.xpm"
#include "../pics/plainitem.xpm"
//#include "../pics/indicator.xpm"
#include "../pics/script.xpm"
#include "../pics/plugin.xpm"

#include <QDebug>

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

// create the chart panel
void SidePanel::createTabs ()
{
  // chart tab
  _chartTab = new ChartPage;
  connect(_chartTab, SIGNAL(fileSelected(BarData)), this, SIGNAL(signalLoadChart(BarData)));
  connect(_chartTab, SIGNAL(addRecentChart(BarData)), this, SIGNAL(signalRecentChart(BarData)));
  connect(_chartTab, SIGNAL(signalReloadChart()), this, SIGNAL(signalReloadChart()));
  connect(_chartTab, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));
  _tabs->addTab(_chartTab, QIcon(plainitem), QString());
  _tabs->setTabToolTip(0, tr("Charts"));

  // group tab
  _groupTab = new GroupPage;
  connect(_groupTab, SIGNAL(fileSelected(BarData)), this, SIGNAL(signalLoadChart(BarData)));
  connect(_chartTab, SIGNAL(signalAddToGroup()), _groupTab, SLOT(updateGroups()));
  connect(_groupTab, SIGNAL(addRecentChart(BarData)), this, SIGNAL(signalRecentChart(BarData)));
  connect(_groupTab, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));
  _tabs->addTab(_groupTab, QIcon(dirclosed), QString());
  _tabs->setTabToolTip(1, tr("Groups"));

  // script tab
  _scriptTab = new ScriptPage;
  connect(_scriptTab, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));
  connect(_scriptTab, SIGNAL(signalScriptDone()), _chartTab, SLOT(updateList()));
  connect(_scriptTab, SIGNAL(signalScriptDone()), _groupTab, SLOT(updateList()));
  _tabs->addTab(_scriptTab, QIcon(script_xpm), QString());
  _tabs->setTabToolTip(3, tr("Scripts"));

  // plugin tab
  _pluginTab = new PluginPage;
  connect(_pluginTab, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));
  connect(_pluginTab, SIGNAL(signalChartRefresh()), this, SIGNAL(signalReloadChart()));
  _tabs->addTab(_pluginTab, QIcon(plugin_xpm), QString());
  _tabs->setTabToolTip(4, tr("Plugins"));
}

void SidePanel::load ()
{
  // we need to load our splitter sizes
  Config config;
  QString s;
  config.getData(Config::DataPanelSize, s);
  if (s.isEmpty())
  {
    QList<int> l;
    l << 331 << 58 << 85;
    
    setSizes(l);
    config.setData(Config::DataPanelSize, (QSplitter *) this);
  }
  else
    config.getData(Config::DataPanelSize, (QSplitter *) this);

  // set our width
  QSize size = _tabs->size();
  int t = config.getInt(Config::SidePanelTabWidth);
  if (! t)
  {
    t = 100;
    config.setData(Config::SidePanelTabWidth, t);
  }
  size.setWidth(t);
  _tabs->resize(size);
}

void SidePanel::save ()
{
  Config config;
  config.setData(Config::DataPanelSize, (QSplitter *) this);

  QSize size = _tabs->size();
  config.setData(Config::SidePanelTabWidth, size.width());
}

PlotSlider * SidePanel::slider ()
{
  return _slider;
}

InfoPanel * SidePanel::info ()
{
  return _info;
}

void SidePanel::updateChartTab ()
{
  _chartTab->updateList();
}

void SidePanel::toggleStatus (bool status)
{
  if (status)
    this->show();
  else
    this->hide();
}

