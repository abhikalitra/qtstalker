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
//#include "ExchangeDataBase.h"
#include "EAVDataBase.h"
#include "KeyIndicatorDataBase.h"
#include "KeyScriptDataBase.h"
#include "KeyScriptLaunchButton.h"
#include "ScriptFunctions.h"

#include <QtDebug>
#include <QDir>
#include <QStringList>
#include <QSettings>
#include <QUuid>

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

  KeyIndicatorDataBase keys;
  Entity i;
  i.setName(QString("OHLC_3MA"));
  i.set(keys.indexToString(KeyIndicatorDataBase::_SESSION), Data(g_session));
  i.set(keys.indexToString(KeyIndicatorDataBase::_FILE), Data(base + "OHLC_3MA.pl"));
  i.set(keys.indexToString(KeyIndicatorDataBase::_COMMAND), Data(QString("perl")));

  EAVDataBase db("indicators");
  db.transaction();
  if (db.set(i))
  {
    qDebug() << "Setup::setupDefaultIndicators: error saving OHLC";
    return;
  }

  i.setName(QString("Volume"));
  i.set(keys.indexToString(KeyIndicatorDataBase::_SESSION), Data(g_session));
  i.set(keys.indexToString(KeyIndicatorDataBase::_FILE), Data(base + "Volume.pl"));
  i.set(keys.indexToString(KeyIndicatorDataBase::_COMMAND), Data(QString("perl")));

  if (db.set(i))
    qDebug() << "Setup::setupDefaultIndicators: error saving Volume";

  db.commit();
}

void Setup::setupDefaultScripts ()
{
  QString name("Yahoo");
  
  QString file = INSTALL_DATA_DIR;
  file.append("/qtstalker/quote/YahooHistoryDownload.pl");
  
  QString comment = QObject::tr("Downloads historical quotes from yahoo finance.\n\n");
  comment.append(QObject::tr("Symbols to download are kept in a file maintained "));
  comment.append(QObject::tr("by the user. One yahoo symbol per line. For example:\n\n"));
  comment.append(QString("GOOG\nIBM\nAAPL\n"));
  
  KeyScriptDataBase dbkeys;
  ScriptFunctions sf;
  Entity script;
  script.setName(name);
  
  Data td(TypeData::_FILE);
  QStringList tl;
  tl << file;
  td.set(tl);  
  script.set(dbkeys.indexToString(KeyScriptDataBase::_FILE), td);
  
  script.set(dbkeys.indexToString(KeyScriptDataBase::_NAME), Data(name));
  script.set(dbkeys.indexToString(KeyScriptDataBase::_COMMAND), Data(QString("perl")));
  script.set(dbkeys.indexToString(KeyScriptDataBase::_STARTUP), Data(FALSE));
  script.set(dbkeys.indexToString(KeyScriptDataBase::_INTERVAL), Data(0));
  script.set(dbkeys.indexToString(KeyScriptDataBase::_COMMENT), Data(comment));
  script.set(dbkeys.indexToString(KeyScriptDataBase::_LAST_RUN), Data(QDateTime::currentDateTime()));
  if (sf.save(script))
    qDebug() << "Setup::setupDefaultScripts: error saving yahoo script";
  
  KeyScriptLaunchButton lkeys;
  EAVDataBase db("scriptLaunchButtons");
  Entity settings;
  settings.setName(QUuid::createUuid());
  settings.set(lkeys.indexToString(KeyScriptLaunchButton::_SCRIPT), Data(name));
  settings.set(lkeys.indexToString(KeyScriptLaunchButton::_ICON), Data());
  settings.set(lkeys.indexToString(KeyScriptLaunchButton::_ICON_USE), Data(FALSE));
  settings.set(lkeys.indexToString(KeyScriptLaunchButton::_ROW), Data(0));
  settings.set(lkeys.indexToString(KeyScriptLaunchButton::_COL), Data(0));
  settings.set(lkeys.indexToString(KeyScriptLaunchButton::_TEXT), Data(QString("1")));
  if (db.set(settings))
    qDebug() << "Setup::setupDefaultScripts: error saving yahoo launch button";
}

void Setup::setupExchanges ()
{
//  ExchangeDataBase db;
//  db.createExchanges();
}
