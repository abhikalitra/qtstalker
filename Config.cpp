/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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
#include "ChartDb.h"
#include <qobject.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qlibrary.h>
#include <qsettings.h>

Config::Config (QString p)
{
  path = p;

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
  s.append("/plugins");
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/plugins directory.");
  }
  setData(PluginPath, s);

  s = home;
  s.append("/index");
  if (! dir.exists(s, TRUE))
    setData(IndexPath, s);

  scanPlugins();
}

Config::~Config ()
{
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
    case PluginPath:
      s = settings.readEntry("/Qtstalker/PluginPath");
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
      s = settings.readEntry("/Qtstalker/Bars", QObject::tr("All"));
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
    case Group:
      s = settings.readEntry("/Qtstalker/Group", "");
      break;
    case ScaleToScreen:
      s = settings.readEntry("/Qtstalker/ScaleToScreen", "0");
      break;
    case IndicatorPlugin:
      s = settings.readEntry("/Qtstalker/IndicatorPlugin", "");
      break;
    case QuotePlugin:
      s = settings.readEntry("/Qtstalker/QuotePlugin", "");
      break;
    case IndexPath:
      s = settings.readEntry("/Qtstalker/IndexPath");
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
    case PluginPath:
      settings.writeEntry("/Qtstalker/PluginPath", d);
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
    case Group:
      settings.writeEntry("/Qtstalker/Group", d);
      break;
    case ScaleToScreen:
      settings.writeEntry("/Qtstalker/ScaleToScreen", d);
      break;
    case IndicatorPlugin:
      settings.writeEntry("/Qtstalker/IndicatorPlugin", d);
      break;
    case QuotePlugin:
      settings.writeEntry("/Qtstalker/QuotePlugin", d);
      break;
    case IndexPath:
      settings.writeEntry("/Qtstalker/IndexPath", d);
      break;
    default:
      break;
  }
}

Setting * Config::getGroup (QString n)
{
  QSettings settings;
  QString s = "/Qtstalker/Group/";
  s.append(n);
  Setting *set = new Setting;
  set->parse(settings.readEntry(s, ""));
  return set;
}

QStringList Config::getGroupList ()
{
  QSettings settings;
  return settings.entryList("/Qtstalker/Group");
}

void Config::setGroup (QString n, Setting *set)
{
  QSettings settings;
  QString s = "/Qtstalker/Group/";
  s.append(n);
  settings.writeEntry(s, set->getString());
}

void Config::deleteGroup (QString n)
{
  QSettings settings;
  QString s = "/Qtstalker/Group/";
  s.append(n);
  settings.removeEntry(s);
}

QStringList Config::getPortfolio (QString n)
{
  QSettings settings;
  QString s = "/Qtstalker/Portfolio/";
  s.append(n);
  return settings.readListEntry(s, ',');
}

QStringList Config::getPortfolioList ()
{
  QSettings settings;
  return settings.entryList("/Qtstalker/Portfolio");
}

void Config::setPortfolio (QString n, QStringList l)
{
  QSettings settings;
  QString s = "/Qtstalker/Portfolio/";
  s.append(n);
  settings.writeEntry(s, l, ',');
}

void Config::deletePortfolio (QString n)
{
  QSettings settings;
  QString s = "/Qtstalker/Portfolio/";
  s.append(n);
  settings.removeEntry(s);
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

void Config::installPlugin (QString selection)
{
  QString sys = "cp -f ";
  sys.append(selection);
  sys.append(" ");
  sys.append(getData(PluginPath));

  QLibrary *lib = new QLibrary(selection);
  Plugin *(*so)() = 0;
  so = (Plugin *(*)()) lib->resolve("create");
  if (so)
  {
    Plugin *plug = (*so)();

    QString type = plug->getPluginType();

    QString name = plug->getPluginName();

    if (system(sys.latin1()) == -1)
      qDebug("Qtstalker: Shell error, unable to install plugin.");

    while (1)
    {
      if (! type.compare("Indicator"))
      {
        QStringList pl = QStringList::split(",", getData(IndicatorPlugin), FALSE);
        int i = pl.findIndex(name);
	if (i == -1)
	{
          pl.append(name);
	  pl.sort();
	  setData(IndicatorPlugin, pl.join(","));
	}
        break;
      }

      if (! type.compare("Quote"))
      {
        QStringList pl = QStringList::split(",", getData(QuotePlugin), FALSE);
        int i = pl.findIndex(name);
        if (i == -1)
        {
          pl.append(name);
  	  pl.sort();
          setData(QuotePlugin, pl.join(","));
        }
	break;
      }

      qDebug("Qtstalker: Invalid plugin.");
      break;
    }

    delete plug;
  }
  else
    qDebug("Qtstalker: Invalid plugin.");

  delete lib;
}

void Config::deletePlugin (QString n)
{
  QString s = getData(PluginPath);
  s.append("/");
  s.append(n);
  s.append(".so");
  QDir dir(s);
  dir.remove(s, TRUE);

  QStringList l = QStringList::split(",",  getData(IndicatorPlugin), FALSE);
  l.remove(n);
  setData(IndicatorPlugin, l.join(","));

  l = QStringList::split(",",  getData(QuotePlugin), FALSE);
  l.remove(n);
  setData(QuotePlugin, l.join(","));
}

void Config::scanPlugins ()
{
  qDebug("scanning plugins...");

  QString pp = getData(PluginPath);
  QDir dir(pp);

  QStringList ip;
  QStringList qp;

  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QString s = pp;
    s.append("/");
    s.append(dir[loop]);

    QLibrary *lib = new QLibrary(s);
    Plugin *(*so)() = 0;
    so = (Plugin *(*)()) lib->resolve("create");
    if (so)
    {
      Plugin *plug = (*so)();

      QString type = plug->getPluginType();

      QString name = plug->getPluginName();

      while (1)
      {
        if (! type.compare("Indicator"))
        {
          ip.append(name);
          break;
        }

        if (! type.compare("Quote"))
        {
          qp.append(name);
	  break;
        }

        qDebug("Qtstalker: %s invalid plugin.", dir[loop].latin1());

        break;
      }

      delete plug;
    }
    else
      qDebug("Qtstalker: Could not load %s plugin.", dir[loop].latin1());

    delete lib;
  }

  ip.sort();
  setData(IndicatorPlugin, ip.join(","));

  qp.sort();
  setData(QuotePlugin, qp.join(","));
}







