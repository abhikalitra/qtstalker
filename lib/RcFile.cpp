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

#include <qstringlist.h>
#include <qsettings.h>

#include "RcFile.h"

// This class do ALL but NOTHING ELSE with the qtstalkerrc file
// located in /home/yourhome/.qt/ or similar, depending on your system
// 
// It is intended to replace the according functions 
// locatet in the Config class. Because Config do more than
// only read/write stettings to and from the rc-file.
// But Config is often used/included to do only this.
// loh.tar
    
    
RcFile::RcFile ()
{ 
  Key[Home] = "Home";
  Def[Home] = "";
  Key[DataPath] = "DataPath";
  Def[DataPath] = "";
  Key[Pixelspace] = "Pixelspace";
  Def[Pixelspace] = "6";
  Key[BarLength] = "BarLength";
  Def[BarLength] = ""; // ? didn't find in my rcfile
  Key[Grid] = "Grid";
  Def[Grid] = "TRUE";
  Key[Bars] = "Bars"; // better to rename in BarsToLoad or BarsCount
  Def[Bars] = "275";
  Key[BackgroundColor] = "BackgroundColor";
  Def[BackgroundColor] = "#000000"; // black
  Key[BorderColor] = "BorderColor";
  Def[BorderColor] = "#ffffff"; // white
  Key[GridColor] = "GridColor";
  Def[GridColor] = "#555555"; // grey 
  Key[PaperTradeMode] = "PaperTradeMode";
  Def[PaperTradeMode] = "FALSE";
  Key[IndicatorPath] = "IndicatorPath";
  Def[IndicatorPath] = "";
  Key[Crosshairs] = "Crosshairs";
  Def[Crosshairs] = "FALSE";
  Key[DrawMode] = "DrawMode";
  Def[DrawMode] = "TRUE";
  Key[DataPanelSize] = "DataPanelSize";
  Def[DataPanelSize] = "338,322";
  Key[ScaleToScreen] = "ScaleToScreen";
  Def[ScaleToScreen] = "TRUE";
  Key[QuotePluginPath] = "QuotePluginPath";
  Def[QuotePluginPath] = "";
  Key[GroupPath] = "GroupPath";
  Def[GroupPath] = "";
  Key[PortfolioPath] = "PortfolioPath";
  Def[PortfolioPath] = "";
  Key[Group] = "Group";
  Def[Group] = ""; // ? didn't find in my rcfile
  Key[TestPath] = "TestPath";
  Def[TestPath] = "";
  Key[PlotFont] = "PlotFont";
  Def[PlotFont] = "DejaVu Sans,9,50,0,0";
  Key[AppFont] = "AppFont";
  Def[AppFont] = "DejaVu Sans,9,50,0,0";
  Key[NavAreaSize] = "NavAreaSize";
  Def[NavAreaSize] = "780,109";
  Key[LogScale] = "LogScalex";
  Def[LogScale] = "FALSE";
  Key[PS1Button] = "PS1Button";
  Def[PS1Button] = "2";
  Key[PS2Button] = "PS2Button";
  Def[PS2Button] = "4";
  Key[PS3Button] = "PS3Button";
  Def[PS3Button] = "6";
  Key[IndexPath] = "IndexPath";
  Def[IndexPath] = "";
  Key[HelpFilePath] = "HelpFilePath";
  Def[HelpFilePath] = "";
  Key[LastQuotePlugin] = "LastQuotePlugin";
  Def[LastQuotePlugin] = "Yahoo";
  Key[MainWindowSize] = "MainWindowSize";
  Def[MainWindowSize] = "566,895";
  Key[MainWindowPos] = "MainWindowPos";
  Def[MainWindowPos] = "90,90";
  Key[PrefDlgWindowSize] = "PrefDlgWindowSize";
  Def[PrefDlgWindowSize] = "400,270";
  Key[ScannerPath] = "ScannerPath";
  Def[ScannerPath] = "";
  Key[Version] = "Version";
  Def[Version] = "0.34";
  Key[PlotSizes] = "PlotSizes";
  Def[PlotSizes] = "319,94,88";
  Key[Menubar] = "Preferences/Menubar"; // better to rename in ShowMenuBar
  Def[Menubar] = "TRUE";
  Key[COPath] = "COPath";
  Def[COPath] = "";
  Key[LocalIndicatorsPath] = "LocalIndicatorsPath";
  Def[LocalIndicatorsPath] = "";
  Key[FundamentalsPath] = "FundamentalsPath";
  Def[FundamentalsPath] = "";
  Key[CurrentChart] = "CurrentChart";
  Def[CurrentChart] = "";
  Key[IndicatorGroup] = "IndicatorGroup";
  Def[IndicatorGroup] = "Indicators";
  Key[QuotePluginStorage] = "QuotePluginStorage";
  Def[QuotePluginStorage] = "";
  Key[LastNewIndicator] = "LastNewIndicator";
  Def[LastNewIndicator] = "";
  Key[PSButtonCount] = "Preferences/PSButtonCount";
  Def[PSButtonCount] = "3";
  Key[PSButton] = "Preferences/PSButton";
  Def[PSButton] = "6";
  Key[ShowSidePanel] = "ShowSidePanel";
  Def[ShowSidePanel] = "TRUE";
  Key[ShowQuitBtn] = "Preferences/ShowQuitBtn";
  Def[ShowQuitBtn] = "TRUE";
  Key[ShowPrefBtn] = "Preferences/ShowPrefBtn";
  Def[ShowPrefBtn] = "TRUE";
  Key[ShowSidePanelBtn] = "Preferences/ShowSidePanelBtn";
  Def[ShowSidePanelBtn] = "TRUE";
  Key[ShowGridBtn] = "Preferences/ShowGridBtn";
  Def[ShowGridBtn] = "TRUE";
  Key[ShowScaleToScreenBtn] = "Preferences/ShowScaleToScreenBtn";
  Def[ShowScaleToScreenBtn] = "TRUE";
  Key[ShowCrosshairBtn] = "Preferences/ShowCrosshairBtn";
  Def[ShowCrosshairBtn] = "TRUE";
  Key[ShowPaperTradeBtn] = "Preferences/ShowPaperTradeBtn";
  Def[ShowPaperTradeBtn] = "TRUE";
  Key[ShowDrawModeBtn] = "Preferences/ShowDrawModeBtn";
  Def[ShowDrawModeBtn] = "TRUE";
  Key[ShowNewIndicatorBtn] = "Preferences/ShowNewIndicatorBtn";
  Def[ShowNewIndicatorBtn] = "TRUE";
  Key[ShowDataWindowBtn] = "Preferences/ShowDataWindowBtn";
  Def[ShowDataWindowBtn] = "TRUE";
  Key[ShowMainQuoteBtn] = "Preferences/ShowMainQuoteBtn";
  Def[ShowMainQuoteBtn] = "TRUE";
  Key[ShowHelpButton] = "Preferences/ShowHelpButton";
  Def[ShowHelpButton] = "TRUE";
  Key[ShowSlider] = "Preferences/ShowSlider";
  Def[ShowSlider] = "TRUE";
  Key[ShowBarsToLoadField] = "Preferences/ShowBarsToLoadField";
  Def[ShowBarsToLoadField] = "TRUE";
  Key[ShowBarSpSpinbox] = "Preferences/ShowBarSpSpinbox";
  Def[ShowBarSpSpinbox] = "TRUE";
  Key[ShowCmps15Btn] = "Preferences/ShowCmps15Btn";
  Def[ShowCmps15Btn] = "TRUE";
  Key[ShowCmpsDayBtn] = "Preferences/ShowCmpsDayBtn";
  Def[ShowCmpsDayBtn] = "TRUE";
  Key[ShowCmpsWkyBtn] = "Preferences/ShowCmpsWkyBtn";
  Def[ShowCmpsWkyBtn] = "TRUE";
  Key[ShowCmpsComboBox] = "Preferences/ShowCmpsComboBox";
  Def[ShowCmpsComboBox] = "TRUE";
  Key[PrefColorCount] = "Preferences/PrefColorCount";
  Def[PrefColorCount] = "5";
  Key[PrefColor] = "Preferences/PrefColor";
  Def[PrefColor] = "#ffffff"; // white
  Key[LastAktiveNav] = "LastAktiveNav";
  Def[LastAktiveNav] = "";
  Key[LastGroupUsed] = "LastGroupUsed";
  Def[LastGroupUsed] = "";
  Key[LastDateTimeCOAdded] = "LastDateTimeCOAdded";
  Def[LastDateTimeCOAdded] = "";
  Key[IndiGroupSplitterSize] = "IndiGroupSplitterSize";
  Def[IndiGroupSplitterSize] = "";
  Key[MainToolBarPos] = "MainToolBarPos";
  Def[MainToolBarPos] = "";
  Key[ChartToolBarPos] = "ChartToolBarPos";
  Def[ChartToolBarPos] = "";
  
}

