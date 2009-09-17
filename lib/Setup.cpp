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
#include <QObject>


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
  setConfig(k, home);

  k = QString::number(Config::Version);
  setConfig(k, version);

  k = QString::number(Config::Pixelspace);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "6";
    setConfig(k, d);
  }

  k = QString::number(Config::BarLength);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "6";
    setConfig(k, d);
  }

  k = QString::number(Config::Grid);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::BarsToLoad);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "275";
    setConfig(k, d);
  }

  k = QString::number(Config::BackgroundColor);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "#000000"; // black
    setConfig(k, d);
  }

  k = QString::number(Config::BorderColor);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "#ffffff"; // white
    setConfig(k, d);
  }

  k = QString::number(Config::GridColor);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "#555555"; // gray
    setConfig(k, d);
  }

  k = QString::number(Config::Crosshairs);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0";
    setConfig(k, d);
  }

  k = QString::number(Config::DrawMode);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::DataPanelSize);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "338,322";
    setConfig(k, d);
  }

  k = QString::number(Config::ScaleToScreen);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::PlotFont);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "DejaVu Sans,9,50,0,0";
    setConfig(k, d);
  }

  k = QString::number(Config::AppFont);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "DejaVu Sans,9,50,0,0";
    setConfig(k, d);
  }

  k = QString::number(Config::NavAreaSize);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "780,109";
    setConfig(k, d);
  }

  k = QString::number(Config::MainWindowSize);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "950,600";
    setConfig(k, d);
  }

  k = QString::number(Config::MainWindowPos);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "30,30";
    setConfig(k, d);
  }

  k = QString::number(Config::PrefDlgWindowSize);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "600,400";
    setConfig(k, d);
  }

  k = QString::number(Config::PlotSizes);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "339,85";
    setConfig(k, d);
  }

  k = QString::number(Config::PSButton1);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "6";
    setConfig(k, d);
  }

  k = QString::number(Config::PSButton2);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "8";
    setConfig(k, d);
  }

  k = QString::number(Config::PSButton3);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "12";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowSidePanel);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowQuitBtn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowPrefBtn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowSidePanelBtn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowGridBtn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowScaleToScreenBtn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowCrosshairBtn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowDrawModeBtn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowNewIndicatorBtn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowDataWindowBtn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowHelpButton);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowSlider);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowBarsToLoadField);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowBarSpSpinbox);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowCmps60Btn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowCmps15Btn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowCmps5Btn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowCmpsDayBtn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowCmpsWkyBtn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowCmpsMtyBtn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowCmpsComboBox);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::ShowRecentCharts);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }

  k = QString::number(Config::LastChartObjectID);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0";
    setConfig(k, d);
  }

  k = QString::number(Config::IndicatorTabRows);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "2";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DbPlugin);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "QSQLITE";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DbHostName);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "localhost";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DbName);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = QDir::homePath() + "/.CSV/quotes.sqlite";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DbUserName);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "qtstalker";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DbPassword);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "qtstalker";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DbDateColumn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "date";
    setConfig(k, d);
  }

  k = QString::number(Config::DbOpenColumn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "open";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DbHighColumn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "high";
    setConfig(k, d);
  }

  k = QString::number(Config::DbLowColumn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "low";
    setConfig(k, d);
  }

  k = QString::number(Config::DbCloseColumn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "close";
    setConfig(k, d);
  }

  k = QString::number(Config::DbVolumeColumn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "volume";
    setConfig(k, d);
  }

  k = QString::number(Config::DbOIColumn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "oi";
    setConfig(k, d);
  }

  k = QString::number(Config::DbIndexTable);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "symbolIndex";
    setConfig(k, d);
  }

  k = QString::number(Config::DbSymbolColumn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "symbol";
    setConfig(k, d);
  }

  k = QString::number(Config::DbNameColumn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "name";
    setConfig(k, d);
  }

  k = QString::number(Config::DbExchangeColumn);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "exchange";
    setConfig(k, d);
  }

  k = QString::number(Config::DefaultIndicators);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultBuyArrowColor);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "green";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultSellArrowColor);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "red";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultHorizontalLineColor);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "red";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultVerticalLineColor);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "red";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultFiboLineColor);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "red";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultFiboLine1);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0.382";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultFiboLine2);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0.5";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultFiboLine3);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0.618";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultFiboLine4);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultFiboLine5);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultFiboLine6);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0";
    setConfig(k, d);
  }   
  
  k = QString::number(Config::DefaultTextColor);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "white";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultTextFont);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "DejaVu Sans,9,50,0,0";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultTextLabel);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = QObject::tr("Text");
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultTrendLineColor);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "red";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultTrendLineBar);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "Close";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultTrendLineExtend);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "1";
    setConfig(k, d);
  }
  
  k = QString::number(Config::DefaultTrendLineUseBar);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "0";
    setConfig(k, d);
  }

  k = QString::number(Config::IndicatorScriptDefault);
  getConfig(k, d);
  if (d.isEmpty())
  {
    d = "perl /usr/local/share/qtstalker/indicator/";
    setConfig(k, d);
  }
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

