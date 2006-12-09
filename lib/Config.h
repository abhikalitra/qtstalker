/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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
#include "IndicatorPlugin.h"
#include "QuotePlugin.h"
#include "Setting.h"

class Config
{
  public:
    enum Parm
    {
      Home,
      DataPath,
      Pixelspace,
      BarLength,
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
      PS1Button,
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
      PS2Button,
      PS3Button,
      Unused4, //unused
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
      Macro1, //unused
      Macro2, //unused
      Macro3, //unused
      Macro4, //unused
      Macro5, //unused
      Macro6, //unused
      Macro7, //unused
      Macro8, //unused
      Macro9, //unused
      Macro10, //unused
      Macro11, //unused
      Macro12, //unused
      IndicatorGroup,
      QuotePluginStorage,
      ShowUpgradeMessage,
      LastNewIndicator
    };

    Config ();
    ~Config ();
    void setData (Parm, QString &);
    void setData (QString &, QString &);
    void getData (Parm, QString &);
    void getData (QString &, QString &);
    void getDirList (QString &, bool, QStringList &);
    void setup ();

    void getIndicators (QString &, QStringList &);
    void getIndicator (QString &, Setting &);
    void deleteIndicator (QString &);
    void getIndicatorList (QStringList &);
    void setIndicator (QString &, Setting &);

    void getPluginList (Config::Parm, QStringList &);
    IndicatorPlugin * getIndicatorPlugin (QString &);
    QuotePlugin * getQuotePlugin (QString &);
    void closePlugins ();
    void closePlugin (QString &);

    void copyIndicatorFile (QString &, QString &);
    void checkUpgrade ();
    void setDefaultIndicators ();
    void createDefaultIndicator (Setting &set, QString &plugin, QString &name, int tabRow);

  protected:
    QDict<QLibrary> libs;
    QDict<IndicatorPlugin> indicatorPlugins;
    QDict<QuotePlugin> quotePlugins;
    QString version;
};

#endif

// remove