RcFile::~RcFile ()
{
}

//
//  public functions, no "string Parm" allowed
//  to prevent mismatch in the rc-file
//

void RcFile::loadData (Parm name, QString &value, const QString &s)
{
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  k.append(s);
  
  value = Def[name];
  
  QSettings settings;
  value = settings.readEntry(k, value);
}

void RcFile::loadData (Parm name, QString &value, const int n)
{
  QString s;
  if (n > -1) s.append(QString::number(n));
  loadData(name, value, s);
}

void RcFile::loadData (Parm name, int &value, const int n)
{ 
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  if (n > -1) k.append(QString::number(n));
  
  QString v = Def[name];
  value = v.toInt();
  
  QSettings settings;
  value = settings.readNumEntry(k, value);
}

void RcFile::loadData (Parm name, bool &value)
{ 
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);

  if (Def[name] == "TRUE") value = TRUE;
  else value = FALSE;
  
  QSettings settings;
  value = settings.readBoolEntry(k, value);
}

void RcFile::saveData (Parm name, QString &value, const QString &s)
{ 
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  k.append(s);
  
  QSettings settings;
  settings.writeEntry(k, value);
}

void RcFile::saveData (Parm name, QString &value, const int n)
{ 
  QString s;
  if (n > -1) s.append(QString::number(n));
  saveData(name, value, s);
}

