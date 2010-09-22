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
#include "IndicatorPlugin.h"
#include "ExchangeDataBase.h"
#include "FuturesDataBase.h"
#include "MiscPluginFactory.h"
#include "QuoteServerRequest.h"
#include "Globals.h"
#include "ChartObjectDataBase.h"
#include "ScriptDataBase.h"
#include "GroupDataBase.h"

#include <QtDebug>
#include <QDir>
#include <QStringList>
#include <QProcess>

Setup::Setup ()
{
}

void Setup::setup (Config &config, QString session)
{
  // set the global variables
  g_session = session;
  
  // setup the disk environment and init databases
  // order is critical here
  setupDirectories(); // initialize directory structure

  // create the database and tables
  config.create(session);
  
  config.init(); // initialize config db

  setupConfigDefaults(config); // initialize config defaults

  // initialize data tables
  setupExchanges();

  setupFutures();

  // initialize chart object tables
  ChartObjectDataBase codb;
  codb.init();

  // initialize group tables
  GroupDataBase gdb;
  gdb.init();
  
  // initialize script tables
  ScriptDataBase sdb;
  sdb.init();

  // get complete plugin inventory
  IndicatorPluginFactory ifac;
  ifac.setPluginList();

  MiscPluginFactory mfac;
  mfac.setPluginList();

  setupDefaultIndicators(config);
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

void Setup::setupDefaultIndicators (Config &config)
{
  int ti = config.getInt(Config::DefaultIndicators);
  if (ti)
    return;

  IndicatorDataBase db;
  db.init();

  db.transaction();
  
  // create the Bars indicator
  IndicatorPluginFactory fac;
  QString s = "BARS";
  IndicatorPlugin *ip = fac.plugin(s);
  if (! ip)
    qDebug() << "Setup::setupDefaultIndicators: BARS error";
  else
  {
    Indicator i;
    i.setIndicator(s);
    i.setTabRow(1);
    i.setColumn(1);
    i.setEnable(1);
    s = "Bars";
    i.setName(s);
    ip->defaults(i);
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
    i.setIndicator(s);
    i.setTabRow(2);
    i.setColumn(1);
    i.setEnable(1);
    s = "Volume";
    i.setName(s);
    ip->defaults(i);
    db.setIndicator(i);
  }

  db.commit();

  config.transaction();
  config.setData(Config::DefaultIndicators, 1);
  config.commit();
}

void Setup::setupExchanges ()
{
  ExchangeDataBase db;
  db.transaction();
  db.createExchanges();  
  db.commit();
}

void Setup::setupFutures ()
{
  FuturesDataBase db;
  db.transaction();
  db.createFutures();  
  db.commit();
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

  // set the quote server hostname
  config.getData(Config::QuoteServerName, d);
  if (d.isEmpty())
  {
    d = "127.0.0.1";
    config.setData(Config::QuoteServerName, d);
  }

  // set the quote server port number
  config.getData(Config::QuoteServerPort, d);
  if (d.isEmpty())
  {
    d = "5000";
    config.setData(Config::QuoteServerPort, d);
  }

  // clear current chart to empty
  d = "";
  config.setData(Config::CurrentChart, d);

  config.commit();
}

void Setup::setupQuoteServer (Config &config)
{
  // create malformed command to get ERROR response
  QStringList tl;
  tl << "Quotes" << "blah";
  QString command = tl.join(",") + "\n";

  QuoteServerRequest qsr;
  qsr.run(command);
  if (qsr.data() == "ERROR")
    return;

  QString serverName;
  config.getData(Config::QuoteServerName, serverName);
  int serverPort = config.getInt(Config::QuoteServerPort);

  command.clear();
  command.append("QuoteServer");
  command.append(" -p ");
  command.append(QString::number(serverPort));
  int rc = QProcess::startDetached(command);
  if (! rc)
  {
    qDebug() << "Setup::setupQuoteServer: error starting the server";
  }
}

