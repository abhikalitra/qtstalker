/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "Config.h"
#include <qobject.h>
#include <qdir.h>
#include <qlibrary.h>
#include <qsettings.h>
#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qapplication.h>
#include <qfileinfo.h>

Config::Config ()
{
  libs.setAutoDelete(TRUE);
  chartPlugins.setAutoDelete(TRUE);
  dbPlugins.setAutoDelete(TRUE);
  indicatorPlugins.setAutoDelete(TRUE);
  quotePlugins.setAutoDelete(TRUE);
  coPlugins.setAutoDelete(TRUE);
  version = "0.30";  // only this version of plugin is allowed to be loaded
}

Config::~Config ()
{
  chartPlugins.clear();
  dbPlugins.clear();
  indicatorPlugins.clear();
  quotePlugins.clear();
  coPlugins.clear();
  libs.clear();
}

void Config::setup ()
{
  QDir dir(QDir::homeDirPath());
  dir.convertToAbs();
  QString home = dir.absPath();
  home.append("/Qtstalker");
  if (! dir.exists(home, TRUE))
  {
    if (! dir.mkdir(home, TRUE))
      qDebug("Unable to create ~/Qtstalker directory.");
  }
  setData(Home, home);

  QString s = home + "/data";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/data directory.");
  }
  setData(DataPath, s);

  s = home + "/group";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/group directory.");
  }
  setData(GroupPath, s);

  s = home + "/portfolio";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/portfolio directory.");
  }
  setData(PortfolioPath, s);

  s = home + "/test";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/test directory.");
  }
  setData(TestPath, s);

  s = home + "/scanner";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/scanner directory.");
  }
  setData(ScannerPath, s);

  s = home + "/indicator";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/indicator directory.");
  }
  setData(IndicatorPath, s);
  
  s = home + "/indicator/Indicators";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/indicator/Indicators directory.");
    else
    {
      setData(IndicatorGroup, "Indicators"); // set the new default template
      
      // copy old indicators into new Indicators template
      s = home + "/indicator";
      QDir dir(s);
      int loop;
      for (loop = 2; loop < (int) dir.count(); loop++)
      {
        QString s2 = dir.absPath() + "/" + dir[loop];
        QFileInfo fi(s2);
	if (! fi.isDir())
	{
	  s = dir.absPath() + "/Indicators/" + dir[loop];
	  copyIndicatorFile(s2, s);
	  dir.remove(s2, TRUE);
	}
      }
    }
  }
  
  s = home + "/cusrules";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/cusrules directory.");
  }
  setData(CUSRulePath, s);

  s = home + "/macro";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/macro directory.");
  }
  setData(MacroPath, s);
  
  s = getData(AppFont);
  if (! s.length())
  {
    QFont font = QApplication::font(0);  
    s = font.family();
    s.append(",");
    s.append(QString::number(font.pointSize()));
    s.append(",");
    s.append(QString::number(font.weight()));
    setData(AppFont, s);
    setData(PlotFont, s);
  }
}

