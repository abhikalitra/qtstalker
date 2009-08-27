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


#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <QString>
#include <QStringList>
#include <QSplitter>
#include <QColor>
#include <QFont>
#include <QPoint>
#include <QSize>

class Config
{
  public:
    enum Parm
    {
      Home, // qtstalker base directory
      DataPath, // file path to database
      Pixelspace, // number of pixels between data points
      BarLength, // bar length ie. 5 minute, daily, weekly
      Grid, // chart grid on/off
      BarsToLoad, // number of bars to load
      BackgroundColor, // color of chart background
      BorderColor, // color of chart borders
      GridColor, // color of chart grid
      DefaultBuyArrowColor, // default buy arrow color
      DbName, // sql db name
      Crosshairs, // char crosshairs on/off
      DrawMode, // chart draw mode on/off
      DataPanelSize, // size of the data panel
      ScaleToScreen, // scale to screen on/off
      PlotFont, // chart plot font
      AppFont, // app font
      NavAreaSize, // navigator panel size
      DbPlugin, // the qt name for the sql db plugin used for quotes
      DbHostName, // the hostname used for sql quotes
      MainWindowSize, // app window size
      MainWindowPos, // app position
      PrefDlgWindowSize, // preferences dialog window size
      DbUserName, // sql quotes db user name needed for login
      BackTesterWindowSize, // last backtester window size
      ScannerWindowSize,  // last scanner window size
      Version,  // current qtstalker version
      PlotSizes, // csv list of plot row sizes
      DbPassword, // sql db user password
      ShowRecentCharts, // csv list of last used charts
      CurrentChart, // currently loaded chart
      DbDateFormat, // date format ie yyyy/MM/dd HH:mm:ss
      DbDateColumn, // the sql date column text
      PSButton1, // toolbar bar spacing button 1
      PSButton2, // toolbar bar spacing button 2
      PSButton3, // toolbar bar spacing button 3
      ShowSidePanel, // side panel flag
      ShowQuitBtn, // toolbar button flag
      ShowPrefBtn,  // toolbar button flag
      ShowSidePanelBtn, // toolbar button flag
      ShowGridBtn, // toolbar button flag
      ShowScaleToScreenBtn, // toolbar button flag
      ShowCrosshairBtn, // toolbar button flag
      DbOpenColumn, // the sql open column text
      ShowDrawModeBtn, // toolbar button flag
      ShowNewIndicatorBtn, // toolbar button flag
      ShowDataWindowBtn, // toolbar button flag
      ShowHelpButton, // toolbar button flag
      ShowSlider, // toolbar button flag
      ShowBarsToLoadField,  // toolbar button flag
      ShowBarSpSpinbox, // toolbar button flag
      ShowCmps5Btn, // toolbar button flag
      ShowCmps15Btn, // toolbar button flag
      ShowCmps60Btn, // toolbar button flag
      ShowCmpsDayBtn, // toolbar button flag
      ShowCmpsWkyBtn, // toolbar button flag
      ShowCmpsMtyBtn, // toolbar button flag
      ShowCmpsComboBox, // toolbar button flag
      DbHighColumn, // the sql high column text
      DbLowColumn, // the sql low column text
      DbCloseColumn, // the sql close column text
      DbVolumeColumn, // the sql volume column text
      LastGroupUsed, // last group displayed in groups area
      LastIndicatorUsed, // csv list of the last indicators used
      DbOIColumn, // the sql oi column text
      DbIndexTable, // table name for quote symbol index
      MainToolBarPos, // last position of the main toolbar
      ChartToolBarPos, // last position of the chart toolbar
      DbSymbolColumn, // the name of the symbol index symbol column
      RecentChartsList, // csv list of the contents of the recent charts combobox
      LastChartObjectID, // used to assign a new co object id number
      DefaultCycleColor,
      DefaultCycleInterval,
      DefaultFiboLineColor,
      DefaultFiboLine1,
      DefaultFiboLine2,
      DefaultFiboLine3,
      DefaultFiboLine4,
      DefaultFiboLine5,
      DefaultFiboLine6,
      DefaultHorizontalLineColor,
      DefaultSellArrowColor,
      DefaultTextColor,
      DefaultTextFont,
      DefaultTrendLineColor,
      DefaultTrendLineBar,
      DefaultTrendLineExtend,
      DefaultTrendLineUseBar,
      DefaultVerticalLineColor, // default vertical line color
      IndicatorTabRows, // # of indicator tab rows
      DbNameColumn, // the name of the symbol index name column
      DbExchangeColumn // the name of the symbol index exchange column
    };

    Config ();
    ~Config ();
    void setData (Parm, QString &);
    void getData (Parm, QString &);
    void getData (Parm, QSplitter *);
    void setData (Parm, QSplitter *);
    void getData (Parm, bool &);
    void setData (Parm, bool);
    void getData (Parm, int &);
    void setData (Parm, int);
    void getData (Parm, double &);
    void setData (Parm, double);
    void getData (Parm, QColor &);
    void setData (Parm, QColor &);
    void getData (Parm, QFont &);
    void setData (Parm, QFont &);
    void getData (Parm, QPoint &);
    void setData (Parm, QPoint &);
    void getData (Parm, QSize &);
    void setData (Parm, QSize &);

  protected:
};

#endif

