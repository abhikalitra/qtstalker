/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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
      UpColor,
      DownColor,
      NeutralColor,
      PaintBarIndicator,
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
      LogScale
    };

    Config (QString);
    ~Config ();
    void setData(Parm, QString);
    QString getData (Parm);
    QStringList getDirList (QString);

    QStringList getIndicators ();
    QStringList getIndicator (QString);
    QString getIndicatorPlugin (QString);
    void setIndicator (QString, QStringList);
    void deleteIndicator (QString);
    QStringList getIndicatorList ();

    QStringList getIndicatorPlugins ();
    QStringList getQuotePlugins ();

  protected:
    QString path;
};

#endif
