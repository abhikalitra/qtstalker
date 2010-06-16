/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "IndicatorPluginFactory.h"
#include "Config.h"

#include <QDebug>

IndicatorPluginFactory::IndicatorPluginFactory ()
{
  Config config;
  config.getData(Config::IndicatorPluginPath, _path);
  if (_path.isEmpty())
  {
    _path = "/usr/local/lib/qtstalker/plugins/indicator";
    config.setData(Config::IndicatorPluginPath, _path);
  }
}

IndicatorPluginFactory::~IndicatorPluginFactory ()
{
  qDeleteAll(_plugins);
}

IndicatorPlugin * IndicatorPluginFactory::plugin (QString &plugin)
{
  IndicatorPlugin *plug = _plugins.value(plugin);
  if (plug)
    return plug;

  QString file = _path;
  file.append("/lib" + plugin);

  QLibrary *lib = new QLibrary(file);
  IndicatorPlugin *(*so)() = 0;
  so = (IndicatorPlugin *(*)()) lib->resolve("createIndicatorPlugin");
  if (so)
  {
    plug = (*so)();
    _libs.insert(plugin, lib);
    _plugins.insert(plugin, plug);
  }
  else
    delete lib;
  
  return plug;
}

void IndicatorPluginFactory::setPluginList ()
{
  QStringList l;
  getPluginList(_path, l);

  Config config;
  config.setData(Config::IndicatorPluginList, l);
}

