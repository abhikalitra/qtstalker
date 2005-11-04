/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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
      ChartStyle, //unused
      Compression,
      Grid,
      Bars,
      BackgroundColor,
      BorderColor,
      GridColor,
      PaperTradeMode,
      IndicatorPath,
      Crosshairs,
      DrawMode,
      DataPanelSize,
      MacroPath,
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
      Version,
      PlotSizes,
      Menubar,
      Macro1,
      Macro2,
      Macro3,
      Macro4,
      Macro5,
      Macro6,
      Macro7,
      Macro8,
      Macro9,
      Macro10,
      Macro11,
      Macro12,
      IndicatorGroup,
      QuotePluginStorage,
      ShowUpgradeMessage,
      LastNewIndicator
    };

    Config ();
    ~Config ();
    void setData (Parm, QString &);
    void setData (QString &, QString &);
    QString getData (Parm);
    QString getData (QString &);
    void getDirList (QString &, bool, QStringList &);
    void setup ();
    QString parseDbPlugin (QString &);

    void getIndicators (QString &, QStringList &);
    void getIndicator (QString &, Setting &);
    void deleteIndicator (QString &);
    void getIndicatorList (QStringList &);
    void setIndicator (QString &, Setting &);

    void getPluginList (Config::Parm, QStringList &);
    DbPlugin * getDbPlugin (QString &);
    IndicatorPlugin * getIndicatorPlugin (QString &);
    QuotePlugin * getQuotePlugin (QString &);
    COPlugin * getCOPlugin (QString &);
    void closePlugins ();
    void closePlugin (QString &);

    void copyIndicatorFile (QString &, QString &);
    void checkUpgrade ();

  protected:
    QDict<QLibrary> libs;
    QDict<DbPlugin> dbPlugins;
    QDict<IndicatorPlugin> indicatorPlugins;
    QDict<QuotePlugin> quotePlugins;
    QDict<COPlugin> coPlugins;
    QString version;
};

#endif
