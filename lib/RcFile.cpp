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
  Key[DataPath] = "DataPath";
  Key[Pixelspace] = "Pixelspace";
  Key[BarLength] = "BarLength";
  Key[Grid] = "Grid";
  Key[Bars] = "Bars";
  Key[BackgroundColor] = "BackgroundColor";
  Key[BorderColor] = "BorderColor";
  Key[GridColor] = "GridColor";
  Key[PaperTradeMode] = "PaperTradeMode";
  Key[IndicatorPath] = "IndicatorPath";
  Key[Crosshairs] = "Crosshairs";
  Key[DrawMode] = "DrawMode";
  Key[DataPanelSize] = "DataPanelSize";
  Key[ScaleToScreen] = "ScaleToScreen";
  Key[QuotePluginPath] = "QuotePluginPath";
  Key[GroupPath] = "GroupPath";
  Key[PortfolioPath] = "PortfolioPath";
  Key[Group] = "Group";
  Key[TestPath] = "TestPath";
  Key[PlotFont] = "PlotFont";
  Key[AppFont] = "AppFont";
  Key[NavAreaSize] = "NavAreaSize";
  Key[LogScale] = "LogScalex";
  Key[PS1Button] = "PS1Button";
  Key[PS2Button] = "PS2Button";
  Key[PS3Button] = "PS3Button";
  Key[IndexPath] = "IndexPathv";
  Key[HelpFilePath] = "HelpFilePath";
  Key[LastQuotePlugin] = "LastQuotePlugin";
  Key[Height] = "Height";
  Key[Width] = "Width";
  Key[X] = "X";
  Key[Y] = "Y";
  Key[ScannerPath] = "ScannerPath";
  Key[Version] = "Version";
  Key[PlotSizes] = "PlotSizes";
  Key[Menubar] = "Menubar";
  Key[COPath] = "COPath";
  Key[LocalIndicatorsPath] = "LocalIndicatorsPath";
  Key[FundamentalsPath] = "FundamentalsPath";
  Key[CurrentChart] = "CurrentChart";
  Key[IndicatorGroup] = "IndicatorGroup";
  Key[QuotePluginStorage] = "QuotePluginStorage";
  Key[LastNewIndicator] = "LastNewIndicator";
  Key[PSButtonCount] = "Preferences/PSButtonCount";
  Key[PSButton] = "Preferences/PSButton";
/* TODO
  Key[PrefColorCount] = "Preferences/PrefColorCount";
  Key[PrefColor] = "Preferences/PrefColor";
*/ 
}

RcFile::~RcFile ()
{
}

//
//  public functions, no "string Parm" allowed
//  to prevent mismatch in the rc-file
//
void RcFile::loadData (Parm name, QString &value)
{
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  value = settings.readEntry(k, value);
}

void RcFile::loadData (Parm name, QString &value, const QString &n)
{
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  k.append(n);
  value = settings.readEntry(k, value);
}

void RcFile::loadData (Parm name, QString &value, const int n)
{
  QString s;
  s.append(QString::number(n));
  loadData(name, value, s);
}

void RcFile::loadData (Parm name, int &value)
{ 
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  value = settings.readNumEntry(k, value);
}

void RcFile::loadData (Parm name, int &value, const int n)
{ 
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  k.append(QString::number(n));
  value = settings.readNumEntry(k, value);
}

void RcFile::loadData (Parm name, bool &value)
{ 
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);

  value = settings.readBoolEntry(k, value);
}

void RcFile::saveData (Parm name, QString &value)
{ 
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  settings.writeEntry(k, value);
}

void RcFile::saveData (Parm name, QString &value, const QString &s)
{ 
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  k.append(s);
  settings.writeEntry(k, value);
}

void RcFile::saveData (Parm name, QString &value, const int n)
{ 
  QString s;
  s.append(QString::number(n));
  saveData(name, value, s);
}

void RcFile::saveData (Parm name, int &value)
{
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  QString v = QString::number(value);
  settings.writeEntry(k, v);
}

void RcFile::saveData (Parm name, int &value, const int n)
{
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  k.append(QString::number(n));
  QString v = QString::number(value);
  settings.writeEntry(k, v);
}

void RcFile::saveData (Parm name, bool &value)
{  
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  
  settings.writeEntry(k, value);
}

//
// for special purposes   
//
void RcFile::loadColor (Parm name, QColor &color)
{
  QString c = color.name();
  loadData(name, c);
  color.setNamedColor(c);
}
    
void RcFile::loadColor (Parm name, QColor &color, const int n)
{
  QString c = color.name();
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
  QString f =font.family() + "," +
              QString::number(font.pointSize()) + "," +
              QString::number(font.weight()) + "," +
              QString::number(font.italic ()) + "," +
              QString::number(font.bold ());
              
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

void RcFile::loadSplitterSize (Parm name, QSplitter *sp, const QString &n = "")
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

void RcFile::saveSplitterSize (Parm name, QSplitter *sp, const QString &n = "")
{
  QStringList stringList;
  QValueList<int> sizeList = sp->sizes();
  
  int loop;
  for (loop = 0; loop < (int) sizeList.count(); loop++)
    stringList.append(QString::number(sizeList[loop]));
  
  QString s(stringList.join(","));
  saveData(name, s, n);
}
