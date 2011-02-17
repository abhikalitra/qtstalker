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

#include "Setup.h"
#include "Indicator.h"
#include "Globals.h"
#include "ScriptPluginFactory.h"
#include "BarData.h"
#include "MiddleMan.h"
#include "qtstalker_defines.h"

#include <QtDebug>
#include <QDir>
#include <QStringList>
#include <QSettings>
#include <QtSql>

Setup::Setup ()
{
}

void Setup::setup (QString session)
{
  // set the global variables
  g_session = session;

  // create a dummy barData
  g_barData = new BarData;
  
  // create a dummy barData
  g_middleMan = new MiddleMan(0);

  // setup the disk environment and init databases
  // order is critical here
  setupDirectories(); // initialize directory structure

  // initialize database
//  setupDataBase();

  // initialize data tables
  setupExchanges();

  setupFutures();

  ScriptPluginFactory sfac;
  sfac.setPluginList();

  setupDefaultIndicators();
}

void Setup::setupDirectories ()
{
  QDir dir(QDir::homePath());
  QString home = dir.absolutePath();

  // create the home directory where all local data is stored
  home.append("/.qtstalker");
  if (! dir.exists(home))
  {
    if (! dir.mkdir(home))
      qDebug() << "Unable to create" << home <<  "directory.";
  }

  // setup the config defaults
  g_settingsFile = home + "/qtstalkerrc" + g_session;

  QSettings settings(g_settingsFile);

  // set the system script directory
  QString sysdir = INSTALL_DATA_DIR;
  sysdir.append("/qtstalker/system/");
  settings.setValue("system_script_directory", sysdir);

  QString s = sysdir + "ChartPanelAddGroup.pl";
  settings.setValue("chart_panel_add_group_script", s);

  s = sysdir + "ChartPanelSymbolSearch.pl";
  settings.setValue("chart_panel_symbol_search_script", s);

  s = sysdir + "CrosshairsStatusChanged.pl";
  settings.setValue("crosshairs_status_changed_script", s);

  s = sysdir + "CrosshairsColor.pl";
  settings.setValue("crosshairs_color_script", s);

  s = sysdir + "DataWindow.pl";
  settings.setValue("data_window_script", s);

  s = sysdir + "GridStatusChanged.pl";
  settings.setValue("grid_status_changed_script", s);

  s = sysdir + "GridColor.pl";
  settings.setValue("grid_color_script", s);

  s = sysdir + "GroupPanelNewGroup.pl";
  settings.setValue("group_panel_new_group_script", s);

  s = sysdir + "GroupPanelEditGroup.pl";
  settings.setValue("group_panel_edit_group_script", s);

  s = sysdir + "GroupPanelDeleteGroup.pl";
  settings.setValue("group_panel_delete_group_script", s);

  s = sysdir + "GroupPanelAddToGroup.pl";
  settings.setValue("group_panel_add_to_group_script", s);

  s = sysdir + "IndicatorNew.pl";
  settings.setValue("indicator_new_script", s);

  s = sysdir + "IndicatorEdit.pl";
  settings.setValue("indicator_edit_script", s);

  s = sysdir + "IndicatorDelete.pl";
  settings.setValue("indicator_delete_script", s);

  s = sysdir + "ScriptPanelNewScript.pl";
  settings.setValue("script_panel_new_script_script", s);

  s = sysdir + "ScriptPanelEditScript.pl";
  settings.setValue("script_panel_edit_script_script", s);

  s = sysdir + "ScriptPanelDeleteScript.pl";
  settings.setValue("script_panel_delete_script_script", s);

  s = sysdir + "ScriptPanelRunScript.pl";
  settings.setValue("script_panel_run_script_script", s);

  s = sysdir + "ChartObjectDelete.pl";
  settings.setValue("chart_object_delete_script", s);

  s = sysdir + "ChartObjectDeleteSelect.pl";
  settings.setValue("chart_object_delete_select_script", s);

  s = sysdir + "ChartObjectEdit.pl";
  settings.setValue("chart_object_edit_script", s);

  s = sysdir + "ChartObjectEditSelect.pl";
  settings.setValue("chart_object_edit_select_script", s);

  s = sysdir + "Configure.pl";
  settings.setValue("configure_script", s);

  settings.sync();
}

void Setup::setupDefaultIndicators ()
{
  QSettings settings(g_settingsFile);
  int ti = settings.value("default_indicators", 0).toInt();
  if (ti)
    return;

  Indicator i;
  i.setName("Bars");
  i.setCommand("perl");
  QString s = INSTALL_DATA_DIR;
  s.append("/qtstalker/indicator/Bars.pl");
  i.setScript(s);
  i.setLock(TRUE);
  if (i.save())
  {
    qDebug() << "Setup::setupDefaultIndicators: IndicatorDataBase saveIndex error";
    return;
  }

  i.setName("VOL");
  s = INSTALL_DATA_DIR;
  s.append("/qtstalker/indicator/Volume.pl");
  i.setScript(s);
  i.setLock(TRUE);
  if (i.save())
  {
    qDebug() << "Setup::setupDefaultIndicators: IndicatorDataBase saveIndex error";
    return;
  }

  settings.setValue("default_indicators", 1);
  settings.sync();
}

void Setup::setupExchanges ()
{
/*  
  ExchangeDataBase db;
  db.transaction();
  db.createExchanges();  
  db.commit();
*/
}

void Setup::setupFutures ()
{
/*
  FuturesDataBase db;
  db.transaction();
  db.createFutures();  
  db.commit();
*/
}
