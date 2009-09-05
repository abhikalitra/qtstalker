/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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
#include "DataBase.h"
#include "Indicator.h"


#include <QtDebug>
#include <QtSql>
#include <QDir>


Setup::Setup ()
{
}

void Setup::setup (QString session)
{
  setupDirectories();
  setupDataBase(session);
  setupQuoteBase();
}

void Setup::setupDirectories ()
{
  QDir dir(QDir::homePath());
  home = dir.absolutePath();

  // create the home directory where all local data is stored
  home.append("/.qtstalker");
  if (! dir.exists(home))
  {
    if (! dir.mkdir(home))
      qDebug() << "Unable to create" << home <<  "directory.";
  }

  version = "0.37";
}

void Setup::setupDataBase (QString session)
{
  QString s = home + "/data.sqlite" + session;
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "data");
  db.setHostName("me");
  db.setDatabaseName(s);
  db.setUserName("QtStalker");
  db.setPassword("qtstalker");
  if (! db.open())
  {
    qDebug() << "Setup::setup: data db open failed";
    return;
  }

  // create the config table
  QSqlQuery q(db);
  s = "CREATE TABLE IF NOT EXISTS config (key INT PRIMARY KEY, setting TEXT)";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createConfigTable: " << q.lastError().text();
  setupConfigDefaults();

  // create the group index table
  s = "CREATE TABLE IF NOT EXISTS groupIndex (name TEXT PRIMARY KEY, parms TEXT)";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createGroupIndexTable: " << q.lastError().text();

  // create the indicator index table
  s = "CREATE TABLE IF NOT EXISTS indicatorIndex (name TEXT PRIMARY KEY, enable INT, tabRow INT, date INT, log INT, command TEXT)";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createIndicatorIndexTable: " << q.lastError().text();
  setupDefaultIndicators();

  // create the scanners table
//  s = "CREATE TABLE IF NOT EXISTS scanners (name TEXT PRIMARY KEY, parms TEXT, allSymbols INT, fileList TEXT, barLength TEXT, bars INT)";
//  q.exec(s);
//  if (q.lastError().isValid())
//    qDebug() << "DataBase::createScannersTable: " << q.lastError().text();

  // create the testers table
//  s = "CREATE TABLE IF NOT EXISTS testers (name TEXT PRIMARY KEY, parms TEXT, trades TEXT, summary TEXT, el TEXT, xl TEXT, es TEXT, xs TEXT, ls TEXT, ss TEXT)";
//  q.exec(s);
//  if (q.lastError().isValid())
//    qDebug() << "DataBase::createTestersTable: " << q.lastError().text();

  // create the chart object table
  s = "CREATE TABLE IF NOT EXISTS chartObjects (id INT PRIMARY KEY, symbol TEXT, indicator TEXT, type TEXT, settings TEXT)";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createChartObjectsTable: " << q.lastError().text();
}

void Setup::setupQuoteBase ()
{
  QString k = QString::number(Config::DbPlugin);
  QString d;
  getConfig(k, d);
  QSqlDatabase db = QSqlDatabase::addDatabase(d, "quotes");
  
  k = QString::number(Config::DbHostName);
  getConfig(k, d);
  db.setHostName(d);

  k = QString::number(Config::DbName);
  getConfig(k, d);
  db.setDatabaseName(d);

  k = QString::number(Config::DbUserName);
  getConfig(k, d);
  db.setUserName(d);

  k = QString::number(Config::DbPassword);
  getConfig(k, d);
  db.setPassword(d);
  if (! db.open())
    qDebug() << "Setup::setup: quote db open failed";
}

