/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#define VOLUME "Line Type,Histogram Bar,13,Type,VOL,0,Alert,False,0,Name,Volume,0,Color,red,5,Label,VOL,4,Plot,False,0,Color Bars,True,3"

Config::Config (QString p)
{
  path = p;
  libs.setAutoDelete(TRUE);
  plugins.setAutoDelete(TRUE);

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

  QStringList l = getIndicators();
  if (l.count() == 0)
    setIndicator(QObject::tr("Volume"), QStringList::split(",", VOLUME, FALSE));
  else
  {
    int loop;
    bool flag = TRUE;
    Setting *i = new Setting;
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      i->clear();
      i->parse(getIndicator(l[loop]));
      if (! i->getData("Plot").compare("False"))
      {
        flag = FALSE;
        break;
      }
    }

    if (flag)
      setIndicator(QObject::tr("Volume"), QStringList::split(",", VOLUME, FALSE));

    delete i;
  }
  
  // remove old paths to plugins
  QSettings settings;
  settings.removeEntry("/Qtstalker/IndicatorPluginPath");
  settings.removeEntry("/Qtstalker/QuotePluginPath");
}

Config::~Config ()
{
  plugins.clear();
  libs.clear();
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
    case ChartStyle:
      s = settings.readEntry("/Qtstalker/ChartStyle", QObject::tr("Bar Chart"));
      break;
    case Compression:
      s = settings.readEntry("/Qtstalker/Compression", QObject::tr("Daily Compression"));
      break;
    case Grid:
      s = settings.readEntry("/Qtstalker/Grid", "1");
      break;
    case Bars:
      s = settings.readEntry("/Qtstalker/Bars", QObject::tr("1 Year"));
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
    case UpColor:
      s = settings.readEntry("/Qtstalker/UpColor", "green");
      break;
    case DownColor:
      s = settings.readEntry("/Qtstalker/DownColor", "red");
      break;
    case NeutralColor:
      s = settings.readEntry("/Qtstalker/NeutralColor", "blue");
      break;
    case PaintBarIndicator:
      s = settings.readEntry("/Qtstalker/PaintBarIndicator", "");
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
    case NavigatorPosition:
      s = settings.readEntry("/Qtstalker/NavigatorPosition", "1");
      break;
    case IndicatorTabs:
      s = settings.readEntry("/Qtstalker/IndicatorTabs", "1");
      break;
    case StackedIndicator:
      s = settings.readEntry("/Qtstalker/StackedIndicator", "");
      break;
    case PAFBoxSize:
      s = settings.readEntry("/Qtstalker/PAFBoxSize", "0");
      break;
    case PAFReversal:
      s = settings.readEntry("/Qtstalker/PAFReversal", "3");
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
    case UpColor:
      settings.writeEntry("/Qtstalker/UpColor", d);
      break;
    case DownColor:
      settings.writeEntry("/Qtstalker/DownColor", d);
      break;
    case NeutralColor:
      settings.writeEntry("/Qtstalker/NeutralColor", d);
      break;
    case PaintBarIndicator:
      settings.writeEntry("/Qtstalker/PaintBarIndicator", d);
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
    case NavigatorPosition:
      settings.writeEntry("/Qtstalker/NavigatorPosition", d);
      break;
    case IndicatorTabs:
      settings.writeEntry("/Qtstalker/IndicatorTabs", d);
      break;
    case StackedIndicator:
      settings.writeEntry("/Qtstalker/StackedIndicator", d);
      break;
    case PAFBoxSize:
      settings.writeEntry("/Qtstalker/PAFBoxSize", d);
      break;
    case PAFReversal:
      settings.writeEntry("/Qtstalker/PAFReversal", d);
      break;
    default:
      break;
  }
}

QStringList Config::getIndicators ()
{
  QSettings settings;
  return settings.entryList("/Qtstalker/Indicator");
}

QStringList Config::getIndicator (QString n)
{
  QSettings settings;
  QString s = "/Qtstalker/Indicator/";
  s.append(n);
  return settings.readListEntry(s, ',');
}

void Config::setIndicator (QString k, QStringList d)
{
  QSettings settings;
  QString s = "/Qtstalker/Indicator/";
  s.append(k);
  settings.writeEntry(s, d, ',');
}

void Config::deleteIndicator (QString n)
{
  QSettings settings;
  QString s = "/Qtstalker/Indicator/";
  s.append(n);
  settings.removeEntry(s);
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

QStringList Config::getIndicatorPlugins ()
{
  QStringList l = getDirList(getData(IndicatorPluginPath));

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    l[loop].truncate(l[loop].length() - 3);
    l[loop].remove(0, 3);
  }

  return l;
}

QStringList Config::getQuotePlugins ()
{
  QStringList l = getDirList(getData(QuotePluginPath));

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    l[loop].truncate(l[loop].length() - 3);
    l[loop].remove(0, 3);
  }

  l.remove(QObject::tr("CBOT"));
  l.remove(QObject::tr("Ratio"));

  return l;
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
    default:
      break;
  }

  s.append("/lib");
  s.append(p);
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
    qDebug("Quote::Dll error\n");
    delete lib;
    return 0;
  }
}

void Config::closePlugins ()
{
  plugins.clear();
  libs.clear();
}