QString Config::getData (Parm p)
{
  QSettings settings;
  QString s;

  switch (p)
  {
    case Home:
      s = settings.readEntry("/Qtstalker/Home");
      break;
    case DataPath:
      s = settings.readEntry("/Qtstalker/DataPath");
      break;
    case GroupPath:
      s = settings.readEntry("/Qtstalker/GroupPath");
      break;
    case PortfolioPath:
      s = settings.readEntry("/Qtstalker/PortfolioPath");
      break;
    case TestPath:
      s = settings.readEntry("/Qtstalker/TestPath");
      break;
    case ScannerPath:
      s = settings.readEntry("/Qtstalker/ScannerPath");
      break;
    case IndicatorPath:
      s = settings.readEntry("/Qtstalker/IndicatorPath");
      break;
    case ChartStyle:
      s = settings.readEntry("/Qtstalker/ChartStyle", QObject::tr("Bar"));
      break;
    case Compression:
      s = settings.readEntry("/Qtstalker/Compression", "4");
      break;
    case Grid:
      s = settings.readEntry("/Qtstalker/Grid", "1");
      break;
    case Bars:
      s = settings.readEntry("/Qtstalker/Bars", "275");
      break;
    case BackgroundColor:
      s = settings.readEntry("/Qtstalker/BackgroundColor", "black");
      break;
    case BorderColor:
      s = settings.readEntry("/Qtstalker/BorderColor", "white");
      break;
    case GridColor:
      s = settings.readEntry("/Qtstalker/GridColor", "#626262");
      break;
    case ScaleToScreen:
      s = settings.readEntry("/Qtstalker/ScaleToScreen", "0");
      break;
    case IndicatorPluginPath:
      s = settings.readEntry("/Qtstalker/IndicatorPluginPath", "/usr/lib/qtstalker/indicator");
      break;
    case QuotePluginPath:
      s = settings.readEntry("/Qtstalker/QuotePluginPath", "/usr/lib/qtstalker/quote");
      break;
    case ChartPluginPath:
      s = settings.readEntry("/Qtstalker/ChartPluginPath", "/usr/lib/qtstalker/chart");
      break;
    case DbPluginPath:
      s = settings.readEntry("/Qtstalker/DbPluginPath", "/usr/lib/qtstalker/db");
      break;
    case COPluginPath:
      s = settings.readEntry("/Qtstalker/COPluginPath", "/usr/lib/qtstalker/co");
      break;
    case Group:
      s = settings.readEntry("/Qtstalker/Group");
      break;
    case PlotFont:
      s = settings.readEntry("/Qtstalker/PlotFont");
      break;
    case AppFont:
      s = settings.readEntry("/Qtstalker/AppFont");
      break;
    case NavAreaSize:
      s = settings.readEntry("/Qtstalker/NavAreaSize", "20");
      break;
    case LogScale:
      s = settings.readEntry("/Qtstalker/LogScale", "0");
      break;
    case Height:
      s = settings.readEntry("/Qtstalker/Height", "640");
      break;
    case Width:
      s = settings.readEntry("/Qtstalker/Width", "480");
      break;
    case X:
      s = settings.readEntry("/Qtstalker/X", "0");
      break;
    case Y:
      s = settings.readEntry("/Qtstalker/Y", "0");
      break;
    case Crosshairs:
      s = settings.readEntry("/Qtstalker/Crosshairs", "1");
      break;
    case DrawMode:
      s = settings.readEntry("/Qtstalker/DrawMode", "0");
      break;
    case CUSRulePath:
      s = settings.readEntry("/Qtstalker/CUSRulePath");
      break;
    case HelpFilePath:
      s = settings.readEntry("/Qtstalker/HelpFilePath", "/usr/share/doc/qtstalker/html");
      break;
    case LastQuotePlugin:
      s = settings.readEntry("/Qtstalker/LastQuotePlugin");
      break;
    case IndicatorPageStatus:
      s = settings.readEntry("/Qtstalker/IndicatorPageStatus");
      break;
    case PlotSizes:
      s = settings.readEntry("/Qtstalker/PlotSizes");
      break;
    case DataPanelSize:
      s = settings.readEntry("/Qtstalker/DataPanelSize");
      break;
    case MacroPath:
      s = settings.readEntry("/Qtstalker/MacroPath");
      break;
    case Menubar:
      s = settings.readEntry("/Qtstalker/Menubar", "1");
      break;
    case Macro1:
      s = settings.readEntry("/Qtstalker/Macro1");
      break;
    case Macro2:
      s = settings.readEntry("/Qtstalker/Macro2");
      break;
    case Macro3:
      s = settings.readEntry("/Qtstalker/Macro3");
      break;
    case Macro4:
      s = settings.readEntry("/Qtstalker/Macro4");
      break;
    case Macro5:
      s = settings.readEntry("/Qtstalker/Macro5");
      break;
    case Macro6:
      s = settings.readEntry("/Qtstalker/Macro6");
      break;
    case Macro7:
      s = settings.readEntry("/Qtstalker/Macro7");
      break;
    case Macro8:
      s = settings.readEntry("/Qtstalker/Macro8");
      break;
    case Macro9:
      s = settings.readEntry("/Qtstalker/Macro9");
      break;
    case Macro10:
      s = settings.readEntry("/Qtstalker/Macro10");
      break;
    case Macro11:
      s = settings.readEntry("/Qtstalker/Macro11");
      break;
    case Macro12:
      s = settings.readEntry("/Qtstalker/Macro12");
      break;
    case IndicatorGroup:
      s = settings.readEntry("/Qtstalker/IndicatorGroup", "Indicators");
      break;
    default:
      break;
  }

  return s;
}

QString Config::getData (QString p)
{
  QSettings settings;
  QString s = settings.readEntry("/Qtstalker/" + p);
  return s;
}

