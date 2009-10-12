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
  Config config;
  QString d;
  config.getData(Config::DbPlugin, d);
  QSqlDatabase db = QSqlDatabase::addDatabase(d, "quotes");
  
  config.getData(Config::DbHostName, d);
  db.setHostName(d);

  config.getData(Config::DbName, d);
  db.setDatabaseName(d);

  config.getData(Config::DbUserName, d);
  db.setUserName(d);

  config.getData(Config::DbPassword, d);
  db.setPassword(d);
  if (! db.open())
    qDebug() << "Setup::setup: quote db open failed";
}

void Setup::setupConfigDefaults ()
{
  Config config;
  config.transaction();

  // check if config defaults already exist
  QString d;
  config.setData(Config::Home, home);

  config.setData(Config::Version, version);

  config.getData(Config::Pixelspace, d);
  if (d.isEmpty())
  {
    d = "6";
    config.setData(Config::Pixelspace, d);
  }

  config.getData(Config::BarLength, d);
  if (d.isEmpty())
  {
    d = "6";
    config.setData(Config::BarLength, d);
  }

  config.getData(Config::Grid, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::Grid, d);
  }

  config.getData(Config::BarsToLoad, d);
  if (d.isEmpty())
  {
    d = "275";
    config.setData(Config::BarsToLoad, d);
  }

  config.getData(Config::BackgroundColor, d);
  if (d.isEmpty())
  {
    d = "#000000"; // black
    config.setData(Config::BackgroundColor, d);
  }

  config.getData(Config::BorderColor, d);
  if (d.isEmpty())
  {
    d = "#ffffff"; // white
    config.setData(Config::BorderColor, d);
  }

  config.getData(Config::GridColor, d);
  if (d.isEmpty())
  {
    d = "#555555"; // gray
    config.setData(Config::GridColor, d);
  }

  config.getData(Config::Crosshairs, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::Crosshairs, d);
  }

  config.getData(Config::DrawMode, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::DrawMode, d);
  }

  config.getData(Config::DataPanelSize, d);
  if (d.isEmpty())
  {
    d = "338,322";
    config.setData(Config::DataPanelSize, d);
  }

  config.getData(Config::ScaleToScreen, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ScaleToScreen, d);
  }

  config.getData(Config::PlotFont, d);
  if (d.isEmpty())
  {
    d = "DejaVu Sans,9,50,0,0";
    config.setData(Config::PlotFont, d);
  }

  config.getData(Config::AppFont, d);
  if (d.isEmpty())
  {
    d = "DejaVu Sans,9,50,0,0";
    config.setData(Config::AppFont, d);
  }

  config.getData(Config::NavAreaSize, d);
  if (d.isEmpty())
  {
    d = "780,109";
    config.setData(Config::NavAreaSize, d);
  }

  config.getData(Config::MainWindowSize, d);
  if (d.isEmpty())
  {
    d = "950,600";
    config.setData(Config::MainWindowSize, d);
  }

  config.getData(Config::MainWindowPos, d);
  if (d.isEmpty())
  {
    d = "30,30";
    config.setData(Config::MainWindowPos, d);
  }

  config.getData(Config::PrefDlgWindowSize, d);
  if (d.isEmpty())
  {
    d = "600,400";
    config.setData(Config::PrefDlgWindowSize, d);
  }

  config.getData(Config::PlotSizes, d);
  if (d.isEmpty())
  {
    d = "339,85";
    config.setData(Config::PlotSizes, d);
  }

  config.getData(Config::PSButton1, d);
  if (d.isEmpty())
  {
    d = "6";
    config.setData(Config::PSButton1, d);
  }

  config.getData(Config::PSButton2, d);
  if (d.isEmpty())
  {
    d = "8";
    config.setData(Config::PSButton2, d);
  }

  config.getData(Config::ShowSidePanel, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowSidePanel, d);
  }

  config.getData(Config::ShowQuitBtn, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowQuitBtn, d);
  }

  config.getData(Config::ShowPrefBtn, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowPrefBtn, d);
  }

  config.getData(Config::ShowSidePanelBtn, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowSidePanelBtn, d);
  }

  config.getData(Config::ShowGridBtn, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowGridBtn, d);
  }

  config.getData(Config::ShowScaleToScreenBtn, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowScaleToScreenBtn, d);
  }

  config.getData(Config::ShowCrosshairBtn, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowCrosshairBtn, d);
  }

  config.getData(Config::ShowDrawModeBtn, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowDrawModeBtn, d);
  }

  config.getData(Config::ShowNewIndicatorBtn, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowNewIndicatorBtn, d);
  }

  config.getData(Config::ShowDataWindowBtn, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowDataWindowBtn, d);
  }

  config.getData(Config::ShowHelpButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowHelpButton, d);
  }

  config.getData(Config::ShowCmps60Btn, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::ShowCmps60Btn, d);
  }

  config.getData(Config::ShowCmps15Btn, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::ShowCmps15Btn, d);
  }

  config.getData(Config::ShowCmps5Btn, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::ShowCmps5Btn, d);
  }

  config.getData(Config::ShowCmpsDayBtn, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowCmpsDayBtn, d);
  }

  config.getData(Config::ShowCmpsWkyBtn, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowCmpsWkyBtn, d);
  }

  config.getData(Config::ShowCmpsMtyBtn, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowCmpsMtyBtn, d);
  }

  config.getData(Config::ShowRecentCharts, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowRecentCharts, d);
  }

  config.getData(Config::LastChartObjectID, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::LastChartObjectID, d);
  }

  config.getData(Config::IndicatorTabRows, d);
  if (d.isEmpty())
  {
    d = "2";
    config.setData(Config::IndicatorTabRows, d);
  }
  
  config.getData(Config::DbPlugin, d);
  if (d.isEmpty())
  {
    d = "QSQLITE";
    config.setData(Config::DbPlugin, d);
  }
  
  config.getData(Config::DbHostName, d);
  if (d.isEmpty())
  {
    d = "localhost";
    config.setData(Config::DbHostName, d);
  }
  
  config.getData(Config::DbName, d);
  if (d.isEmpty())
  {
    d = QDir::homePath() + "/.CSV/quotes.sqlite";
    config.setData(Config::DbName, d);
  }
  
  config.getData(Config::DbUserName, d);
  if (d.isEmpty())
  {
    d = "qtstalker";
    config.setData(Config::DbUserName, d);
  }
  
  config.getData(Config::DbPassword, d);
  if (d.isEmpty())
  {
    d = "qtstalker";
    config.setData(Config::DbPassword, d);
  }

  config.getData(Config::DbAllSymbols, d);
  if (d.isEmpty())
  {
    d = "SELECT symbol FROM symbolIndex";
    config.setData(Config::DbAllSymbols, d);
  }

  config.getData(Config::DbSearchSymbols, d);
  if (d.isEmpty())
  {
    d = "SELECT symbol FROM symbolIndex WHERE symbol LIKE";
    config.setData(Config::DbSearchSymbols, d);
  }

  config.getData(Config::DbFirstDate, d);
  if (d.isEmpty())
  {
    d = "SELECT min(date) FROM $symbol";
    config.setData(Config::DbFirstDate, d);
  }

  config.getData(Config::DbLastDate, d);
  if (d.isEmpty())
  {
    d = "SELECT max(date) FROM $symbol";
    config.setData(Config::DbLastDate, d);
  }

  config.getData(Config::DbGetSymbol, d);
  if (d.isEmpty())
  {
    d = "SELECT date,open,high,low,close,volume,oi FROM $symbol WHERE date >= $sd AND date <= $ed ORDER BY date DESC LIMIT $records";
    config.setData(Config::DbGetSymbol, d);
  }

  config.getData(Config::DefaultIndicators, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::DefaultIndicators, d);
  }
  
  config.getData(Config::DefaultBuyArrowColor, d);
  if (d.isEmpty())
  {
    d = "green";
    config.setData(Config::DefaultBuyArrowColor, d);
  }
  
  config.getData(Config::DefaultSellArrowColor, d);
  if (d.isEmpty())
  {
    d = "red";
    config.setData(Config::DefaultSellArrowColor, d);
  }
  
  config.getData(Config::DefaultHorizontalLineColor, d);
  if (d.isEmpty())
  {
    d = "red";
    config.setData(Config::DefaultHorizontalLineColor, d);
  }
  
  config.getData(Config::DefaultVerticalLineColor, d);
  if (d.isEmpty())
  {
    d = "red";
    config.setData(Config::DefaultVerticalLineColor, d);
  }
  
  config.getData(Config::DefaultFiboLineColor, d);
  if (d.isEmpty())
  {
    d = "red";
    config.setData(Config::DefaultFiboLineColor, d);
  }
  
  config.getData(Config::DefaultFiboLine1, d);
  if (d.isEmpty())
  {
    d = "0.382";
    config.setData(Config::DefaultFiboLine1, d);
  }
  
  config.getData(Config::DefaultFiboLine2, d);
  if (d.isEmpty())
  {
    d = "0.5";
    config.setData(Config::DefaultFiboLine2, d);
  }
  
  config.getData(Config::DefaultFiboLine3, d);
  if (d.isEmpty())
  {
    d = "0.618";
    config.setData(Config::DefaultFiboLine3, d);
  }
  
  config.getData(Config::DefaultFiboLine4, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::DefaultFiboLine4, d);
  }
  
  config.getData(Config::DefaultFiboLine5, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::DefaultFiboLine5, d);
  }
  
  config.getData(Config::DefaultFiboLine6, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::DefaultFiboLine6, d);
  }   
  
  config.getData(Config::DefaultTextColor, d);
  if (d.isEmpty())
  {
    d = "white";
    config.setData(Config::DefaultTextColor, d);
  }
  
  config.getData(Config::DefaultTextFont, d);
  if (d.isEmpty())
  {
    d = "DejaVu Sans,9,50,0,0";
    config.setData(Config::DefaultTextFont, d);
  }
  
  config.getData(Config::DefaultTextLabel, d);
  if (d.isEmpty())
  {
    d = QObject::tr("Text");
    config.setData(Config::DefaultTextLabel, d);
  }
  
  config.getData(Config::DefaultTrendLineColor, d);
  if (d.isEmpty())
  {
    d = "red";
    config.setData(Config::DefaultTrendLineColor, d);
  }
  
  config.getData(Config::DefaultTrendLineBar, d);
  if (d.isEmpty())
  {
    d = "Close";
    config.setData(Config::DefaultTrendLineBar, d);
  }
  
  config.getData(Config::DefaultTrendLineExtend, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::DefaultTrendLineExtend, d);
  }
  
  config.getData(Config::DefaultTrendLineUseBar, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::DefaultTrendLineUseBar, d);
  }

  config.getData(Config::IndicatorScriptDefault, d);
  if (d.isEmpty())
  {
    d = "perl /usr/local/share/qtstalker/indicator/";
    config.setData(Config::IndicatorScriptDefault, d);
  }

  config.getData(Config::Refresh, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::Refresh, d);
  }

  config.getData(Config::RefreshStatus, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::RefreshStatus, d);
  }

  config.commit();
}

void Setup::setupDefaultIndicators ()
{
  QString d;
  Config config;
  config.getData(Config::DefaultIndicators, d);
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

