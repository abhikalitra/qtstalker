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
#include "Plugin.h"
#include <qobject.h>
#include <qdir.h>
#include <qlibrary.h>
#include <qsettings.h>
#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>

Config::Config ()
{
  libs.setAutoDelete(TRUE);
  plugins.setAutoDelete(TRUE);
  version = "0.26";  // only this version of plugin is allowed to be loaded
}

Config::~Config ()
{
  plugins.clear();
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

  QString s = home;
  s.append("/data");
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/data directory.");
  }
  setData(DataPath, s);

  s = home;
  s.append("/group");
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/group directory.");
  }
  setData(GroupPath, s);

  s = home;
  s.append("/portfolio");
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/portfolio directory.");
  }
  setData(PortfolioPath, s);

  s = home;
  s.append("/test");
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/test directory.");
  }
  setData(TestPath, s);

  s = home;
  s.append("/scanner");
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/scanner directory.");
  }
  setData(ScannerPath, s);

  s = home;
  s.append("/indicator");
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/indicator directory.");
  }
  setData(IndicatorPath, s);
  
  QStringList l = getIndicators();
  if (l.count() == 0)
  {
    Plugin *plug = getPlugin(Config::IndicatorPluginPath, "VOL");
    if (! plug)
    {
      qDebug("Config::Config: could not open plugin");
      closePlugin("VOL");
    }
    else
    {
      QString s = getData(Config::IndicatorPath) + "/Volume";
      plug->saveIndicatorSettings(s);
      closePlugin("VOL");
    }
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
    case MainPlotSize:
      s = settings.readEntry("/Qtstalker/MainPlotSize", "150");
      break;
    case IndicatorPlotSize:
      s = settings.readEntry("/Qtstalker/IndicatorPlotSize", "150");
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
    case Group:
      s = settings.readEntry("/Qtstalker/Group");
      break;
    case PlotFont:
      s = settings.readEntry("/Qtstalker/PlotFont", "Helvetica 8 50");
      break;
    case AppFont:
      s = settings.readEntry("/Qtstalker/AppFont", "Helvetica 9 50");
      break;
    case NavAreaSize:
      s = settings.readEntry("/Qtstalker/NavAreaSize", "20");
      break;
    case LogScale:
      s = settings.readEntry("/Qtstalker/LogScale", "0");
      break;
    case StackedIndicator:
      s = settings.readEntry("/Qtstalker/StackedIndicator", "");
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
    default:
      break;
  }

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
    case MainPlotSize:
      settings.writeEntry("/Qtstalker/MainPlotSize", d);
      break;
    case IndicatorPlotSize:
      settings.writeEntry("/Qtstalker/IndicatorPlotSize", d);
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
    case StackedIndicator:
      settings.writeEntry("/Qtstalker/StackedIndicator", d);
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
    default:
      break;
  }
}

QStringList Config::getIndicators ()
{
  return getDirList(getData(IndicatorPath));
}

Setting * Config::getIndicator (QString n)
{
  Setting *set = new Setting;
  
  QString s = getData(IndicatorPath) + "/" + n;
  
  QFile f(s);
  if (! f.open(IO_ReadOnly))
  {
    qDebug("Config::getIndicator:can't open indicator file %s", s.latin1());
    return set;
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

    set->setData(l[0], l[1]);      
  }
  
  f.close();
  
  return set;
}

void Config::deleteIndicator (QString n)
{
  QString s = getData(IndicatorPath);
  s.append("/" + n);
  QDir dir;
  dir.remove(s);
}

QStringList Config::getDirList (QString path)
{
  QStringList l;

  QDir dir(path);

  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
    l.append(dir[loop]);

  return l;
}

QStringList Config::getPluginList (Config::Parm d) 
{
  QStringList l = getDirList(getData(d));
  
  if (! l.count())
    return l;
  
  int loop;
  QStringList l2;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    if (! l[loop].contains(version))
      continue;

    QString s = l[loop];
    s.truncate(s.length() - 8);
    s.remove(0, 3);
    l2.append(s);
  }

  return l2;
}

Plugin * Config::getPlugin (Config::Parm t, QString p)
{
  Plugin *plug = plugins[p];
  if (plug)
    return plug;

  QString s;

  switch (t)
  {
    case Config::IndicatorPluginPath:
      s = getData(IndicatorPluginPath);
      break;
    case Config::QuotePluginPath:
      s = getData(QuotePluginPath);
      break;
    case Config::ChartPluginPath:
      s = getData(ChartPluginPath);
      break;
    default:
      break;
  }

  s.append("/lib");
  s.append(p);
  s.append(".");
  s.append(version);
  s.append(".so");

  QLibrary *lib = new QLibrary(s);
  Plugin *(*so)() = 0;
  so = (Plugin *(*)()) lib->resolve("create");
  if (so)
  {
    plug = (*so)();
    libs.replace(p, lib);
    plugins.replace(p, plug);
    return plug;
  }
  else
  {
    qDebug("Config::getPlugin:%s Dll error\n", s.latin1());
    delete lib;
    return 0;
  }
}

void Config::closePlugins ()
{
  QDictIterator<Plugin> it(plugins);
  for (; it.current(); ++it)
  {
    switch(it.current()->getPluginType())
    {
      case Plugin::ChartPlug:
        it.current()->saveSettings();
	break;
      default:
	break;
    }
  }
  
  plugins.clear();
  libs.clear();
}

void Config::closePlugin (QString d)
{
  Plugin *plug = plugins[d];
  if (plug)
  {
    switch(plug->getPluginType())
    {
      case Plugin::ChartPlug:
        plug->saveSettings();
	break;
      default:
	break;
    }
  }
   
  plugins.remove(d);
  libs.remove(d);
}

