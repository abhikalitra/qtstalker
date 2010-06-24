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
#include "qtstalker_defines.h"
#include "IndicatorDataBase.h"
#include "Indicator.h"
#include "IndicatorPluginFactory.h"
#include "DBPluginFactory.h"
#include "IndicatorPlugin.h"
#include "DBPlugin.h"
#include "ExchangeDataBase.h"
#include "FuturesDataBase.h"
#include "PlotLine.h"
//#include "QuoteIndexDataBase.h"
#include "MiscPluginFactory.h"
//#include "GroupDataBase.h"
//#include "ScriptDataBase.h"
//#include "CODataBase.h"

#include <QtDebug>
#include <QtSql>
#include <QDir>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QHash>

Setup::Setup ()
{
}

void Setup::setup (Config &config, QString session)
{
  // setup the disk environment and init databases
  // order is critical here
  setupDirectories(); // initialize directory structure

  config.init(session); // initialize config db

  setupConfigDefaults(config); // initialize config defaults

  // initialize the quotes db
//  QuoteIndexDataBase qidb;
//  QString s = QDir::homePath() + "/.qtstalker/quotes.sqlite";
//  qidb.init(s);

  // initialize data tables
  setupExchanges();
  setupFutures();

  // get complete plugin inventory
  IndicatorPluginFactory ifac;
  ifac.setPluginList();

  DBPluginFactory dbfac;
  dbfac.setPluginList();

  MiscPluginFactory mfac;
  mfac.setPluginList();

  // initialize the groups db
//  GroupDataBase gdb;
//  gdb.init();

  // initialize the scripts db
//  ScriptDataBase sdb;
//  sdb.init();

  // initialize the indicator db
//  IndicatorDataBase idb;
//  idb.init();
  setupDefaultIndicators(config);

  // initialize the chart object db
//  CODataBase codb;
//  codb.init();

  // initialize an example symbol for first run
  setupDefaultSymbol(config);
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
  
  QString s = home + "/data.sqlite";
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "data");
  db.setHostName("me");
  db.setDatabaseName(s);
  db.setUserName("qtstalker");
  db.setPassword("qtstalker");
  if (! db.open())
    qDebug() << "Setup::setupDirectories:" << db.lastError().text();
}

void Setup::setupDefaultIndicators (Config &config)
{
  int ti = config.getInt(Config::DefaultIndicators);
  if (ti)
    return;

  // create the Bars indicator
  IndicatorPluginFactory fac;
  IndicatorDataBase db;
  QString s = "BARS";
  IndicatorPlugin *ip = fac.plugin(s);
  if (! ip)
    qDebug() << "Setup::setupDefaultIndicators: BARS error";
  else
  {
    Indicator i;
    ip->settings(i);
    i.setTabRow(1);
    i.setEnable(1);
    s = "Bars";
    i.setName(s);
    db.setIndicator(i);
  }

  // lets add the Volume indicator
  s = "VOL";
  ip = fac.plugin(s);
  if (! ip)
    qDebug() << "Setup::setupDefaultIndicators: VOL error";
  else
  {
    Indicator i;
    ip->settings(i);
    i.setTabRow(2);
    i.setEnable(1);
    s = "Volume";
    i.setName(s);
    db.setIndicator(i);
  }

  config.setData(Config::DefaultIndicators, 1);
}

void Setup::setupExchanges ()
{
  ExchangeDataBase db;
  db.createExchanges();  
}

void Setup::setupFutures ()
{
  FuturesDataBase db;
  db.createFutures();  
}

void Setup::setupDefaultSymbol (Config &config)
{
  int ti = config.getInt(Config::DefaultSymbol);
  if (ti)
    return;

  DBPluginFactory fac;
  QString s = "Stock";
  DBPlugin *plug = fac.plugin(s);
  if (! plug)
  {
    qDebug() << "Setup::setupDefaultSymbol: plugin error";
    return;
  }

  QFile file("/usr/local/share/qtstalker/db/example_quotes.csv");
  if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug() << "Setup::setupDefaultSymbol: quotes file error";
    return;
  }

  QTextStream in(&file);

  QString command = "QUOTE,Stock,SET_QUOTE,XNYS,SAMPLE,yyyyMMdd,";
  
  while (! in.atEnd())
  {
    s = in.readLine();
    QString s2 = command + s;
    QStringList l = s2.split(",");

    Indicator ind;
    QByteArray ba;
    int rc = plug->scriptCommand(l, ind, ba);
    if (rc)
      qDebug() << "Setup::setupDefaultSymbol: quote not imported" << s2;
  }

  Indicator ind;
  QByteArray ba;
  s = "QUOTE,Stock,SAVE_QUOTES";
  QStringList l = s.split(",");
  int rc = plug->scriptCommand(l, ind, ba);
  if (rc)
    qDebug() << "Setup::setupDefaultSymbol: quotes not saved" << s;

  file.close();

  config.setData(Config::DefaultSymbol, 1);
  
  return;
}

void Setup::setupConfigDefaults (Config &config)
{
  config.transaction();

  // this has to be set before app starts so we know beforehand how many
  // tab rows to construct
  QString d;
  config.getData(Config::IndicatorTabRows, d);
  if (d.isEmpty())
  {
    d = "2";
    config.setData(Config::IndicatorTabRows, d);
  }

/*
  config.getData(Config::IndicatorPluginPath, d);
  if (d.isEmpty())
  {
    d = "/usr/local/lib/qtstalker/plugins/indicator";
    config.setData(Config::IndicatorPluginPath, d);
  }

  config.getData(Config::DBPluginPath, d);
  if (d.isEmpty())
  {
    d = "/usr/local/lib/qtstalker/plugins/database";
    config.setData(Config::DBPluginPath, d);
  }

  config.getData(Config::MiscPluginPath, d);
  if (d.isEmpty())
  {
    d = "/usr/local/lib/qtstalker/plugins/misc";
    config.setData(Config::MiscPluginPath, d);
  }

  config.getData(Config::DbName, d);
  if (d.isEmpty())
  {
    d = QDir::homePath() + "/.qtstalker/quotes.sqlite";
    config.setData(Config::DbName, d);
  }
*/

  // clear current chart to empty
  d = "";
  config.setData(Config::CurrentChart, d);

  config.commit();
}