void Config::setData (Parm p, QString d)
{
  QSettings settings;

  switch (p)
  {
    case Home:
      settings.writeEntry("/Qtstalker/Home", d);
      break;
    case DataPath:
      settings.writeEntry("/Qtstalker/DataPath", d);
      break;
    case GroupPath:
      settings.writeEntry("/Qtstalker/GroupPath", d);
      break;
    case PortfolioPath:
      settings.writeEntry("/Qtstalker/PortfolioPath", d);
      break;
    case TestPath:
      settings.writeEntry("/Qtstalker/TestPath", d);
      break;
    case ScannerPath:
      settings.writeEntry("/Qtstalker/ScannerPath", d);
      break;
    case IndicatorPath:
      settings.writeEntry("/Qtstalker/IndicatorPath", d);
      break;
    case ChartStyle:
      settings.writeEntry("/Qtstalker/ChartStyle", d);
      break;
    case Compression:
      settings.writeEntry("/Qtstalker/Compression", d);
      break;
    case Grid:
      settings.writeEntry("/Qtstalker/Grid", d);
      break;
    case Bars:
      settings.writeEntry("/Qtstalker/Bars", d);
      break;
    case BackgroundColor:
      settings.writeEntry("/Qtstalker/BackgroundColor", d);
      break;
    case BorderColor:
      settings.writeEntry("/Qtstalker/BorderColor", d);
      break;
    case GridColor:
      settings.writeEntry("/Qtstalker/GridColor", d);
      break;
    case ScaleToScreen:
      settings.writeEntry("/Qtstalker/ScaleToScreen", d);
      break;
    case IndicatorPluginPath:
      settings.writeEntry("/Qtstalker/IndicatorPluginPath", d);
      break;
    case QuotePluginPath:
      settings.writeEntry("/Qtstalker/QuotePluginPath", d);
      break;
    case ChartPluginPath:
      settings.writeEntry("/Qtstalker/ChartPluginPath", d);
      break;
    case DbPluginPath:
      settings.writeEntry("/Qtstalker/DbPluginPath", d);
      break;
    case COPluginPath:
      settings.writeEntry("/Qtstalker/COPluginPath", d);
      break;
    case Group:
      settings.writeEntry("/Qtstalker/Group", d);
      break;
    case PlotFont:
      settings.writeEntry("/Qtstalker/PlotFont", d);
      break;
    case AppFont:
      settings.writeEntry("/Qtstalker/AppFont", d);
      break;
    case NavAreaSize:
      settings.writeEntry("/Qtstalker/NavAreaSize", d);
      break;
    case LogScale:
      settings.writeEntry("/Qtstalker/LogScale", d);
      break;
    case Height:
      settings.writeEntry("/Qtstalker/Height", d);
      break;
    case Width:
      settings.writeEntry("/Qtstalker/Width", d);
      break;
    case X:
      settings.writeEntry("/Qtstalker/X", d);
      break;
    case Y:
      settings.writeEntry("/Qtstalker/Y", d);
      break;
    case Crosshairs:
      settings.writeEntry("/Qtstalker/Crosshairs", d);
      break;
    case DrawMode:
      settings.writeEntry("/Qtstalker/DrawMode", d);
      break;
    case CUSRulePath:
      settings.writeEntry("/Qtstalker/CUSRulePath", d);
      break;
    case HelpFilePath:
      settings.writeEntry("/Qtstalker/HelpFilePath", d);
      break;
    case LastQuotePlugin:
      settings.writeEntry("/Qtstalker/LastQuotePlugin", d);
      break;
    case IndicatorPageStatus:
      settings.writeEntry("/Qtstalker/IndicatorPageStatus", d);
      break;
    case PlotSizes:
      settings.writeEntry("/Qtstalker/PlotSizes", d);
      break;
    case DataPanelSize:
      settings.writeEntry("/Qtstalker/DataPanelSize", d);
      break;
    case MacroPath:
      settings.writeEntry("/Qtstalker/MacroPath", d);
      break;
    case Menubar:
      settings.writeEntry("/Qtstalker/Menubar", d);
      break;
    case Macro1:
      settings.writeEntry("/Qtstalker/Macro1", d);
      break;
    case Macro2:
      settings.writeEntry("/Qtstalker/Macro2", d);
      break;
    case Macro3:
      settings.writeEntry("/Qtstalker/Macro3", d);
      break;
    case Macro4:
      settings.writeEntry("/Qtstalker/Macro4", d);
      break;
    case Macro5:
      settings.writeEntry("/Qtstalker/Macro5", d);
      break;
    case Macro6:
      settings.writeEntry("/Qtstalker/Macro6", d);
      break;
    case Macro7:
      settings.writeEntry("/Qtstalker/Macro7", d);
      break;
    case Macro8:
      settings.writeEntry("/Qtstalker/Macro8", d);
      break;
    case Macro9:
      settings.writeEntry("/Qtstalker/Macro9", d);
      break;
    case Macro10:
      settings.writeEntry("/Qtstalker/Macro10", d);
      break;
    case Macro11:
      settings.writeEntry("/Qtstalker/Macro11", d);
      break;
    case Macro12:
      settings.writeEntry("/Qtstalker/Macro12", d);
      break;
    case IndicatorGroup:
      settings.writeEntry("/Qtstalker/IndicatorGroup", d);
      break;
    default:
      break;
  }
}

