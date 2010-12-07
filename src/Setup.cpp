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
}

void Setup::setupDefaultIndicators ()
{
  QSettings settings;
  settings.beginGroup("main" + g_session);
  
  int ti = settings.value("default_indicators", 0).toInt();
  if (ti)
    return;

  QStringList cl;
  cl << "INDICATOR_DATABASE" << "SAVE" << "Bars" << "/usr/local/share/qtstalker/indicator/Bars.perl";
  cl << "perl" << "1" << "0" << "1";

  Command command(cl.join(","));

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "Setup::setupDefaultIndicators" << command.plugin();
    return;
  }

  plug->command(&command);
  if (command.stringData() == "ERROR")
    qDebug() << "Setup::setupDefaultIndicators: error saving default Bars indicator";

  cl.clear();
  cl << "INDICATOR_DATABASE" << "SAVE" << "VOL" << "/usr/local/share/qtstalker/indicator/Volume.perl";
  cl << "perl" << "2" << "0" << "1";

  command.parse(cl.join(","));

  plug->command(&command);
  
  if (command.stringData() == "ERROR")
    qDebug() << "Setup::setupDefaultIndicators: error saving default Volume indicator";
  
  delete plug;

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

/*
void Setup::setupDataBase ()
{
  QString basePath = QDir::homePath() + "/.qtstalker/";
  QString s = basePath + "quote.sqlite";
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "quote");
  db.setHostName("QtStalker");
  db.setDatabaseName(s);
  db.setUserName("QtStalker");
  db.setPassword("QtStalker");
  if (! db.open())
    qDebug() << "Setup::setupDataBase: quote database" << db.lastError().text();

  s = basePath + "data.sqlite";
  db = QSqlDatabase::addDatabase("QSQLITE", "data");
  db.setHostName("QtStalker");
  db.setDatabaseName(s);
  db.setUserName("QtStalker");
  db.setPassword("QtStalker");
  if (! db.open())
    qDebug() << "Setup::setupDataBase: data database" << db.lastError().text();
}
*/
