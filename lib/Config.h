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

// *************************************************************************************************
// All the qtstalker app settings along with any plugin settings are stored using this interface
// to a sqlite db. the enums serve as a way of keeping the keys from being corrupted. plugins use
// a string key to keep things local to the plugin that Config will not know about. just dont use
// any numbers for keys or there will be a conflict between app and plugin settings
// *************************************************************************************************

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <QString>

#include "ConfigDataBase.h"

class Config : public ConfigDataBase
{
  public:
    enum Parm
    {
      Home, // qtstalker base directory
      DataPath, // file path to database
      Pixelspace, // number of pixels between data points
      BarLength, // bar length ie. 5 minute, daily, weekly
      Grid, // chart grid on/off
      QuoteServerName, // the quote server hostname 
      BackgroundColor, // color of chart background
      BorderColor, // color of chart borders
      GridColor, // color of chart grid
      DefaultChartObjectBuyColor, // default buy arrow color
      LastDateRange, // last date range setting (int)
      DateRangeButtonStatus, // status of the date range button on/off (int)
      DateRangeStart, // the start date range
      DataPanelSize, // size of the data panel
      ScriptLaunchButtonRows, // the number of script launch button rows on the script panel
      PlotFont, // chart plot font
      DateRangeEnd, // the end of the date range
      NavAreaSize, // navigator panel size
      QuoteServerPort, // the quote server port number
      IndicatorRow1Height, // the height of row1 of indicators
      MainWindowSize, // app window size
      MainWindowPos, // app position
      IndicatorRow2Height, // the height of row2 of indicators
      IndicatorRow3Height, // the height of row3 of indicators
      IndicatorPluginPath, // path to the indicator plugins
      IndicatorPluginList,  // csv list of current indicator plugins from startup inventory
      Version,  // current qtstalker version
      PlotSizes, // csv list of plot row sizes
      SidePanelTabWidth, // width of the side panel tabs
      ChartLayoutSizes, // splitter sizes for the ChartLayout rows
      CurrentChart, // csv string (exchange,symbol) currently loaded chart
      DefaultIndicators, // install flag for default indicators, == 0 if not installed, == 1 if installed
      SidePanelStatus, //  show / hide status of the side panel
      PSButton1, // toolbar bar spacing button 1
      PSButton2, // toolbar bar spacing button 2
      Refresh, // refreshes chart every x minutes
      ScriptLaunchButtonCols, // the number of script launch button cols on the script panel
      CrossHairs, // crosshairs status
      ExchangeFileDate,  // the last modification date of /docs/exchanges.csv
      LastScriptPath, // the dir of the last script run from a dialog
      FuturesFileDate,   // the file size of /docs/futures.csv
      CrossHairsColor, // crosshairs color
      SidePanelLastPage, // the last side panel page displayed
      LastScriptSearch, // the last used script search string
      LastChartPanelExchangeSearch, // last exchange search used on the chart panel
      ScriptLaunchButton3, // ********* UNUSED ***************
      ScriptLaunchButton4, // ********* UNUSED ***************
      ScriptLaunchButton5, // ********* UNUSED ***************
      ScriptLaunchButton6, // ********* UNUSED ***************
      ScriptLaunchButton7, // ********* UNUSED ***************
      ScriptLaunchButton8, // ********* UNUSED ***************
      ScriptLaunchButton9, // ********* UNUSED ***************
      ScriptLaunchButton10, // ********* UNUSED ***************
      ScriptLaunchButton11, // ********* UNUSED ***************
      ScriptLaunchButton12, // ********* UNUSED ***************
      DefaultSymbol, // default symbol flag used in Setup::
      MiscPluginPath, // path to the misc plugins
      MiscPluginList,  // csv list of current misc plugins from startup inventory
      LastNewIndicator, // the last selected indicator from the new indicator dialog
      UNUSED6, // ********* UNUSED *************
      UNUSED7, // ******** UNUSED *************
      RefreshStatus, // status of the refresh chart toggle button
      LastGroupUsed, // last group displayed in groups area
      UNUSED8, // ************* UNUSED *****************
      IndicatorRow1Position, // tab position of the 1st row
      IndicatorRow2Position, // tab position of the 2nd row
      MainToolBarPos, // last position of the main toolbar
      ChartToolBarPos, // last position of the chart toolbar
      IndicatorRow3Position, // tab position of the 3rd row
      RecentChartsList, // csv list of the contents of the recent charts combobox
      LastChartObjectID, // used to assign a new co object id number
      DefaultChartObjectTextLabel,
      LastChartPanelSymbolSearch, // the last symbol search used in the chart panel
      DefaultChartObjectRetracementColor,
      DefaultChartObjectRetracementLine1,
      DefaultChartObjectRetracementLine2,
      DefaultChartObjectRetracementLine3,
      DefaultChartObjectRetracementLine4,
      DefaultChartObjectRetracementLine5,
      DefaultChartObjectRetracementLine6,
      DefaultChartObjectHLineColor,
      DefaultChartObjectSellColor,
      DefaultChartObjectTextColor,
      DefaultChartObjectTextFont,
      DefaultChartObjectTLineColor,
      DefaultChartObjectTLineBar,
      DefaultChartObjectTLineExtend,
      DefaultChartObjectTLineUseBar,
      DefaultChartObjectVLineColor, // default vertical line color
      IndicatorTabRows, // # of indicator tab rows
      LastIndicatorRow1, // last visible tab on row 1
      LastIndicatorRow2, // last visible tab on row 2
      LastIndicatorRow3 // last visible tab on row 3
    };

    Config ();
    void init (); // called only at qtstalker startup, initializes tables
};

#endif