void Setup::setupConfigDefaults ()
{
  // check if config defaults already exist
  QString k, d;
  k = QString::number(Config::Home);
  getConfig(k, d);
  if (d.length())
    return;

  setConfig(k, home);

  k = QString::number(Config::Version);
  setConfig(k, version);

  k = QString::number(Config::Pixelspace);
  d = "6";
  setConfig(k, d);

  k = QString::number(Config::BarLength);
  d = "6";
  setConfig(k, d);

  k = QString::number(Config::Grid);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::BarsToLoad);
  d = "475";
  setConfig(k, d);

  k = QString::number(Config::BackgroundColor);
  d = "#000000"; // black
  setConfig(k, d);

  k = QString::number(Config::BorderColor);
  d = "#ffffff"; // white
  setConfig(k, d);

  k = QString::number(Config::GridColor);
  d = "#555555"; // gray
  setConfig(k, d);

  k = QString::number(Config::Crosshairs);
  d = "0";
  setConfig(k, d);

  k = QString::number(Config::DrawMode);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::DataPanelSize);
  d = "338,322";
  setConfig(k, d);

  k = QString::number(Config::ScaleToScreen);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::PlotFont);
  d = "DejaVu Sans,9,50,0,0";
  setConfig(k, d);

  k = QString::number(Config::AppFont);
  d = "DejaVu Sans,9,50,0,0";
  setConfig(k, d);

  k = QString::number(Config::NavAreaSize);
  d = "780,109";
  setConfig(k, d);

  k = QString::number(Config::MainWindowSize);
  d = "950,600";
  setConfig(k, d);

  k = QString::number(Config::MainWindowPos);
  d = "30,30";
  setConfig(k, d);

  k = QString::number(Config::PrefDlgWindowSize);
  d = "400,400";
  setConfig(k, d);

  k = QString::number(Config::PlotSizes);
  d = "339,85";
  setConfig(k, d);

  k = QString::number(Config::PSButton1);
  d = "6";
  setConfig(k, d);

  k = QString::number(Config::PSButton2);
  d = "8";
  setConfig(k, d);

  k = QString::number(Config::PSButton3);
  d = "12";
  setConfig(k, d);

  k = QString::number(Config::ShowSidePanel);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowQuitBtn);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowPrefBtn);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowSidePanelBtn);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowGridBtn);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowScaleToScreenBtn);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowCrosshairBtn);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowDrawModeBtn);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowNewIndicatorBtn);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowDataWindowBtn);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowHelpButton);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowSlider);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowBarsToLoadField);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowBarSpSpinbox);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowCmps60Btn);
  d = "0";
  setConfig(k, d);

  k = QString::number(Config::ShowCmps15Btn);
  d = "0";
  setConfig(k, d);

  k = QString::number(Config::ShowCmps5Btn);
  d = "0";
  setConfig(k, d);

  k = QString::number(Config::ShowCmpsDayBtn);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowCmpsWkyBtn);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowCmpsMtyBtn);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowCmpsComboBox);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::ShowRecentCharts);
  d = "1";
  setConfig(k, d);

  k = QString::number(Config::LastChartObjectID);
  d = "0";
  setConfig(k, d);

  k = QString::number(Config::IndicatorTabRows);
  d = "2";
  setConfig(k, d);
  
  k = QString::number(Config::DbPlugin);
  d = "QSQLITE";
  setConfig(k, d);
  
  k = QString::number(Config::DbHostName);
  d = "localhost";
  setConfig(k, d);
  
  k = QString::number(Config::DbName);
  d = QDir::homePath() + "/.CSV/quotes.sqlite";
  setConfig(k, d);
  
  k = QString::number(Config::DbUserName);
  d = "qtstalker";
  setConfig(k, d);
  
  k = QString::number(Config::DbPassword);
  d = "qtstalker";
  setConfig(k, d);
  
  k = QString::number(Config::DbDateColumn);
  d = "date";
  setConfig(k, d);

  k = QString::number(Config::DbOpenColumn);
  d = "open";
  setConfig(k, d);

  k = QString::number(Config::DbHighColumn);
  d = "high";
  setConfig(k, d);

  k = QString::number(Config::DbLowColumn);
  d = "low";
  setConfig(k, d);

  k = QString::number(Config::DbCloseColumn);
  d = "close";
  setConfig(k, d);

  k = QString::number(Config::DbVolumeColumn);
  d = "volume";
  setConfig(k, d);

  k = QString::number(Config::DbOIColumn);
  d = "oi";
  setConfig(k, d);

  k = QString::number(Config::DbIndexTable);
  d = "symbolIndex";
  setConfig(k, d);

  k = QString::number(Config::DbSymbolColumn);
  d = "symbol";
  setConfig(k, d);

  k = QString::number(Config::DbNameColumn);
  d = "name";
  setConfig(k, d);

  k = QString::number(Config::DbExchangeColumn);
  d = "exchange";
  setConfig(k, d);

  k = QString::number(Config::DefaultIndicators);
  d = "0";
  setConfig(k, d);
}

void Setup::setConfig (QString &k, QString &d)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO config (key, setting) VALUES (" + k + ",'" + d + "')";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "Setup::setConfig: " << q.lastError().text();
    return;
  }
}

void Setup::getConfig (QString &k, QString &d)
{
  d.clear();
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT setting FROM config WHERE key=" + k;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "Setup::getData: " << q.lastError().text();
    return;
  }

  if (q.next())
    d = q.value(0).toString();
}

void Setup::setupDefaultIndicators ()
{
  QString k = QString::number(Config::DefaultIndicators);
  QString d;
  getConfig(k, d);
  if (d.toInt())
    return;
  
  // create the Bars indicator
  Indicator i;
  i.setTabRow(1);
  i.setEnable(1);
  QString s = "Bars";
  i.setName(s);
  s = "perl ";
  s.append(INSTALL_DATA_DIR);
  s.append("/qtstalker/indicator/Bars.perl");
  i.setCommand(s);
  
  DataBase db;
  db.setIndicator(i);

  // lets add the Volume indicator
  i.setTabRow(2);
  i.setEnable(1);
  s = "Volume";
  i.setName(s);
  s = "perl ";
  s.append(INSTALL_DATA_DIR);
  s.append("/qtstalker/indicator/Volume.perl");
  i.setCommand(s);
  db.setIndicator(i);
}