void Config::setData (QString p, QString d)
{
  QSettings settings;
  settings.writeEntry("/Qtstalker/" + p, d);
}

QStringList Config::getIndicators (QString d)
{
  QString s = getData(IndicatorPath) + "/" + d;
  return getDirList(s, TRUE);
}

void Config::getIndicator (QString d, Setting &set)
{
  QFile f(d);
  if (! f.open(IO_ReadOnly))
  {
    qDebug("Config::getIndicator:can't open indicator file %s", d.latin1());
    return;
  }
  QTextStream stream(&f);
  
  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;
      
    QStringList l = QStringList::split("=", s, FALSE);
    if (l.count() != 2)
      continue;

    set.setData(l[0], l[1]);      
  }
  
  f.close();
}

void Config::setIndicator (QString d, Setting &set)
{
  QFile f(d);
  if (! f.open(IO_WriteOnly))
  {
    qDebug("Config::getIndicator:can't open indicator file %s", d.latin1());
    return;
  }
  QTextStream stream(&f);
  
  int loop;
  QStringList l = set.getKeyList();
  for (loop = 0; loop < (int) l.count(); loop++)
    stream << l[loop] << "=" << set.getData(l[loop]) << "\n";
  
  f.close();
}

void Config::deleteIndicator (QString d)
{
//  QString s = getData(IndicatorPath) + "/" + d + "/" + n;
  QDir dir;
  dir.remove(d);
}

QStringList Config::getDirList (QString path, bool flag)
{
  QStringList l;

  QDir dir(path);

  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QString s = dir.absPath() + "/" + dir[loop];
    QFileInfo fi(s);
    if (! fi.isDir())
    {
      if (flag)
        l.append(fi.absFilePath());
      else
        l.append(dir[loop]);
    }
  }

  return l;
}

QStringList Config::getPluginList (Config::Parm d) 
{
  QStringList l = getDirList(getData(d), TRUE);
  
  if (! l.count())
    return l;
  
  int loop;
  QStringList l2;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QFileInfo fi(l[loop]);
    if (! fi.fileName().contains(version))
      continue;

    QString s = fi.baseName(FALSE);
    s.remove(0, 3);
    l2.append(s);
  }

  return l2;
}

QStringList Config::getIndicatorList () 
{
  QStringList l = getPluginList(IndicatorPluginPath);
  l.remove("COMP");
  l.remove("MATH");
  l.remove("REF");
  l.remove("COUNTER");
  return l;
}

ChartPlugin * Config::getChartPlugin (QString p)
{
  ChartPlugin *plug = chartPlugins[p];
  if (plug)
    return plug;

//  QString s = getData(ChartPluginPath) + "/lib" + p + "." + version + ".so";
  QString s = getData(ChartPluginPath) + "/lib" + p + "." + version;

  QLibrary *lib = new QLibrary(s);
  ChartPlugin *(*so)() = 0;
  so = (ChartPlugin *(*)()) lib->resolve("createChartPlugin");
  if (so)
  {
    plug = (*so)();
    libs.replace(p, lib);
    chartPlugins.replace(p, plug);
    return plug;
  }
  else
  {
    qDebug("Config::getChartPlugin:%s Dll error\n", s.latin1());
    delete lib;
    return 0;
  }
}