void RcFile::saveData (Parm name, int value, const int n)
{
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  if (n > -1) k.append(QString::number(n));
  QString v = QString::number(value);
  
  QSettings settings;
  settings.writeEntry(k, v);
}

void RcFile::saveData (Parm name, const bool value)
{  
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  
  QSettings settings;
  settings.writeEntry(k, value);
}

//
// for special purposes   
//
void RcFile::loadColor (Parm name, QColor &color)
{
  QString c;
  loadData(name, c);
  color.setNamedColor(c);
}
    
void RcFile::loadColor (Parm name, QColor &color, const int n)
{
  QString c;
  loadData(name, c, n);
  color.setNamedColor(c);
}

void RcFile::saveColor (Parm name, QColor &color)
{
  QString c = color.name();
  saveData(name, c);
}
  
void RcFile::saveColor (Parm name, QColor &color, const int n)
{
  QString c = color.name();
  saveData(name, c, n);
}

void RcFile::loadFont (Parm name, QFont &font)
{
  QString f;

  loadData(name, f);

  QStringList l = QStringList::split(",", f, FALSE);

  font.setFamily(l[0]);
  font.setPointSize(l[1].toInt());
  font.setWeight(l[2].toInt());
  font.setItalic(l[3].toInt());
  font.setBold(l[4].toInt());
}

void RcFile::saveFont (Parm name, QFont &font)
{
  QString f =font.family() + "," +
              QString::number(font.pointSize()) + "," +
              QString::number(font.weight()) + "," +
              QString::number(font.italic ()) + "," +
              QString::number(font.bold ());
		
  saveData(name, f);
}

void RcFile::loadSplitterSize (Parm name, QSplitter *sp,  const QString &n)
{
  QString s;
  loadData(name, s, n);
  
  QStringList stringList = QStringList::split(",", s, FALSE);
  QValueList<int> sizeList = sp->sizes();
  
  int loop;
  for (loop = 0; loop < (int) stringList.count(); loop++)
    sizeList[loop] = stringList[loop].toInt();
  
  sp->setSizes(sizeList);
}

void RcFile::saveSplitterSize (Parm name, QSplitter *sp, const QString &n)
{
  QStringList stringList;
  QValueList<int> sizeList = sp->sizes();
  
  int loop;
  for (loop = 0; loop < (int) sizeList.count(); loop++)
    stringList.append(QString::number(sizeList[loop]));
  
  QString s(stringList.join(","));
  saveData(name, s, n);
}

void RcFile::loadPoint (Parm name, QPoint &p, const int n)
{
  QString s;
  loadData(name, s, n);
  
  QStringList l = QStringList::split(",", s, FALSE);
  p.setX(l[0].toInt());
  p.setY(l[1].toInt());
}

void RcFile::savePoint (Parm name, const QPoint &p, const int n)
{
  QString s = QString::number(p.x()) + "," + QString::number(p.y());
  
  saveData(name, s, n);
}

void RcFile::loadSize (Parm name, QSize &sz, const int n)
{
  QString s;
  loadData(name, s, n);
  
  QStringList l = QStringList::split(",", s, FALSE);
  sz.setWidth (l[0].toInt());
  sz.setHeight(l[1].toInt());
}

void RcFile::saveSize (Parm name, const QSize &sz, const int n)
{
  QString s = QString::number(sz.width()) + "," + QString::number(sz.height());
  
  saveData(name, s, n);
}
