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
#include "Config.h"
#include "IndicatorDataBase.h"
#include "Indicator.h"
#include "PluginFactory.h"
#include "IndicatorPlugin.h"
#include "DBPlugin.h"
#include "ExchangeDataBase.h"
#include "FuturesDataBase.h"
#include "PlotLine.h"

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

void Setup::setupDefaultIndicators ()
{
  QString d;
  Config config;
  config.getData(Config::DefaultIndicators, d);
  if (d.toInt())
    return;

  QString path;
  config.getData(Config::IndicatorPluginPath, path);

  // create the Bars indicator
  PluginFactory fac;
  IndicatorDataBase db;
  QString s = "BARS";
  IndicatorPlugin *ip = fac.getIndicator(path, s);
  if (! ip)
    qDebug() << "Setup::setupDefaultIndicators: BARS error";
  else
  {
    Indicator i;
    ip->getSettings(i);
    i.setTabRow(1);
    i.setEnable(1);
    s = "Bars";
    i.setName(s);
    db.setIndicator(i);
  }

  // lets add the Volume indicator
  s = "VOL";
  ip = fac.getIndicator(path, s);
  if (! ip)
    qDebug() << "Setup::setupDefaultIndicators: VOL error";
  else
  {
    Indicator i;
    ip->getSettings(i);
    i.setTabRow(2);
    i.setEnable(1);
    s = "Volume";
    i.setName(s);
    db.setIndicator(i);
  }

  config.setData((int) Config::DefaultIndicators, 1);
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

void Setup::setupDefaultSymbol ()
{
  Config config;
  QString s;
  config.getData(Config::DefaultSymbol, s);
  if (s.toInt())
    return;

  PluginFactory fac;
  QString path;
  s = "Stock";
  config.getData(Config::DBPluginPath, path);
  DBPlugin *plug = fac.getDB(path, s);
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
  QHash<QString, PlotLine *> lines;
  
  while (! in.atEnd())
  {
    s = in.readLine();
    QString s2 = command + s;
    QStringList l = s2.split(",");

   int rc = plug->scriptCommand(l, lines);
   if (rc)
     qDebug() << "Setup::setupDefaultSymbol: quote not imported" << s2;
  }

  s = "QUOTE,Stock,SAVE_QUOTES";
  QStringList l = s.split(",");
  int rc = plug->scriptCommand(l, lines);
  if (rc)
    qDebug() << "Setup::setupDefaultSymbol: quotes not saved" << s;

  file.close();

  config.setData((int) Config::DefaultSymbol, 1);
  
  return;
}

