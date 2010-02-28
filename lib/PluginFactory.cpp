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

#include "PluginFactory.h"
#include "Config.h"

#include <QDir>
#include <QDebug>
#include <QFileInfo>

PluginFactory::PluginFactory ()
{
}

PluginFactory::~PluginFactory ()
{
  qDeleteAll(indicatorPlugins);
  qDeleteAll(coPlugins);
  qDeleteAll(plotPlugins);
  qDeleteAll(libs);
}

void PluginFactory::setup ()
{
  // indicator plugin inventory
  Config config;
  QString s;
  config.getData(Config::IndicatorPluginPath, s);

  QDir dir(s);
  int loop;
  QStringList l;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QFileInfo fi(QString(dir.absolutePath() + "/" + dir[loop]));
    s = fi.baseName();
    s.remove(0, 3);
    l.append(s);
  }

  l.sort();
  config.setBaseData(Config::IndicatorPluginList, l);
  
  // co plugin inventory
  config.getData(Config::COPluginPath, s);

  dir.setPath(s);
  l.clear();
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QFileInfo fi(QString(dir.absolutePath() + "/" + dir[loop]));
    s = fi.baseName();
    s.remove(0, 3);
    l.append(s);
  }

  l.sort();
  config.setBaseData(Config::COPluginList, l);

  // plot plugin inventory
  config.getData(Config::PlotPluginPath, s);

  dir.setPath(s);
  l.clear();
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QFileInfo fi(QString(dir.absolutePath() + "/" + dir[loop]));
    s = fi.baseName();
    s.remove(0, 3);
    l.append(s);
  }

  l.sort();
  config.setBaseData(Config::PlotPluginList, l);
}

IndicatorPlugin * PluginFactory::getIndicator (QString &indicator)
{
  IndicatorPlugin *plug = indicatorPlugins.value(indicator);
  if (plug)
    return plug;

  Config config;
  QString file;
  config.getData(Config::IndicatorPluginPath, file);
  file.append("/lib" + indicator + ".so");
  plug = getIndicatorPlugin(indicator, file);

  return plug;
}

IndicatorPlugin * PluginFactory::getIndicatorPlugin (QString &indicator, QString &file)
{
  IndicatorPlugin *plug = 0;
  QLibrary *lib = new QLibrary(file);
  IndicatorPlugin *(*so)() = 0;
  so = (IndicatorPlugin *(*)()) lib->resolve("createIndicatorPlugin");
  if (so)
  {
    plug = (*so)();
    libs.insert(indicator, lib);
    indicatorPlugins.insert(indicator, plug);
    return plug;
  }
  else
  {
//    qDebug() << "PluginFactory::getIndicatorPlugin: loading error" << file;
    delete lib;
    return 0;
  }
}

COPlugin * PluginFactory::getCO (QString &co)
{
  COPlugin *plug = coPlugins.value(co);
  if (plug)
    return plug;

  Config config;
  QString file;
  config.getData(Config::COPluginPath, file);
  file.append("/lib" + co + ".so");
  plug = getCOPlugin(co, file);

  return plug;
}

COPlugin * PluginFactory::getCOPlugin (QString &co, QString &file)
{
  COPlugin *plug = 0;
  QLibrary *lib = new QLibrary(file);
  COPlugin *(*so)() = 0;
  so = (COPlugin *(*)()) lib->resolve("createCOPlugin");
  if (so)
  {
    plug = (*so)();
    libs.insert(co, lib);
    coPlugins.insert(co, plug);
    return plug;
  }
  else
  {
//    qDebug() << "PluginFactory::getIndicatorPlugin: loading error" << file;
    delete lib;
    return 0;
  }
}

PlotPlugin * PluginFactory::getPlot (QString &co)
{
  PlotPlugin *plug = plotPlugins.value(co);
  if (plug)
    return plug;

  Config config;
  QString file;
  config.getData(Config::PlotPluginPath, file);
  file.append("/lib" + co + ".so");
  plug = getPlotPlugin(co, file);

  return plug;
}

PlotPlugin * PluginFactory::getPlotPlugin (QString &co, QString &file)
{
  PlotPlugin *plug = 0;
  QLibrary *lib = new QLibrary(file);
  PlotPlugin *(*so)() = 0;
  so = (PlotPlugin *(*)()) lib->resolve("createPlotPlugin");
  if (so)
  {
    plug = (*so)();
    libs.insert(co, lib);
    plotPlugins.insert(co, plug);
    return plug;
  }
  else
  {
//    qDebug() << "PluginFactory::getIndicatorPlugin: loading error" << file;
    delete lib;
    return 0;
  }
}

