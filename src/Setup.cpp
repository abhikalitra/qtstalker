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
#include "Global.h"
#include "qtstalker_defines.h"
#include "ExchangeDataBase.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QDir>
#include <QStringList>
#include <QSettings>

Setup::Setup ()
{
}

void Setup::setup (QObject *, QString session)
{
  // set the global variables
  g_session = session;

  // setup the disk environment and init databases
  // order is critical here
  setupDirectories(); // initialize directory structure

  // setup the system scripts
  setupDefaults();

  QSettings settings(g_localSettings);
  int ti = settings.value("default_indicators", 0).toInt();
  if (! ti)
  {
    setupDefaultIndicators();
    setupDefaultScripts();
    setupExchanges();

    settings.setValue("default_indicators", 1);
    settings.sync();
  }
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
//  QString sysdir = INSTALL_LIB_DIR;
//  sysdir.append("/qtstalker/plugins/script/");
//  settings.setValue("plugin_path", sysdir);

  // set the db ascii file directory
  QString sysdir = INSTALL_DATA_DIR;
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
//  QString s = sysdir + "DataWindow.pl";
//  settings.setValue("data_window_script", s);

  settings.sync();
}

void Setup::setupDefaultIndicators ()
{
  QString base = INSTALL_DATA_DIR;
  base.append("/qtstalker/indicator/");

  IndicatorDataBase i;
  QString s = base + "OHLC_3MA.pl";
  i.add("perl", s);

  s = base + "Volume.pl";
  i.add("perl", s);
}

void Setup::setupDefaultScripts ()
{
  QString name("Yahoo");

  QString file = INSTALL_DATA_DIR;
  file.append("/qtstalker/quote/YahooHistoryDownload.pl");

  QSettings settings(g_localSettings);
  settings.setValue("script_launch_button_command_1", QString("perl"));
  settings.setValue("script_launch_button_file_1", file);
  settings.setValue("script_launch_button_icon_1", QString());
  settings.setValue("script_launch_button_use_icon_1", 0);
  settings.sync();
}

void Setup::setupExchanges ()
{
  ExchangeDataBase db;
  db.createExchanges();
}
