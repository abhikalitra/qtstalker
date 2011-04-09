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
#include "BarData.h"
#include "MiddleMan.h"
#include "qtstalker_defines.h"
#include "ExchangeDataBase.h"
#include "PluginFactory.h"
#include "Script.h"
#include "DataDataBase.h"

#include <QtDebug>
#include <QDir>
#include <QStringList>
#include <QSettings>
#include <QtSql>

Setup::Setup ()
{
}

void Setup::setup (QObject *p, QString session)
{
  // set the global variables
  g_session = session;

  // create a dummy barData
  g_barData = new BarData;
  
  // create a dummy barData
  g_middleMan = new MiddleMan(p);

  // setup the disk environment and init databases
  // order is critical here
  setupDirectories(); // initialize directory structure

  // setup the system scripts
  setupDefaults();

  // scan plugins
  scanPlugins();
  
  // initialize data tables
  setupExchanges();

//  setupFutures();

  QSettings settings(g_localSettings);
  int ti = settings.value("default_indicators", 0).toInt();
  if (ti)
    return;
  
  setupDefaultIndicators();
  setupDefaultScripts();

  settings.setValue("default_indicators", 1);
  settings.sync();
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
  g_localSettings = "QtStalker/qtstalkerrc" + g_session;
  g_globalSettings = "QtStalker/qtstalkerrc";
}

void Setup::setupDefaults ()
{
  QSettings settings(g_globalSettings);

  // set the plugin directory
  QString sysdir = INSTALL_LIB_DIR;
  sysdir.append("/qtstalker/plugins/script/");
  settings.setValue("plugin_path", sysdir);
  
  // set the db ascii file directory
  sysdir = INSTALL_DATA_DIR;
  sysdir.append("/qtstalker/db/");
  settings.setValue("db_data_directory", sysdir);

  // set the indicator script directory
  sysdir = INSTALL_DATA_DIR;
  sysdir.append("/qtstalker/indicator/");
  settings.setValue("indicator_script_directory", sysdir);

  // set the system script directory
  sysdir = INSTALL_DATA_DIR;
  sysdir.append("/qtstalker/system/");
  settings.setValue("system_script_directory", sysdir);

  // setup the system scripts
  QString s = sysdir + "DataWindow.pl";
  settings.setValue("data_window_script", s);

  settings.sync();
}

void Setup::setupDefaultIndicators ()
{
  QSettings set(g_globalSettings);
  QString s = set.value("db_data_directory").toString();
  s.append("default_indicators");

  DataDataBase db("indicators");
  db.transaction();
  db.import(s);
  db.commit();

  Indicator i;
  i.add("OHLC");
  i.add("VOL");
}

void Setup::setupDefaultScripts ()
{
  QString name("Yahoo");
  QString file = INSTALL_DATA_DIR;
  file.append("/qtstalker/quote/YahooHistoryDownload.pl");
  
  Script script;
  script.setName(name);
  script.setCommand("perl");
  script.setFile(file);
  script.setMinutes(0);
  script.save();

  QSettings settings(g_localSettings);
  settings.setValue("script_launch_button_1", name);
  settings.setValue("script_launch_button_icon_", QString());
  settings.setValue("script_launch_button_use_icon_", 0);
  settings.sync();
}

void Setup::setupExchanges ()
{
  ExchangeDataBase db;
  db.createExchanges();  
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

void Setup::scanPlugins ()
{
  QSettings settings(g_globalSettings);

  QDir dir(settings.value("plugin_path").toString());
  QStringList l = dir.entryList(QDir::NoDotAndDotDot | QDir::Files);

  PluginFactory fac;
  int loop = 0;
  QStringList il;
  QStringList col;
  QStringList cl;
  for (; loop < l.count(); loop++)
  {
    QString s = l.at(loop);
    s = s.remove("lib");
    s = s.remove(".so");
    s = s.remove(".dy");
    
    Plugin *plug = fac.plugin(s);
    if (! plug)
    {
      qDebug() << "Setup::scanPlugins: no plugin" << s;
      continue;
    }

    if (plug->type() == "INDICATOR")
      il << s;
    else if (plug->type() == "CHART_OBJECT")
    {
      il << s;
      col << s;
    }
    else if (plug->type() == "CURVE")
      cl << s;

    delete plug;
  }

  settings.setValue("indicator_plugins", il);
  settings.setValue("chart_object_plugins", col);
  settings.setValue("curve_plugins", cl);
  settings.sync();
}
