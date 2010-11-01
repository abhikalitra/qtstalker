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

#include "ScriptPluginFactory.h"
#include "Config.h"
#include "qtstalker_defines.h"

#include <QDebug>

ScriptPluginFactory::ScriptPluginFactory ()
{
  Config config;
  config.getData(Config::ScriptPluginPath, _path);
  if (_path.isEmpty())
  {
    _path = QString("%1/qtstalker/plugins/script").arg(INSTALL_LIB_DIR);
    
    config.transaction();
    config.setData(Config::ScriptPluginPath, _path);
    config.commit();
  }
}

ScriptPluginFactory::~ScriptPluginFactory ()
{
  qDeleteAll(_plugins);
}

ScriptPlugin * ScriptPluginFactory::plugin (QString plugin)
{
  ScriptPlugin *plug = _plugins.value(plugin);
  if (plug)
    return plug;

  QString file = _path;
  file.append("/lib" + plugin);

  QLibrary *lib = new QLibrary(file);
  ScriptPlugin *(*so)() = 0;
  so = (ScriptPlugin *(*)()) lib->resolve("createScriptPlugin");
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

void ScriptPluginFactory::setPluginList ()
{
  QStringList l;
  getPluginList(_path, l);

  Config config;
  config.transaction();
  config.setData(Config::ScriptPluginList, l);
  config.commit();
}
