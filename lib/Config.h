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
      BarLength, 
      Grid, // chart grid on/off
      BarsToLoad, // number of bars to load
      BackgroundColor, // color of chart background
      BorderColor, // color of chart borders
      GridColor, // color of chart grid
      DefaultBuyArrowColor, // default buy arrow color
      QuotePath, // path to quote db
      Crosshairs, // char crosshairs on/off
      DrawMode, // chart draw mode on/off
      DataPanelSize, // size of the data panel
      ScaleToScreen, // scale to screen on/off
      PlotFont, // chart plot font
      AppFont, // app font
      NavAreaSize, // navigator panel size
      UserDocsPath, // path to user docs
      DocsPath, // path to docs
      MainWindowSize, // app window size
      MainWindowPos, // app position
      PrefDlgWindowSize, // preferences dialog window size
      HelpWindowSize, // 
      BackTesterWindowSize,
      ScannerWindowSize,
      Version,
      PlotSizes,
      ShowMenuBar,
      ShowExtraToolbar,
      CurrentChart,
      LastNewIndicator,
      PSButtonCount,
      PSButton1,
      PSButton2,
      PSButton3,
      ShowSidePanel,
      ShowQuitBtn,
      ShowPrefBtn,
      ShowSidePanelBtn,
      ShowGridBtn,
      ShowScaleToScreenBtn,
      ShowCrosshairBtn,
      ShowPaperTradeBtn,
      ShowDrawModeBtn,
      ShowNewIndicatorBtn,
      ShowDataWindowBtn,
      ShowHelpButton,
      ShowSlider,
      ShowBarsToLoadField,
      ShowBarSpSpinbox,
      ShowCmps5Btn,
      ShowCmps15Btn,
      ShowCmps60Btn,
      ShowCmpsDayBtn,
      ShowCmpsWkyBtn,
      ShowCmpsMtyBtn,
      ShowCmpsComboBox,
      PrefColorCount,
      PrefColor,
      ShowRecentCharts,
      LastAktiveNav,
      LastGroupUsed,
      LastIndicatorUsed,
      LastDateTimeCOAdded,
      IndiGroupSplitterSize,
      MainToolBarPos,
      ChartToolBarPos,
      NavigatorActiveButton,
      RecentChartsList,
      LastChartObjectID,
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
      DefaultVerticalLineColor
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

