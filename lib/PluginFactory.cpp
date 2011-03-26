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

#include "PluginFactory.h"
#include "Globals.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

PluginFactory::PluginFactory ()
{
  QSettings settings(g_globalSettings);
  _path = settings.value("plugin_path").toString();
}

PluginFactory::~PluginFactory ()
{
  qDeleteAll(_libs);
}

Plugin * PluginFactory::plugin (QString plugin)
{
  Plugin *plug = 0;
  QString file = _path;
  file.append("/lib" + plugin);

  QLibrary *lib = new QLibrary(file);
  Plugin *(*so)() = 0;
  so = (Plugin *(*)()) lib->resolve("createPlugin");
  if (so)
  {
    plug = (*so)();
    _libs.insert(plugin, lib);
  }
  else
    delete lib;
  
  return plug;
}
