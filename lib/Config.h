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


#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <QString>

#include "BaseConfig.h"

class Config : public BaseConfig
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
      DefaultBuyColor, // default buy arrow color
      LastDateRange, // last date range setting (int)
      DateRangeButtonStatus, // status of the date range button on/off (int)
      DateRangeStart, // the start date range
      DataPanelSize, // size of the data panel
      ScriptLaunchButton1, // script panel launch button 1
      PlotFont, // chart plot font
      DateRangeEnd, // the end of the date range
      NavAreaSize, // navigator panel size
      UNUSED10, // ******* UNUSED ************
      UNUSED9, // ******* UNUSED ***********
      MainWindowSize, // app window size
      MainWindowPos, // app position
      UNUSED8, // ******** UNUSED ************
      UNUSED7, // ***** UNUSED ************
      IndicatorPluginPath, // path to the indicator plugins
      IndicatorPluginList,  // csv list of current indicator plugins from startup inventory
      Version,  // current qtstalker version
      PlotSizes, // csv list of plot row sizes
      UNUSED6, // ********* UNUSED ***********
      UNUSED14, // ******** UNUSED ***********
      CurrentChart, // csv string (exchange,symbol) currently loaded chart 
      DefaultIndicators, // install flag for default indicators, == 0 if not installed, == 1 if installed
      UNUSED5, // ******* UNUSED ************
      PSButton1, // toolbar bar spacing button 1
      PSButton2, // toolbar bar spacing button 2
      Refresh, // refreshes chart every x minutes
      ScriptLaunchButton2, // script panel launch button 2
      SavedRunningScripts, // saved scripts that were running when app terminated
      ExchangeFileDate,  // the last modification date of /docs/exchanges.csv
      LastScriptPath, // the dir of the last script run from a dialog
      FuturesFileDate,   // the file size of /docs/futures.csv
      DBPluginPath, // path where db plugins are stored
      DBPluginList, // csv list of current db plugins from startup inventory
      UNUSED4, // ******** UNUSED *************
      LastChartPanelExchangeSearch, // last exchange search used on the chart panel
      ScriptLaunchButton3, // script panel launch button 3
      ScriptLaunchButton4, // script panel launch button 4
      ScriptLaunchButton5, // script panel launch button 5
      ScriptLaunchButton6, // script panel launch button 6
      ScriptLaunchButton7, // script panel launch button 7
      ScriptLaunchButton8, // script panel launch button 8
      ScriptLaunchButton9, // script panel launch button 9
      ScriptLaunchButton10, // script panel launch button 10
      ScriptLaunchButton11, // script panel launch button 11
      ScriptLaunchButton12, // script panel launch button 12
      DefaultSymbol, // default symbol flag used in Setup::
      MiscPluginPath, // path to the misc plugins
      MiscPluginList,  // csv list of current misc plugins from startup inventory
      UNUSED3, // ******** UNUSED *************
      UNUSED2, // ********* UNUSED *************
      UNUSED1, // ******** UNUSED *************
      RefreshStatus, // status of the refresh chart toggle button
      LastGroupUsed, // last group displayed in groups area
      UNUSED0, // ************* UNUSED *****************
      IndicatorRow1Position, // tab position of the 1st row
      IndicatorRow2Position, // tab position of the 2nd row
      MainToolBarPos, // last position of the main toolbar
      ChartToolBarPos, // last position of the chart toolbar
      IndicatorRow3Position, // tab position of the 3rd row
      RecentChartsList, // csv list of the contents of the recent charts combobox
      LastChartObjectID, // used to assign a new co object id number
      DefaultTextLabel,
      LastChartPanelSymbolSearch, // the last symbol search used in the chart panel
      DefaultRetracementColor,
      DefaultRetracementLine1,
      DefaultRetracementLine2,
      DefaultRetracementLine3,
      DefaultRetracementLine4,
      DefaultRetracementLine5,
      DefaultRetracementLine6,
      DefaultHLineColor,
      DefaultSellColor,
      DefaultTextColor,
      DefaultTextFont,
      DefaultTLineColor,
      DefaultTLineBar,
      DefaultTLineExtend,
      DefaultTLineUseBar,
      DefaultVLineColor, // default vertical line color
      IndicatorTabRows, // # of indicator tab rows
      LastIndicatorRow1, // last visible tab on row 1
      LastIndicatorRow2, // last visible tab on row 2
      LastIndicatorRow3 // last visible tab on row 3
    };

    Config ();
    void init (QString session); // called only at qtstalker startup, initializes tables
};

#endif

