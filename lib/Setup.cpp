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

void Setup::setup ()
{
//  setupDirectories();
  setupConfigDefaults();
  setupDefaultIndicators();
  setupQuoteBase();
//  setupIndicatorSettings();
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

  config.getData(Config::ShowQuitButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowQuitButton, d);
  }

  config.getData(Config::ShowPrefButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowPrefButton, d);
  }

  config.getData(Config::ShowSidePanelButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowSidePanelButton, d);
  }

  config.getData(Config::ShowGridButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowGridButton, d);
  }

  config.getData(Config::ShowScaleToScreenButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowScaleToScreenButton, d);
  }

  config.getData(Config::ShowCrosshairButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowCrosshairButton, d);
  }

  config.getData(Config::ShowDrawModeButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowDrawModeButton, d);
  }

  config.getData(Config::ShowNewIndicatorButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowNewIndicatorButton, d);
  }

  config.getData(Config::ShowDataWindowButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowDataWindowButton, d);
  }

  config.getData(Config::ShowHelpButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowHelpButton, d);
  }

  config.getData(Config::ShowCmps60Button, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::ShowCmps60Button, d);
  }

  config.getData(Config::ShowCmps15Button, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::ShowCmps15Button, d);
  }

  config.getData(Config::ShowCmps5Button, d);
  if (d.isEmpty())
  {
    d = "0";
    config.setData(Config::ShowCmps5Button, d);
  }

  config.getData(Config::ShowCmpsDayButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowCmpsDayButton, d);
  }

  config.getData(Config::ShowCmpsWkyButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowCmpsWkyButton, d);
  }

  config.getData(Config::ShowCmpsMtyButton, d);
  if (d.isEmpty())
  {
    d = "1";
    config.setData(Config::ShowCmpsMtyButton, d);
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

  config.getData(Config::IndicatorScriptCommand, d);
  if (d.isEmpty())
  {
    d = "perl";
    config.setData(Config::IndicatorScriptCommand, d);
  }

  config.getData(Config::IndicatorScriptPath, d);
  if (d.isEmpty())
  {
    d = INSTALL_DATA_DIR;
    d.append("/qtstalker/indicator");
    config.setData(Config::IndicatorScriptPath, d);
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
  i.setData(Indicator::IndicatorParmTabRow, 1);
  i.setData(Indicator::IndicatorParmEnable, 1);

  QString s = "Bars";
  i.setData(Indicator::IndicatorParmName, s);

  config.getData(Config::IndicatorScriptCommand, s);
  i.setData(Indicator::IndicatorParmCommand, s);

  config.getData(Config::IndicatorScriptPath, s);
  s.append("/Bars.perl");
  i.setData(Indicator::IndicatorParmPath, s);

  DataBase db;
  db.setIndicator(i);

  // lets add the Volume indicator
  i.setData(Indicator::IndicatorParmTabRow, 2);
  i.setData(Indicator::IndicatorParmEnable, 1);

  s = "Volume";
  i.setData(Indicator::IndicatorParmName, s);

  config.getData(Config::IndicatorScriptCommand, s);
  i.setData(Indicator::IndicatorParmCommand, s);

  config.getData(Config::IndicatorScriptPath, s);
  s.append("/Volume.perl");
  i.setData(Indicator::IndicatorParmPath, s);

  db.setIndicator(i);
}

/*
void Setup::setupIndicatorSettings ()
{
  DataBase db;
  db.transaction();

  IndicatorSettings set;
  QString indicator, tn, tv;
  QString fastP(QObject::tr("Fast Period"));
  QString slowP(QObject::tr("Slow Period"));
  QString period(QObject::tr("Period"));
  QString penetration(QObject::tr("Penetration"));
  QString sma(QObject::tr("SMA"));
  QString ma(QObject::tr("MA"));
  QString red("red");
  QString op("=");
  QString deviation(QObject::tr("Deviation"));
  QString output(QObject::tr("Output"));
  QString input(QObject::tr("Input"));
  QString input2(QObject::tr("Input 2"));
  QString close("Close");

  // ACOS settings
  set.clear();
  indicator = "ACOS";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  QString s = QObject::tr("Vector Trigonometric ACos");
  set.setName(s);
  db.setIndicatorSettings(set);

  // AD settings
  set.clear();
  indicator = "AD";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Chaikin Accumulation Distribution Line");
  set.setName(s);
  db.setIndicatorSettings(set);

  // ADD settings
  set.clear();
  indicator = "ADD";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(input2, (int) IndicatorSettings::TypeInput2, close);
  s = QObject::tr("Vector Arithmetic Add");
  set.setName(s);
  db.setIndicatorSettings(set);

  // ADOSC settings
  set.clear();
  indicator = "ADOSC";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(fastP, (int) IndicatorSettings::TypeInt, 3);
  set.addItem(slowP, (int) IndicatorSettings::TypeInt, 10);
  s = QObject::tr("Chaikin Accumulation Distribution Oscillator");
  set.setName(s);
  db.setIndicatorSettings(set);

  // ADX settings
  set.clear();
  indicator = "ADX";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Average Directional Movement Index");
  set.setName(s);
  db.setIndicatorSettings(set);

  // ADXR settings
  set.clear();
  indicator = "ADXR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Average Directional Movement Index Rating");
  set.setName(s);
  db.setIndicatorSettings(set);

  // APO settings
  set.clear();
  indicator = "APO";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(fastP, (int) IndicatorSettings::TypeInt, 12);
  set.addItem(slowP, (int) IndicatorSettings::TypeInt, 26);
  set.addItem(ma, (int) IndicatorSettings::TypeMA, sma);
  s = QObject::tr("Absolute Price Oscillator");
  set.setName(s);
  db.setIndicatorSettings(set);

  // AROON settings
  set.clear();
  indicator = "AROON";
  set.setIndicator(indicator);
  tn = QObject::tr("AROON Down");
  set.addItem(tn, (int) IndicatorSettings::TypeOutput, tn);
  tn = QObject::tr("AROON Up");
  set.addItem(tn, (int) IndicatorSettings::TypeOutput2, tn);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Aroon");
  set.setName(s);
  db.setIndicatorSettings(set);

  // AROONOSC settings
  set.clear();
  indicator = "AROONOSC";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Aroon Oscillator");
  set.setName(s);
  db.setIndicatorSettings(set);

  // ASIN settings
  set.clear();
  indicator = "ASIN";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Trigonometric ASin");
  set.setName(s);
  db.setIndicatorSettings(set);

  // ATAN settings
  set.clear();
  indicator = "ATAN";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Trigonometric ATan");
  set.setName(s);
  db.setIndicatorSettings(set);

  // ATR settings
  set.clear();
  indicator = "ATR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Average True Range");
  set.setName(s);
  db.setIndicatorSettings(set);

  // AVGPRICE settings
  set.clear();
  indicator = "AVGPRICE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Average Price");
  set.setName(s);
  db.setIndicatorSettings(set);

  // BARS settings
  set.clear();
  indicator = "BARS";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  tn = QObject::tr("Up Color");
  tv = "green";
  set.addItem(tn, (int) IndicatorSettings::TypeColor, tv);
  tn = QObject::tr("Down Color");
  tv = "red";
  set.addItem(tn, (int) IndicatorSettings::TypeColor, tv);
  tn = QObject::tr("Neutral Color");
  tv = "blue";
  set.addItem(tn, (int) IndicatorSettings::TypeColor, tv);
  s = QObject::tr("OHLC Price Bars");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CANDLES settings
  set.clear();
  indicator = "CANDLES";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  tn = QObject::tr("Color");
  tv = "green";
  set.addItem(tn, (int) IndicatorSettings::TypeColor, tv);
  s = QObject::tr("Candle Bars");
  set.setName(s);
  db.setIndicatorSettings(set);

  // BBANDS settings
  set.clear();
  indicator = "BBANDS";
  set.setIndicator(indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  tn = QObject::tr("BBANDS Up");
  set.addItem(tn, (int) IndicatorSettings::TypeOutput, tn);
  tn = QObject::tr("BBANDS Mid");
  set.addItem(tn, (int) IndicatorSettings::TypeOutput2, tn);
  tn = QObject::tr("BBANDS Low");
  set.addItem(tn, (int) IndicatorSettings::TypeOutput3, tn);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 5);
  tn = QObject::tr("Upper Deviation");
  set.addItem(tn, (int) IndicatorSettings::TypeDouble, 2.0);
  tn = QObject::tr("Lower Deviation");
  set.addItem(tn, (int) IndicatorSettings::TypeDouble, 2.0);
  set.addItem(ma, (int) IndicatorSettings::TypeMA, sma);
  s = QObject::tr("Bollinger Bands");
  set.setName(s);
  db.setIndicatorSettings(set);

  // BETA settings
  set.clear();
  indicator = "BETA";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(input2, (int) IndicatorSettings::TypeInput2, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 5);
  s = QObject::tr("Beta");
  set.setName(s);
  db.setIndicatorSettings(set);

  // BOP settings
  set.clear();
  indicator = "BOP";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Balance Of Power");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CCI settings
  set.clear();
  indicator = "CCI";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Commodity Channel Index");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDL2CROWS settings
  set.clear();
  indicator = "CDL2CROWS";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Two Crows");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDL3BLACKCROWS settings
  set.clear();
  indicator = "CDL3BLACKCROWS";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Three Black Crows");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDL3INSIDE settings
  set.clear();
  indicator = "CDL3INSIDE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Three Inside Up/Down");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDL3LINESTRIKE settings
  set.clear();
  indicator = "CDL3LINESTRIKE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Three-Line Strike");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDL3OUTSIDE settings
  set.clear();
  indicator = "CDL3OUTSIDE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Three Outside Up/Down");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDL3STARSINSOUTH settings
  set.clear();
  indicator = "CDL3STARSINSOUTH";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Three Stars In The South");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDL3WHITESOLDIERS settings
  set.clear();
  indicator = "CDL3WHITESOLDIERS";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Three Advancing White Soldiers");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLABANDONEDBABY settings
  set.clear();
  indicator = "CDLABANDONEDBABY";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(penetration, (int) IndicatorSettings::TypeDouble, 3.0);
  s = QObject::tr("Abandoned Baby");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLADVANCEBLOCK settings
  set.clear();
  indicator = "CDLADVANCEBLOCK";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Advance Block");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLBELTHOLD settings
  set.clear();
  indicator = "CDLBELTHOLD";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Belt-hold");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLBREAKAWAY settings
  set.clear();
  indicator = "CDLBREAKAWAY";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Breakaway");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLCLOSINGMARUBOZU settings
  set.clear();
  indicator = "CDLCLOSINGMARUBOZU";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Closing Marubozu");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLCONCEALBABYSWALL settings
  set.clear();
  indicator = "CDLCONCEALBABYSWALL";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Concealing Baby Swallow");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLCOUNTERATTACK settings
  set.clear();
  indicator = "CDLCOUNTERATTACK";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Counterattack");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLDARKCLOUDCOVER settings
  set.clear();
  indicator = "CDLDARKCLOUDCOVER";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(penetration, (int) IndicatorSettings::TypeDouble, 3.0);
  s = QObject::tr("Dark Cloud Cover");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLDOJI settings
  set.clear();
  indicator = "CDLDOJI";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Doji");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLDOJISTAR settings
  set.clear();
  indicator = "CDLDOJISTAR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Doji Star");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLDRAGONFLYDOJI settings
  set.clear();
  indicator = "CDLDRAGONFLYDOJI";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Dragonfly Doji");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLENGULFING settings
  set.clear();
  indicator = "CDLENGULFING";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Engulfing Pattern");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLEVENINGDOJISTAR settings
  set.clear();
  indicator = "CDLEVENINGDOJISTAR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(penetration, (int) IndicatorSettings::TypeDouble, 3.0);
  s = QObject::tr("Evening Doji Star");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLEVENINGSTAR settings
  set.clear();
  indicator = "CDLEVENINGSTAR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(penetration, (int) IndicatorSettings::TypeDouble, 3.0);
  s = QObject::tr("Evening Star");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLGAPSIDESIDEWHITE settings
  set.clear();
  indicator = "CDLGAPSIDESIDEWHITE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Up/Down-gap side-by-side white lines");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLGRAVESTONEDOJI settings
  set.clear();
  indicator = "CDLGRAVESTONEDOJI";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Gravestone Doji");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLHAMMER settings
  set.clear();
  indicator = "CDLHAMMER";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Hammer");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLHANGINGMAN settings
  set.clear();
  indicator = "CDLHANGINGMAN";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Hanging Man");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLHARAMI settings
  set.clear();
  indicator = "CDLHARAMI";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Harami Pattern");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLHARAMICROSS settings
  set.clear();
  indicator = "CDLHARAMICROSS";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Harami Cross Pattern");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLHIGHWAVE settings
  set.clear();
  indicator = "CDLHIGHWAVE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("High-Wave Candle");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLHIKKAKE settings
  set.clear();
  indicator = "CDLHIKKAKE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Hikkake Pattern");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLHIKKAKEMOD settings
  set.clear();
  indicator = "CDLHIKKAKEMOD";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Modified Hikkake Pattern");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLHOMINGPIGEON settings
  set.clear();
  indicator = "CDLHOMINGPIGEON";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Homing Pigeon");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLIDENTICAL3CROWS settings
  set.clear();
  indicator = "CDLIDENTICAL3CROWS";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Identical Three Crows");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLINNECK settings
  set.clear();
  indicator = "CDLINNECK";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("In-Neck Pattern");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLINVERTEDHAMMER settings
  set.clear();
  indicator = "CDLINVERTEDHAMMER";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Inverted Hammer");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLKICKING settings
  set.clear();
  indicator = "CDLKICKING";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Kicking");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLKICKINGBYLENGTH settings
  set.clear();
  indicator = "CDLKICKINGBYLENGTH";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Kicking - bull/bear determined by the longer marubozu");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLLADDERBOTTOM settings
  set.clear();
  indicator = "CDLLADDERBOTTOM";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Ladder Bottom");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLLONGLEGGEDDOJI settings
  set.clear();
  indicator = "CDLLONGLEGGEDDOJI";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Long Legged Doji");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLLONGLINE settings
  set.clear();
  indicator = "CDLLONGLINE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Long Line Candle");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLMARUBOZU settings
  set.clear();
  indicator = "CDLMARUBOZU";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Marubozu");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLMATCHINGLOW settings
  set.clear();
  indicator = "CDLMATCHINGLOW";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Matching Low");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLMATHOLD settings
  set.clear();
  indicator = "CDLMATHOLD";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(penetration, (int) IndicatorSettings::TypeDouble, 3.0);
  s = QObject::tr("Mat Hold");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLMORNINGDOJISTAR settings
  set.clear();
  indicator = "CDLMORNINGDOJISTAR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(penetration, (int) IndicatorSettings::TypeDouble, 3.0);
  s = QObject::tr("Morning Doji Star");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLMORNINGSTAR settings
  set.clear();
  indicator = "CDLMORNINGSTAR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(penetration, (int) IndicatorSettings::TypeDouble, 3.0);
  s = QObject::tr("Morning Star");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLONNECK settings
  set.clear();
  indicator = "CDLONNECK";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("On-Neck Pattern");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLPIERCING settings
  set.clear();
  indicator = "CDLPIERCING";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Piercing Pattern");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLRICKSHAWMAN settings
  set.clear();
  indicator = "CDLRICKSHAWMAN";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Rickshaw Man");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLRISEFALL3METHODS settings
  set.clear();
  indicator = "CDLRISEFALL3METHODS";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Rising/Falling Three Methods");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLSEPARATINGLINES settings
  set.clear();
  indicator = "CDLSEPARATINGLINES";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Separating Lines");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLSHOOTINGSTAR settings
  set.clear();
  indicator = "CDLSHOOTINGSTAR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Shooting Star");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLSHORTLINE settings
  set.clear();
  indicator = "CDLSHORTLINE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Short Line Candle");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLSPINNINGTOP settings
  set.clear();
  indicator = "CDLSPINNINGTOP";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Spinning Top");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLSTALLEDPATTERN settings
  set.clear();
  indicator = "CDLSTALLEDPATTERN";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Stalled Pattern");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLSTICKSANDWICH settings
  set.clear();
  indicator = "CDLSTICKSANDWICH";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Stick Sandwich");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLTAKURI settings
  set.clear();
  indicator = "CDLTAKURI";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Takuri (Dragonfly Doji with very long lower shadow)");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLTASUKIGAP settings
  set.clear();
  indicator = "CDLTASUKIGAP";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Tasuki Gap");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLTHRUSTING settings
  set.clear();
  indicator = "CDLTHRUSTING";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Thrusting Pattern");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLTRISTAR settings
  set.clear();
  indicator = "CDLTRISTAR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Tristar Pattern");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLUNIQUE3RIVER settings
  set.clear();
  indicator = "CDLUNIQUE3RIVER";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Unique 3 River");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLUPSIDEGAP2CROWS settings
  set.clear();
  indicator = "CDLUPSIDEGAP2CROWS";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Upside Gap Two Crows");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CDLXSIDEGAP3METHODS settings
  set.clear();
  indicator = "CDLXSIDEGAP3METHODS";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Upside/Downside Gap Three Methods");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CEIL settings
  set.clear();
  indicator = "CEIL";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Ceil");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CMO settings
  set.clear();
  indicator = "CMO";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Chande Momentum Oscillator");
  set.setName(s);
  db.setIndicatorSettings(set);

  // COLOR settings
  set.clear();
  indicator = "COLOR";
  set.setIndicator(indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(input2, (int) IndicatorSettings::TypeInput2, close);
  s = QObject::tr("Value");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0.0);
  s = QObject::tr("Color");
  set.addItem(s, (int) IndicatorSettings::TypeColor, red);
  s = QObject::tr("Color Bars");
  set.setName(s);
  db.setIndicatorSettings(set);

  // COMPARE settings
  set.clear();
  indicator = "COMPARE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(input2, (int) IndicatorSettings::TypeInput2, close);
  s = QObject::tr("Operator");
  set.addItem(s, (int) IndicatorSettings::TypeOperator, op);
  s = QObject::tr("Compare Vectors");
  set.setName(s);
  db.setIndicatorSettings(set);

  // COMPARE2 settings
  set.clear();
  indicator = "COMPARE2";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Value");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0.0);
  s = QObject::tr("Operator");
  set.addItem(s, (int) IndicatorSettings::TypeOperator, op);
  s = QObject::tr("Compare Vector With Value");
  set.setName(s);
  db.setIndicatorSettings(set);

  // CORREL settings
  set.clear();
  indicator = "CORREL";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(input2, (int) IndicatorSettings::TypeInput2, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Pearsons Correlation Coefficient");
  set.setName(s);
  db.setIndicatorSettings(set);

  // COS settings
  set.clear();
  indicator = "COS";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Trigonometric Cos");
  set.setName(s);
  db.setIndicatorSettings(set);

  // COSH settings
  set.clear();
  indicator = "COSH";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Trigonometric Cosh");
  set.setName(s);
  db.setIndicatorSettings(set);

  // DEMA settings
  set.clear();
  indicator = "DEMA";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Double Exponential Moving Average");
  set.setName(s);
  db.setIndicatorSettings(set);

  // DIV settings
  set.clear();
  indicator = "DIV";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(input2, (int) IndicatorSettings::TypeInput2, close);
  s = QObject::tr("Vector Arithmetic Div");
  set.setName(s);
  db.setIndicatorSettings(set);

  // DX settings
  set.clear();
  indicator = "DX";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Directional Movement Index");
  set.setName(s);
  db.setIndicatorSettings(set);

  // EMA settings
  set.clear();
  indicator = "EMA";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Exponential Moving Average");
  set.setName(s);
  db.setIndicatorSettings(set);

  // EXP settings
  set.clear();
  indicator = "EXP";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Arithmetic Exp");
  set.setName(s);
  db.setIndicatorSettings(set);

  // FLOOR settings
  set.clear();
  indicator = "FLOOR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Floor");
  set.setName(s);
  db.setIndicatorSettings(set);

  // HT_DCPERIOD settings
  set.clear();
  indicator = "HT_DCPERIOD";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Hilbert Transform - Dominant Cycle Period");
  set.setName(s);
  db.setIndicatorSettings(set);

  // HT_DCPHASE settings
  set.clear();
  indicator = "HT_DCPHASE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Hilbert Transform - Dominant Cycle Phase");
  set.setName(s);
  db.setIndicatorSettings(set);

  // HT_PHASOR settings
  set.clear();
  indicator = "HT_PHASOR";
  set.setIndicator(indicator);
  s = QObject::tr("In Phase");
  set.addItem(s, (int) IndicatorSettings::TypeOutput, s);
  s = QObject::tr("Quadrature");
  set.addItem(s, (int) IndicatorSettings::TypeOutput2, s);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Hilbert Transform - Phasor Components");
  set.setName(s);
  db.setIndicatorSettings(set);

  // HT_SINE settings
  set.clear();
  indicator = "HT_SINE";
  set.setIndicator(indicator);
  s = QObject::tr("Sine");
  set.addItem(s, (int) IndicatorSettings::TypeOutput, s);
  s = QObject::tr("Lead Sine");
  set.addItem(s, (int) IndicatorSettings::TypeOutput2, s);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Hilbert Transform - SineWave");
  set.setName(s);
  db.setIndicatorSettings(set);

  // HT_TRENDLINE settings
  set.clear();
  indicator = "HT_TRENDLINE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Hilbert Transform - Instantaneous Trendline");
  set.setName(s);
  db.setIndicatorSettings(set);

  // HT_TRENDMODE settings
  set.clear();
  indicator = "HT_TRENDMODE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Hilbert Transform - Trend vs Cycle Mode");
  set.setName(s);
  db.setIndicatorSettings(set);

  // KAMA settings
  set.clear();
  indicator = "KAMA";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Kaufman Adaptive Moving Average");
  set.setName(s);
  db.setIndicatorSettings(set);

  // LINEARREG settings
  set.clear();
  indicator = "LINEARREG";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Linear Regression");
  set.setName(s);
  db.setIndicatorSettings(set);

  // LINEARREG_ANGLE settings
  set.clear();
  indicator = "LINEARREG_ANGLE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Linear Regression Angle");
  set.setName(s);
  db.setIndicatorSettings(set);

  // LINEARREG_INTERCEPT settings
  set.clear();
  indicator = "LINEARREG_INTERCEPT";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Linear Regression Intercept");
  set.setName(s);
  db.setIndicatorSettings(set);

  // LINEARREG_SLOPE settings
  set.clear();
  indicator = "LINEARREG_SLOPE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Linear Regression Slope");
  set.setName(s);
  db.setIndicatorSettings(set);

  // LN settings
  set.clear();
  indicator = "LN";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Log Natural");
  set.setName(s);
  db.setIndicatorSettings(set);

  // LOG10 settings
  set.clear();
  indicator = "LOG10";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Log10");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MACD settings
  set.clear();
  indicator = "MACD";
  set.setIndicator(indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("MACD");
  set.addItem(s, (int) IndicatorSettings::TypeOutput, s);
  s = QObject::tr("MACD Signal");
  set.addItem(s, (int) IndicatorSettings::TypeOutput2, s);
  s = QObject::tr("MACD Hist");
  set.addItem(s, (int) IndicatorSettings::TypeOutput3, s);
  set.addItem(fastP, (int) IndicatorSettings::TypeInt, 12);
  set.addItem(slowP, (int) IndicatorSettings::TypeInt, 26);
  s = QObject::tr("Signal Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 9);
  s = QObject::tr("Moving Average Convergence/Divergence");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MACDEXT settings
  set.clear();
  indicator = "MACDEXT";
  set.setIndicator(indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("MACD");
  set.addItem(s, (int) IndicatorSettings::TypeOutput, s);
  s = QObject::tr("MACD Signal");
  set.addItem(s, (int) IndicatorSettings::TypeOutput2, s);
  s = QObject::tr("MACD Hist");
  set.addItem(s, (int) IndicatorSettings::TypeOutput3, s);
  set.addItem(fastP, (int) IndicatorSettings::TypeInt, 12);
  s = QObject::tr("Fast MA");
  set.addItem(s, (int) IndicatorSettings::TypeMA, sma);
  set.addItem(slowP, (int) IndicatorSettings::TypeInt, 26);
  s = QObject::tr("Slow MA");
  set.addItem(s, (int) IndicatorSettings::TypeMA, sma);
  s = QObject::tr("Signal Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 9);
  s = QObject::tr("Signal MA");
  set.addItem(s, (int) IndicatorSettings::TypeMA, sma);
  s = QObject::tr("MACD with controllable MA type");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MACDFIX settings
  set.clear();
  indicator = "MACDFIX";
  set.setIndicator(indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("MACD");
  set.addItem(s, (int) IndicatorSettings::TypeOutput, s);
  s = QObject::tr("MACD Signal");
  set.addItem(s, (int) IndicatorSettings::TypeOutput2, s);
  s = QObject::tr("MACD Hist");
  set.addItem(s, (int) IndicatorSettings::TypeOutput3, s);
  s = QObject::tr("Signal Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 9);
  s = QObject::tr("Moving Average Convergence/Divergence Fix 12/26");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MAMA settings
  set.clear();
  indicator = "MAMA";
  set.setIndicator(indicator);
  set.addItem(indicator, (int) IndicatorSettings::TypeOutput, indicator);
  s = "FAMA";
  set.addItem(s, (int) IndicatorSettings::TypeOutput2, s);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Fast Limit");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0.5);
  s = QObject::tr("Slow Limit");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0.5);
  s = QObject::tr("MESA Adaptive Moving Average");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MAVP settings
  set.clear();
  indicator = "MAVP";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(input2, (int) IndicatorSettings::TypeInput2, close);
  s = QObject::tr("Min Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 2);
  s = QObject::tr("Max Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 30);
  set.addItem(ma, (int) IndicatorSettings::TypeMA, sma);
  s = QObject::tr("Moving average with variable period");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MAX settings
  set.clear();
  indicator = "MAX";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Highest value over a specified period");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MAXINDEX settings
  set.clear();
  indicator = "MAXINDEX";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Index of highest value over a specified period");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MEDPRICE settings
  set.clear();
  indicator = "MEDPRICE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Median Price");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MFI settings
  set.clear();
  indicator = "MFI";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Money Flow Index");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MIDPOINT settings
  set.clear();
  indicator = "MIDPOINT";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("MidPoint over period");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MIDPRICE settings
  set.clear();
  indicator = "MIDPRICE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Midpoint Price over period");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MIN settings
  set.clear();
  indicator = "MIN";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Lowest value over a specified period");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MININDEX settings
  set.clear();
  indicator = "MININDEX";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Index of lowest value over a specified period");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MINMAX settings
  set.clear();
  indicator = "MINMAX";
  set.setIndicator(indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Min Output");
  set.addItem(s, (int) IndicatorSettings::TypeOutput, s);
  s = QObject::tr("Max Output");
  set.addItem(s, (int) IndicatorSettings::TypeOutput2, s);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Lowest and highest values over a specified period");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MINMAXINDEX settings
  set.clear();
  indicator = "MINMAXINDEX";
  set.setIndicator(indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Min Index Output");
  set.addItem(s, (int) IndicatorSettings::TypeOutput, s);
  s = QObject::tr("Max Index Output");
  set.addItem(s, (int) IndicatorSettings::TypeOutput2, s);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Indexes of lowest and highest values over a specified period");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MINUS_DI settings
  set.clear();
  indicator = "MINUS_DI";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Minus Directional Indicator");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MINUS_DM settings
  set.clear();
  indicator = "MINUS_DM";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Minus Directional Movement");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MOM settings
  set.clear();
  indicator = "MOM";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 10);
  s = QObject::tr("Momentum");
  set.setName(s);
  db.setIndicatorSettings(set);

  // MULT settings
  set.clear();
  indicator = "MULT";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(input2, (int) IndicatorSettings::TypeInput2, close);
  s = QObject::tr("Vector Arithmetic Mult");
  set.setName(s);
  db.setIndicatorSettings(set);

  // NATR settings
  set.clear();
  indicator = "NATR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Normalized Average True Range");
  set.setName(s);
  db.setIndicatorSettings(set);

  // NORMAL settings
  set.clear();
  indicator = "NORMAL";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Normalize Vector");
  set.setName(s);
  db.setIndicatorSettings(set);

  // OBV settings
  set.clear();
  indicator = "OBV";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Volume");
  QString s2 = "Volume";
  set.addItem(s, (int) IndicatorSettings::TypeInput2, s2);
  s = QObject::tr("On Balance Volume");
  set.setName(s);
  db.setIndicatorSettings(set);

  // PLUS_DI settings
  set.clear();
  indicator = "PLUS_DI";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Plus Directional Indicator");
  set.setName(s);
  db.setIndicatorSettings(set);

  // PLUS_DM settings
  set.clear();
  indicator = "PLUS_DM";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Plus Directional Movement");
  set.setName(s);
  db.setIndicatorSettings(set);

  // PPO settings
  set.clear();
  indicator = "PPO";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(fastP, (int) IndicatorSettings::TypeInt, 12);
  set.addItem(slowP, (int) IndicatorSettings::TypeInt, 26);
  set.addItem(ma, (int) IndicatorSettings::TypeMA, sma);
  s = QObject::tr("Percentage Price Oscillator");
  set.setName(s);
  db.setIndicatorSettings(set);

  // REF settings
  set.clear();
  indicator = "REF";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 0);
  s = QObject::tr("Create Vector with Optional Period Displacement");
  set.setName(s);
  db.setIndicatorSettings(set);

  // ROC settings
  set.clear();
  indicator = "ROC";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 10);
  s = QObject::tr("Rate of change : ((price/prevPrice)-1)*100");
  set.setName(s);
  db.setIndicatorSettings(set);

  // ROCP settings
  set.clear();
  indicator = "ROCP";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 10);
  s = QObject::tr("Rate of change Percentage: (price-prevPrice)/prevPrice");
  set.setName(s);
  db.setIndicatorSettings(set);

  // ROCR settings
  set.clear();
  indicator = "ROCR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 10);
  s = QObject::tr("Rate of change ratio: (price/prevPrice)");
  set.setName(s);
  db.setIndicatorSettings(set);

  // ROCR100 settings
  set.clear();
  indicator = "ROCR100";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 10);
  s = QObject::tr("Rate of change ratio 100 scale: (price/prevPrice)*100");
  set.setName(s);
  db.setIndicatorSettings(set);

  // RSI settings
  set.clear();
  indicator = "RSI";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Relative Strength Index");
  set.setName(s);
  db.setIndicatorSettings(set);

  // SAR settings
  set.clear();
  indicator = "SAR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Acceleration");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0.02);
  s = QObject::tr("Maximum");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0.2);
  s = QObject::tr("Parabolic SAR");
  set.setName(s);
  db.setIndicatorSettings(set);

  // SAREXT settings
  set.clear();
  indicator = "SAREXT";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Start Value");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0);
  s = QObject::tr("Offset On Reverse");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0);
  s = QObject::tr("Initial Acceleration Long");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0);
  s = QObject::tr("Acceleration Long");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0);
  s = QObject::tr("Acceleration Max Long");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0);
  s = QObject::tr("Initial Acceleration Short");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0);
  s = QObject::tr("Acceleration Short");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0);
  s = QObject::tr("Acceleration Max Short");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0);
  s = QObject::tr("Parabolic SAR - Extended");
  set.setName(s);
  db.setIndicatorSettings(set);

  // SIN settings
  set.clear();
  indicator = "SIN";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Trigonometric Sin");
  set.setName(s);
  db.setIndicatorSettings(set);

  // SINH settings
  set.clear();
  indicator = "SINH";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Trigonometric Sinh");
  set.setName(s);
  db.setIndicatorSettings(set);

  // SMA settings
  set.clear();
  indicator = "SMA";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Simple Moving Average");
  set.setName(s);
  db.setIndicatorSettings(set);

  // SQRT settings
  set.clear();
  indicator = "SQRT";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Square Root");
  set.setName(s);
  db.setIndicatorSettings(set);

  // STDDEV settings
  set.clear();
  indicator = "STDDEV";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 5);
  s = QObject::tr("Deviation");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 2.0);
  s = QObject::tr("Standard Deviation");
  set.setName(s);
  db.setIndicatorSettings(set);

  // STOCH settings
  set.clear();
  indicator = "STOCH";
  set.setIndicator(indicator);
  s = QObject::tr("SlowK");
  set.addItem(s, (int) IndicatorSettings::TypeOutput, s);
  s = QObject::tr("SlowD");
  set.addItem(s, (int) IndicatorSettings::TypeOutput2, s);
  s = QObject::tr("FastK Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 5);
  s = QObject::tr("SlowK Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 3);
  s = QObject::tr("SlowK MA");
  set.addItem(s, (int) IndicatorSettings::TypeMA, sma);
  s = QObject::tr("SlowD Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 3);
  s = QObject::tr("SlowD MA");
  set.addItem(s, (int) IndicatorSettings::TypeMA, sma);
  s = QObject::tr("Stochastic");
  set.setName(s);
  db.setIndicatorSettings(set);

  // STOCHF settings
  set.clear();
  indicator = "STOCHF";
  set.setIndicator(indicator);
  s = QObject::tr("FastK");
  set.addItem(s, (int) IndicatorSettings::TypeOutput, s);
  s = QObject::tr("FastD");
  set.addItem(s, (int) IndicatorSettings::TypeOutput2, s);
  s = QObject::tr("FastK Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 5);
  s = QObject::tr("FastD Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 3);
  s = QObject::tr("FastD MA");
  set.addItem(s, (int) IndicatorSettings::TypeMA, sma);
  s = QObject::tr("Stochastic Fast");
  set.setName(s);
  db.setIndicatorSettings(set);

  // STOCHRSI settings
  set.clear();
  indicator = "STOCHRSI";
  set.setIndicator(indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("FastK");
  set.addItem(s, (int) IndicatorSettings::TypeOutput, s);
  s = QObject::tr("FastD");
  set.addItem(s, (int) IndicatorSettings::TypeOutput2, s);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("FastK Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 5);
  s = QObject::tr("FastD Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 3);
  s = QObject::tr("FastD MA");
  set.addItem(s, (int) IndicatorSettings::TypeMA, sma);
  s = QObject::tr("Stochastic Relative Strength Index");
  set.setName(s);
  db.setIndicatorSettings(set);

  // SUB settings
  set.clear();
  indicator = "SUB";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(input2, (int) IndicatorSettings::TypeInput2, close);
  s = QObject::tr("Vector Arithmetic Substraction");
  set.setName(s);
  db.setIndicatorSettings(set);

  // SUM settings
  set.clear();
  indicator = "SUM";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Summation");
  set.setName(s);
  db.setIndicatorSettings(set);

  // SYMBOL settings
  set.clear();
  indicator = "SYMBOL";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Symbol");
  s2 = "IBM";
  set.addItem(s, (int) IndicatorSettings::TypeSymbol, s2);
  s = QObject::tr("Bar Field");
  set.addItem(s, (int) IndicatorSettings::TypeText, close);
  s = QObject::tr("Bar Length");
  s2 = QObject::tr("Daily");
  set.addItem(s, (int) IndicatorSettings::TypeText, s2);
  s = QObject::tr("Bars");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 275);
  s = QObject::tr("Create Price Vector");
  set.setName(s);
  db.setIndicatorSettings(set);

  // T3 settings
  set.clear();
  indicator = "T3";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 5);
  s = QObject::tr("Volume Factor");
  set.addItem(s, (int) IndicatorSettings::TypeDouble, 0.7);
  s = QObject::tr("Triple Exponential Moving Average (T3)");
  set.setName(s);
  db.setIndicatorSettings(set);

  // TAN settings
  set.clear();
  indicator = "TAN";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Trigonometric Tan");
  set.setName(s);
  db.setIndicatorSettings(set);

  // TANH settings
  set.clear();
  indicator = "TANH";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  s = QObject::tr("Vector Trigonometric Tanh");
  set.setName(s);
  db.setIndicatorSettings(set);

  // TEMA settings
  set.clear();
  indicator = "TEMA";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Triple Exponential Moving Average");
  set.setName(s);
  db.setIndicatorSettings(set);

  // TRANGE settings
  set.clear();
  indicator = "TRANGE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("True Range");
  set.setName(s);
  db.setIndicatorSettings(set);

  // TRIMA settings
  set.clear();
  indicator = "TRIMA";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Triangular Moving Average");
  set.setName(s);
  db.setIndicatorSettings(set);

  // TRIX settings
  set.clear();
  indicator = "TRIX";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("1-day Rate-Of-Change (ROC) of a Triple Smooth EMA");
  set.setName(s);
  db.setIndicatorSettings(set);

  // TSF settings
  set.clear();
  indicator = "TSF";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Time Series Forecast");
  set.setName(s);
  db.setIndicatorSettings(set);

  // TYPPRICE settings
  set.clear();
  indicator = "TYPPRICE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Typical Price");
  set.setName(s);
  db.setIndicatorSettings(set);

  // ULTOSC settings
  set.clear();
  indicator = "ULTOSC";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Fast Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 7);
  s = QObject::tr("Mid Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Slow Period");
  set.addItem(s, (int) IndicatorSettings::TypeInt, 28);
  s = QObject::tr("Ultimate Oscillator");
  set.setName(s);
  db.setIndicatorSettings(set);

  // VAR settings
  set.clear();
  indicator = "VAR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 5);
  set.addItem(deviation, (int) IndicatorSettings::TypeDouble, 2.0);
  s = QObject::tr("Variance");
  set.setName(s);
  db.setIndicatorSettings(set);

  // WCLPRICE settings
  set.clear();
  indicator = "WCLPRICE";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  s = QObject::tr("Weighted Close Price");
  set.setName(s);
  db.setIndicatorSettings(set);

  // WILLR settings
  set.clear();
  indicator = "WILLR";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 14);
  s = QObject::tr("Williams %R");
  set.setName(s);
  db.setIndicatorSettings(set);

  // WMA settings
  set.clear();
  indicator = "WMA";
  set.setIndicator(indicator);
  set.addItem(output, (int) IndicatorSettings::TypeOutput, indicator);
  set.addItem(input, (int) IndicatorSettings::TypeInput, close);
  set.addItem(period, (int) IndicatorSettings::TypeInt, 30);
  s = QObject::tr("Weighted Moving Average");
  set.setName(s);
  db.setIndicatorSettings(set);

  db.commit();
}
*/