DbPlugin * Config::getDbPlugin (QString p)
{
  DbPlugin *plug = dbPlugins[p];
  if (plug)
    return plug;

//  QString s = getData(DbPluginPath) + "/lib" + p + "." + version + ".so";
  QString s = getData(DbPluginPath) + "/lib" + p + "." + version;

  QLibrary *lib = new QLibrary(s);
  DbPlugin *(*so)() = 0;
  so = (DbPlugin *(*)()) lib->resolve("createDbPlugin");
  if (so)
  {
    plug = (*so)();
    libs.replace(p, lib);
    dbPlugins.replace(p, plug);
    return plug;
  }
  else
  {
    qDebug("Config::getDbPlugin:%s Dll error\n", s.latin1());
    delete lib;
    return 0;
  }
}

IndicatorPlugin * Config::getIndicatorPlugin (QString p)
{
  IndicatorPlugin *plug = indicatorPlugins[p];
  if (plug)
    return plug;

//  QString s = getData(IndicatorPluginPath) + "/lib" + p + "." + version + ".so";
  QString s = getData(IndicatorPluginPath) + "/lib" + p + "." + version;

  QLibrary *lib = new QLibrary(s);
  IndicatorPlugin *(*so)() = 0;
  so = (IndicatorPlugin *(*)()) lib->resolve("createIndicatorPlugin");
  if (so)
  {
    plug = (*so)();
    libs.replace(p, lib);
    indicatorPlugins.replace(p, plug);
    return plug;
  }
  else
  {
    qDebug("Config::getIndicatorPlugin:%s Dll error\n", s.latin1());
    delete lib;
    return 0;
  }
}

QuotePlugin * Config::getQuotePlugin (QString p)
{
  QuotePlugin *plug = quotePlugins[p];
  if (plug)
    return plug;

//  QString s = getData(QuotePluginPath) + "/lib" + p + "." + version + ".so";
  QString s = getData(QuotePluginPath) + "/lib" + p + "." + version;

  QLibrary *lib = new QLibrary(s);
  QuotePlugin *(*so)() = 0;
  so = (QuotePlugin *(*)()) lib->resolve("createQuotePlugin");
  if (so)
  {
    plug = (*so)();
    libs.replace(p, lib);
    quotePlugins.replace(p, plug);
    return plug;
  }
  else
  {
    qDebug("Config::getQuotePlugin:%s Dll error\n", s.latin1());
    delete lib;
    return 0;
  }
}

COPlugin * Config::getCOPlugin (QString p)
{
  COPlugin *plug = coPlugins[p];
  if (plug)
    return plug;

//  QString s = getData(COPluginPath) + "/lib" + p + "." + version + ".so";
  QString s = getData(COPluginPath) + "/lib" + p + "." + version;

  QLibrary *lib = new QLibrary(s);
  COPlugin *(*so)() = 0;
  so = (COPlugin *(*)()) lib->resolve("createCOPlugin");
  if (so)
  {
    plug = (*so)();
    libs.replace(p, lib);
    coPlugins.replace(p, plug);
    return plug;
  }
  else
  {
    qDebug("Config::getCOPlugin:%s Dll error\n", s.latin1());
    delete lib;
    return 0;
  }
}

void Config::closePlugins ()
{
  QDictIterator<ChartPlugin> it(chartPlugins);
  for (; it.current(); ++it)
    it.current()->saveSettings();
  
  chartPlugins.clear();
  dbPlugins.clear();
  indicatorPlugins.clear();
  quotePlugins.clear();
  
  libs.clear();
}

void Config::closePlugin (QString d)
{
  ChartPlugin *plug = chartPlugins[d];
  if (plug)
    plug->saveSettings();
   
  chartPlugins.remove(d);
  dbPlugins.remove(d);
  indicatorPlugins.remove(d);
  quotePlugins.remove(d);
  
  libs.remove(d);
}

QString Config::parseDbPlugin (QString d)
{
  QStringList l = QStringList::split("/", d, FALSE);
  int i = l.findIndex("Qtstalker");
  i = i + 2;
  return l[i];
}

void Config::copyIndicatorFile (QString d, QString d2)
{
  QFile f(d);
  if (! f.open(IO_ReadOnly))
  {
    qDebug("Config::copyFile:can't open input file %s", d.latin1());
    return;
  }
  QTextStream stream(&f);
  
  QFile f2(d2);
  if (! f2.open(IO_WriteOnly))
  {
    qDebug("Config::copyFile:can't open output file %s", d2.latin1());
    f.close();
    return;
  }
  QTextStream stream2(&f2);
  
  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    stream2 << s << "\n";
  }
  stream2 << "enable=1\n";
    
  f.close();
  f2.close();
}

