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
#include "IndicatorFactory.h"
#include "IndicatorBase.h"
#include "ta_libc.h"


#include <QtDebug>
#include <QtSql>
#include <QDir>
#include <QObject>


Setup::Setup ()
{
}

void Setup::setup ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("TALIB::setDefaults:error on TA_Initialize");

  setupDirectories();
  setupConfigDefaults();
  setupDefaultIndicators();
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
  IndicatorFactory fac;
  DataBase db;
  QString s = "BARS";
  IndicatorBase *ib = fac.getFunction(s);
  if (! ib)
    qDebug() << "Setup::setupDefaultIndicators: BARS error";
  else
  {
    Indicator i;
    ib->getSettings(i);
    i.setTabRow(1);
    i.setEnable(1);
    s = "Bars";
    i.setName(s);
    db.setIndicator(i);
    delete ib;
  }

  // lets add the Volume indicator
  s = "VOL";
  ib = fac.getFunction(s);
  if (! ib)
    qDebug() << "Setup::setupDefaultIndicators: VOL error";
  else
  {
    Indicator i;
    ib->getSettings(i);
    i.setTabRow(2);
    i.setEnable(1);
    s = "Volume";
    i.setName(s);
    db.setIndicator(i);
    delete ib;
  }

  config.setData(Config::DefaultIndicators, 1);
}

