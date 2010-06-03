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

#include "DBPluginFactory.h"
#include "Config.h"

#include <QDebug>

DBPluginFactory::DBPluginFactory ()
{
  Config config;
  config.getData(Config::DBPluginPath, _path);
}

DBPluginFactory::~DBPluginFactory ()
{
  qDeleteAll(_plugins);
}

DBPlugin * DBPluginFactory::plugin (QString &plugin)
{
  DBPlugin *plug = _plugins.value(plugin);
  if (plug)
    return plug;

  QString file = _path;
  file.append("/lib" + plugin);

  QLibrary *lib = new QLibrary(file);
  DBPlugin *(*so)() = 0;
  so = (DBPlugin *(*)()) lib->resolve("createDBPlugin");
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

void DBPluginFactory::setPluginList ()
{
  QStringList l;
  getPluginList(_path, l);

  Config config;
  config.setData(Config::DBPluginList, l);
}

