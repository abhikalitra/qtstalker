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
#include <qobject.h>
#include <qdir.h>
#include <qlibrary.h>
#include <qsettings.h>
#include <qfile.h>
#include <qtextstream.h>

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
  s.append("/plugins");
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/plugins directory.");
  }

  s = home;
  s.append("/plugins/indicator");
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/plugins/indicator directory.");
  }
  setData(IndicatorPluginPath, s);

  s = home;
  s.append("/plugins/quote");
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/Qtstalker/plugins/quote directory.");
  }
  setData(QuotePluginPath, s);
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
      s = settings.readEntry("/Qtstalker/IndicatorPluginPath");
      break;
    case QuotePluginPath:
      s = settings.readEntry("/Qtstalker/QuotePluginPath");
      break;
    case Group:
      s = settings.readEntry("/Qtstalker/Group");
      break;
    case PlotFont:
      s = settings.readEntry("/Qtstalker/PlotFont", "Helvetica 9 50");
      break;
    case AppFont:
      s = settings.readEntry("/Qtstalker/AppFont", "Helvetica 9 50");
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
    default:
      break;
  }
}

QStringList Config::getGroup (QString n)
{
  QString s = getData(GroupPath);
  s.append("/");
  s.append(n);
  return getDirList(s);
}

QStringList Config::getGroupList ()
{
  return getDirList(getData(GroupPath));
}

void Config::setGroup (QString n, QStringList l)
{
  deleteGroup(n);

  QString s = getData(GroupPath);
  s.append("/");
  s.append(n);

  QDir dir;
  dir.mkdir(s, TRUE);

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QString s2 = "ln -s ";
    s2.append(l[loop]);
    s2.append(" ");
    s2.append(s);
    s2.append("/");
    QFileInfo fi(l[loop]);
    s2.append(fi.fileName());
    system (s2);
  }
}

void Config::deleteGroup (QString n)
{
  QString s =  "rm -r ";
  s.append(getData(GroupPath));
  s.append("/");
  s.append(n);
  system (s);
}

QStringList Config::getPortfolio (QString n)
{
  QString s = getData(PortfolioPath);
  s.append("/");
  s.append(n);
  return loadFile(s);
}

QStringList Config::getPortfolioList ()
{
  return getDirList(getData(PortfolioPath));
}

void Config::setPortfolio (QString n, QStringList l)
{
  QString s = getData(PortfolioPath);
  s.append("/");
  s.append(n);
  saveFile(s, l);
}

void Config::deletePortfolio (QString n)
{
  QString s = getData(PortfolioPath);
  QDir dir(s);
  s.append("/");
  s.append(n);
  dir.remove(s);
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

QStringList Config::getTest (QString n)
{
  QString s = getData(TestPath);
  s.append("/");
  s.append(n);
  return loadFile(s);
}

QStringList Config::getTestList ()
{
  return getDirList(getData(TestPath));
}

void Config::setTest (QString n, QStringList l)
{
  QString s = getData(TestPath);
  s.append("/");
  s.append(n);
  saveFile(s, l);
}

void Config::deleteTest (QString n)
{
  QString s = getData(TestPath);
  QDir dir(s);
  s.append("/");
  s.append(n);
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

QStringList Config::loadFile (QString path)
{
  QStringList l;

  QFile f(path);
  if (! f.open(IO_ReadOnly))
    return l;
  QTextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();

    if (s.length())
      l.append(s);
  }

  f.close();

  return l;
}

void Config::saveFile (QString path, QStringList l)
{
  QFile f(path);
  if (! f.open(IO_WriteOnly))
    return;
  QTextStream stream(&f);

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    stream << l[loop] << "\n";

  f.close();
}

void Config::installPlugin (QString selection)
{
  QLibrary *lib = new QLibrary(selection);
  Plugin *(*so)() = 0;
  so = (Plugin *(*)()) lib->resolve("create");
  if (so)
  {
    Plugin *plug = (*so)();

    QString type = plug->getPluginType();

    QString name = plug->getPluginName();

    QString sys = "cp -f ";
    sys.append(selection);
    sys.append(" ");

    while (1)
    {
      if (! type.compare("Indicator"))
      {
        sys.append(getData(IndicatorPluginPath));

        if (system(sys.latin1()) == -1)
          qDebug("Qtstalker: Shell error, unable to install plugin.");

        break;
      }

      if (! type.compare("Quote"))
      {
        sys.append(getData(QuotePluginPath));

        if (system(sys.latin1()) == -1)
          qDebug("Qtstalker: Shell error, unable to install plugin.");

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

QStringList Config::getIndicatorPlugins ()
{
  QStringList l = getDirList(getData(IndicatorPluginPath));

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    l[loop].truncate(l[loop].length() - 3);

  return l;
}

QStringList Config::getQuotePlugins ()
{
  QStringList l = getDirList(getData(QuotePluginPath));

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    l[loop].truncate(l[loop].length() - 3);

  return l;
}






