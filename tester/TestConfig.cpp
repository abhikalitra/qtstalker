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

#include "TestConfig.h"

#include <QtDebug>
#include <QDir>
#include <QtSql>


TestConfig::TestConfig ()
{
}

void TestConfig::init (QString session)
{
  // create the home directory where all local data is stored
  QDir dir(QDir::homePath());
  QString home = dir.absolutePath();

  home.append("/QtStalkerTest");
  if (! dir.exists(home))
  {
    if (! dir.mkdir(home))
      qDebug() << "Unable to create" << home <<  "directory.";
  }

  QString s = home + "/config.sqlite" + session;
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "config");
  db.setHostName("me");
  db.setDatabaseName(s);
  db.setUserName("QtStalkerTest");
  db.setPassword("QtStalkerTest");
  if (! db.open())
  {
    qDebug() << "TestConfig::init:" << db.lastError().text();
    return;
  }

  createTable();
  setDefaults();
}

void TestConfig::getData (Parm p, QString &d)
{
  getBaseData((int) p, d);
}

void TestConfig::setData (Parm p, QString &d)
{
  setBaseData((int) p, d);
}

void TestConfig::setDefaults ()
{
  transaction();

  QString d;
  getData(DbPlugin, d);
  if (d.isEmpty())
  {
    d = "QSQLITE";
    setData(DbPlugin, d);
  }

  getData(DbHostName, d);
  if (d.isEmpty())
  {
    d = "localhost";
    setData(DbHostName, d);
  }

  getData(DbName, d);
  if (d.isEmpty())
  {
    d = QDir::homePath() + "/.qtstalker/quotes.sqlite";
    setData(DbName, d);
  }

  getData(DbUserName, d);
  if (d.isEmpty())
  {
    d = "qtstalker";
    setData(DbUserName, d);
  }

  getData(DbPassword, d);
  if (d.isEmpty())
  {
    d = "qtstalker";
    setData(DbPassword, d);
  }

  getData(IndicatorPluginPath, d);
  if (d.isEmpty())
  {
    d = "/usr/local/lib/qtstalker/plugins/indicator";
    setData(IndicatorPluginPath, d);
  }

  getData(PlotPluginPath, d);
  if (d.isEmpty())
  {
    d = "/usr/local/lib/qtstalker/plugins/plot";
    setData(PlotPluginPath, d);
  }

  getData(COPluginPath, d);
  if (d.isEmpty())
  {
    d = "/usr/local/lib/qtstalker/plugins/object";
    setData(COPluginPath, d);
  }

  getData(DBPluginPath, d);
  if (d.isEmpty())
  {
    d = "/usr/local/lib/qtstalker/plugins/database";
    setData(DBPluginPath, d);
  }

  commit();
}

