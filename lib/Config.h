/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <qstring.h>
#include <qstringlist.h>
#include <qlibrary.h>
#include <qdict.h>
#include "ChartPlugin.h"
#include "DbPlugin.h"
#include "IndicatorPlugin.h"
#include "QuotePlugin.h"
#include "COPlugin.h"
#include "Setting.h"

class Config
{
  public:
    enum Parm
    {
      Home,
      DataPath,
      ChartStyle,
      Compression,
      Grid,
      Bars,
      BackgroundColor,
      BorderColor,
      GridColor,
      ChartPluginPath,
      IndicatorPath,
      Crosshairs,
      DrawMode,
      MainPlotSize,
      IndicatorPlotSize,
      ScaleToScreen,
      IndicatorPluginPath,
      QuotePluginPath,
      GroupPath,
      PortfolioPath,
      Group,
      TestPath,
      PlotFont,
      AppFont,
      NavAreaSize,
      LogScale,
      DbPluginPath,
      CUSRulePath,
      COPluginPath,
      HelpFilePath,
      LastQuotePlugin,
      Height,
      Width,
      X,
      Y,
      ScannerPath,
      CandleColor //unused
    };

    Config ();
    ~Config ();
    void setData(Parm, QString);
    QString getData (Parm);
    QStringList getDirList (QString);
    void setup ();
    QString parseDbPlugin (QString);

    QStringList getIndicators ();
    Setting * getIndicator (QString);
    void deleteIndicator (QString);
    QStringList getIndicatorList ();

    QStringList getPluginList (Config::Parm);
    ChartPlugin * getChartPlugin (QString);
    DbPlugin * getDbPlugin (QString);
    IndicatorPlugin * getIndicatorPlugin (QString);
    QuotePlugin * getQuotePlugin (QString);
    COPlugin * getCOPlugin (QString);
    void closePlugins ();
    void closePlugin (QString);

  protected:
    QDict<QLibrary> libs;
    QDict<ChartPlugin> chartPlugins;
    QDict<DbPlugin> dbPlugins;
    QDict<IndicatorPlugin> indicatorPlugins;
    QDict<QuotePlugin> quotePlugins;
    QDict<COPlugin> coPlugins;
    QString version;
};

#endif
