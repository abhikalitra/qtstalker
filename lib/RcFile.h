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


#ifndef RCFILE_HPP
#define RCFILE_HPP

#include <qstring.h>
#include <qcolor.h>
#include <qfont.h>
#include <qsplitter.h>
#include <qpoint.h>
#include <qsize.h>

//
// This class do ALL but NOTHING ELSE with the qtstalkerrc file
// locatet in /home/yourhome/.qt/ or similar, depending on your system
// 
// It is intended to replace the according functions 
// located in the Config class. Because Config do more than
// only read/write stettings to and from the rc-file.
// But Config is often used/included to do only this.
// loh.tar
    
class RcFile
{
  public:
    
     RcFile ();
    ~RcFile ();
 
    // here are all available parameters, 
    // in case of some modifications
    // don't forget to edit also RcFile.cpp
    enum Parm
    {
      Home,
      DataPath,
      Pixelspace,
      BarLength,
      Grid,
      BarsToLoad,
      BackgroundColor,
      BorderColor,
      GridColor,
      PaperTradeMode,
      IndicatorPath,
      Crosshairs,
      DrawMode,
      DataPanelSize,
      ScaleToScreen,
      QuotePluginPath,
      GroupPath,
      PortfolioPath,
      TestPath,
      PlotFont,
      AppFont,
      NavAreaSize,
      LogScale,
 /*     PS1Button, // deprecated
      PS2Button, //  "
      PS3Button, //  "
   */   IndexPath,
      HelpFilePath,
      UserDocsPath,
      LastQuotePlugin,
      MainWindowSize,
      MainWindowPos,
      PrefDlgWindowSize,
      EditStockWindowSize,
      HelpWindowSize,
      BackTesterWindowSize,
      ScannerWindowSize,
      ScannerPath,
      Version,
      PlotSizes,
      ShowMenuBar,
      COPath,
      LocalIndicatorsPath,
      FundamentalsPath,
      CurrentChart,
      IndicatorGroup,
      QuotePluginStorage,
      LastNewIndicator,
      PSButtonCount,  // not yet used
      PSButton,
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
      ShowMainQuoteBtn,
      ShowHelpButton,
      ShowSlider,
      ShowBarsToLoadField,
      ShowBarSpSpinbox,
      ShowCmps15Btn,
      ShowCmpsDayBtn,
      ShowCmpsWkyBtn,
      ShowCmpsComboBox,
      PrefColorCount,
      PrefColor,
      LastAktiveNav,
      LastGroupUsed,
      LastChartDir,
      LastDateTimeCOAdded,
      IndiGroupSplitterSize,
      MainToolBarPos,
      ChartToolBarPos,
      NavigatorPosition,
      NavigatorActiveButton,
      MAX_PARM  // keep this at the end
    };
  
  private:
    char *Key[MAX_PARM];
    char *Def[MAX_PARM]; // default values
    
  public:
   
    // public functions, no string as "Parm" allowed
    // to prevent mismatch in the rc-file
    
    // for any kind of data
    void loadData (Parm, QString &, const QString & = 0);
    void loadData (Parm, QString &, const int);
    void loadData (Parm, int &, const int = -1);
    void loadData (Parm, bool &);
    
    void saveData (Parm, QString &, const QString & = 0);
    void saveData (Parm, QString &, const int);
    void saveData (Parm, int, const int = -1);
    void saveData (Parm, const bool);
    
    // for special purposes
    void loadColor (Parm, QColor &);
    void loadColor (Parm, QColor &, const int);
    
    void saveColor (Parm, QColor &);
    void saveColor (Parm, QColor &, const int);
    
    void loadFont (Parm, QFont &);
    void saveFont (Parm, QFont &);
    
    void loadSplitterSize (Parm, QSplitter *, const QString & = 0);
    void saveSplitterSize (Parm, QSplitter *, const QString & = 0);
    
    void loadPoint (Parm, QPoint &, const int = -1);
    void savePoint (Parm, const QPoint &, const int = -1);
    
    void loadSize (Parm, QSize &, const int = -1);
    void saveSize (Parm, const QSize &, const int = -1);
    
  private:
    // DON'T WORK! runs in strange trouble
    // in case of interlaced use
    // QSettings settings;
};

#endif

