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

#include "Config.h"

#include <QtDebug>
#include <QDir>
#include <QtSql>

Config::Config ()
{
//  version = "0.37";
}

void Config::init (QString session)
{
  QString s = QDir::homePath() + "/.qtstalker/config.sqlite" + session;
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", dbName);
  db.setHostName("me");
  db.setDatabaseName(s);
  db.setUserName("qtstalker");
  db.setPassword("qtstalker");
  if (! db.open())
  {
    qDebug() << "Config::Config:" << db.lastError().text();
    return;
  }

  createTable();

  transaction();
  
  // check if config defaults already exist
  QString d;
  getData(Pixelspace, d);
  if (d.isEmpty())
  {
    d = "6";
    setData(Pixelspace, d);
  }

  getData(BarLength, d);
  if (d.isEmpty())
  {
    d = "6";
    setData(BarLength, d);
  }

  getData(Grid, d);
  if (d.isEmpty())
  {
    d = "1";
    setData(Grid, d);
  }

  getData(BarsToLoad, d);
  if (d.isEmpty())
  {
    d = "275";
    setData(BarsToLoad, d);
  }

  getData(BackgroundColor, d);
  if (d.isEmpty())
  {
    d = "#000000"; // black
    setData(BackgroundColor, d);
  }

  getData(BorderColor, d);
  if (d.isEmpty())
  {
    d = "#ffffff"; // white
    setData(BorderColor, d);
  }

  getData(GridColor, d);
  if (d.isEmpty())
  {
    d = "#555555"; // gray
    setData(GridColor, d);
  }

  getData(Crosshairs, d);
  if (d.isEmpty())
  {
    d = "0";
    setData(Crosshairs, d);
  }

  getData(DrawMode, d);
  if (d.isEmpty())
  {
    d = "1";
    setData(DrawMode, d);
  }

  getData(DataPanelSize, d);
  if (d.isEmpty())
  {
    d = "338,322";
    setData(DataPanelSize, d);
  }

  getData(PlotFont, d);
  if (d.isEmpty())
  {
    d = "DejaVu Sans,9,50,0,0";
    setData(PlotFont, d);
  }

  getData(AppFont, d);
  if (d.isEmpty())
  {
    d = "DejaVu Sans,9,50,0,0";
    setData(AppFont, d);
  }

  getData(NavAreaSize, d);
  if (d.isEmpty())
  {
    d = "800,100";
    setData(NavAreaSize, d);
  }

  getData(MainWindowSize, d);
  if (d.isEmpty())
  {
    d = "800,600";
    setData(MainWindowSize, d);
  }

  getData(MainWindowPos, d);
  if (d.isEmpty())
  {
    d = "30,30";
    setData(MainWindowPos, d);
  }

  getData(PrefDlgWindowSize, d);
  if (d.isEmpty())
  {
    d = "300,200";
    setData(PrefDlgWindowSize, d);
  }

  getData(PlotSizes, d);
  if (d.isEmpty())
  {
    d = "300,80";
    setData(PlotSizes, d);
  }

  getData(PSButton1, d);
  if (d.isEmpty())
  {
    d = "6";
    setData(PSButton1, d);
  }

  getData(PSButton2, d);
  if (d.isEmpty())
  {
    d = "8";
    setData(PSButton2, d);
  }

  getData(ShowRecentCharts, d);
  if (d.isEmpty())
  {
    d = "1";
    setData(ShowRecentCharts, d);
  }

  getData(LastChartObjectID, d);
  if (d.isEmpty())
  {
    d = "0";
    setData(LastChartObjectID, d);
  }

  getData(IndicatorTabRows, d);
  if (d.isEmpty())
  {
    d = "2";
    setData(IndicatorTabRows, d);
  }

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

  getData(DefaultIndicators, d);
  if (d.isEmpty())
  {
    d = "0";
    setData(DefaultIndicators, d);
  }

  getData(DefaultBuyColor, d);
  if (d.isEmpty())
  {
    d = "green";
    setData(DefaultBuyColor, d);
  }

  getData(DefaultSellColor, d);
  if (d.isEmpty())
  {
    d = "red";
    setData(DefaultSellColor, d);
  }

  getData(DefaultHLineColor, d);
  if (d.isEmpty())
  {
    d = "red";
    setData(DefaultHLineColor, d);
  }

  getData(DefaultVLineColor, d);
  if (d.isEmpty())
  {
    d = "red";
    setData(DefaultVLineColor, d);
  }

  getData(DefaultRetracementColor, d);
  if (d.isEmpty())
  {
    d = "red";
    setData(DefaultRetracementColor, d);
  }

  getData(DefaultRetracementLine1, d);
  if (d.isEmpty())
  {
    d = "0.382";
    setData(DefaultRetracementLine1, d);
  }

  getData(DefaultRetracementLine2, d);
  if (d.isEmpty())
  {
    d = "0.5";
    setData(DefaultRetracementLine2, d);
  }

  getData(DefaultRetracementLine3, d);
  if (d.isEmpty())
  {
    d = "0.618";
    setData(DefaultRetracementLine3, d);
  }

  getData(DefaultRetracementLine4, d);
  if (d.isEmpty())
  {
    d = "0";
    setData(DefaultRetracementLine4, d);
  }

  getData(DefaultRetracementLine5, d);
  if (d.isEmpty())
  {
    d = "0";
    setData(DefaultRetracementLine5, d);
  }

  getData(DefaultRetracementLine6, d);
  if (d.isEmpty())
  {
    d = "0";
    setData(DefaultRetracementLine6, d);
  }

  getData(DefaultTextColor, d);
  if (d.isEmpty())
  {
    d = "white";
    setData(DefaultTextColor, d);
  }

  getData(DefaultTextFont, d);
  if (d.isEmpty())
  {
    d = "DejaVu Sans,9,50,0,0";
    setData(DefaultTextFont, d);
  }

  getData(DefaultTextLabel, d);
  if (d.isEmpty())
  {
    d = QObject::tr("Text");
    setData(DefaultTextLabel, d);
  }

  getData(DefaultTLineColor, d);
  if (d.isEmpty())
  {
    d = "red";
    setData(DefaultTLineColor, d);
  }

  getData(DefaultTLineBar, d);
  if (d.isEmpty())
  {
    d = "Close";
    setData(DefaultTLineBar, d);
  }

  getData(DefaultTLineExtend, d);
  if (d.isEmpty())
  {
    d = "1";
    setData(DefaultTLineExtend, d);
  }

  getData(DefaultTLineUseBar, d);
  if (d.isEmpty())
  {
    d = "0";
    setData(DefaultTLineUseBar, d);
  }

  getData(Refresh, d);
  if (d.isEmpty())
  {
    d = "1";
    setData(Refresh, d);
  }

  getData(RefreshStatus, d);
  if (d.isEmpty())
  {
    d = "0";
    setData(RefreshStatus, d);
  }

  getData(IndicatorPluginPath, d);
  if (d.isEmpty())
  {
    d = "/usr/local/lib/qtstalker/plugins/indicator";
    setData(IndicatorPluginPath, d);
  }

  getData(DBPluginPath, d);
  if (d.isEmpty())
  {
    d = "/usr/local/lib/qtstalker/plugins/database";
    setData(DBPluginPath, d);
  }

  getData(ExchangeFileSize, d);
  if (d.isEmpty())
  {
    d = "0";
    setData(ExchangeFileSize, d);
  }

  getData(DefaultSymbol, d);
  if (d.isEmpty())
  {
    d = "0";
    setData(DefaultSymbol, d);
  }

  commit();
}

