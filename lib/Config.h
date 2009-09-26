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
      DefaultIndicators, // install flag for default indicators, == 0 if not installed, == 1 if installed
      DbAllSymbols, // the sql command to get all symbols in db
      PSButton1, // toolbar bar spacing button 1
      PSButton2, // toolbar bar spacing button 2
      Unused8, // ******** UNUSED **************
      ShowSidePanel, // side panel flag
      ShowQuitBtn, // toolbar button flag
      ShowPrefBtn,  // toolbar button flag
      ShowSidePanelBtn, // toolbar button flag
      ShowGridBtn, // toolbar button flag
      ShowScaleToScreenBtn, // toolbar button flag
      ShowCrosshairBtn, // toolbar button flag
      DbSearchSymbols, // the sql command to get a symbol search
      ShowDrawModeBtn, // toolbar button flag
      ShowNewIndicatorBtn, // toolbar button flag
      ShowDataWindowBtn, // toolbar button flag
      ShowHelpButton, // toolbar button flag
      Unused7, // ********** UNUSED *************
      Unused9,  // ********** UNUSED *************
      Unused10, // ****** UNUSED **********
      ShowCmps5Btn, // toolbar button flag
      ShowCmps15Btn, // toolbar button flag
      ShowCmps60Btn, // toolbar button flag
      ShowCmpsDayBtn, // toolbar button flag
      ShowCmpsWkyBtn, // toolbar button flag
      ShowCmpsMtyBtn, // toolbar button flag
      Unused11, // ********* UNUSED *************
      DbFirstDate, // the sql command to get the first date for a symbol
      DbLastDate, // the sql command to get the last date for a symbol
      DbGetSymbol, // the sql command to get symbol quotes
      UNUSED1, // ********* UNUSED *******************
      LastGroupUsed, // last group displayed in groups area
      LastIndicatorUsed, // csv list of the last indicators used
      UNUSED2, // ************ UNUSED ************************
      UNUSED3, // ************* UNUSED *********************
      MainToolBarPos, // last position of the main toolbar
      ChartToolBarPos, // last position of the chart toolbar
      UNUSED4, // ************** UNUSED ***************************
      RecentChartsList, // csv list of the contents of the recent charts combobox
      LastChartObjectID, // used to assign a new co object id number
      DefaultTextLabel,
      LastSymbolSearch,
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
      UNUSED5, // ************ UNUSED ****************************
      UNUSED6, // ************ UNUSED ***********************
      IndicatorScriptDefault
    };

    Config ();
    void beginTransaction ();
    void commit ();
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

