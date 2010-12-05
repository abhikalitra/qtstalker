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
#include "qtstalker_defines.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

ScriptPluginFactory::ScriptPluginFactory ()
{
  QSettings settings;
  settings.beginGroup("main"); // global setting, no session id appended

  _path = settings.value("script_plugin_path").toString();
  if (_path.isEmpty())
  {
    _path = QString("%1/qtstalker/plugins/script").arg(INSTALL_LIB_DIR);
    settings.setValue("script_plugin_path", _path);
    settings.sync();
  }
}

ScriptPluginFactory::~ScriptPluginFactory ()
{
//  qDeleteAll(_plugins);
  qDeleteAll(_libs);
}

ScriptPlugin * ScriptPluginFactory::plugin (QString plugin)
{
  ScriptPlugin *plug = 0;
//  ScriptPlugin *plug = _plugins.value(plugin);
//  if (plug)
//    return plug;

  QString file = _path;
  file.append("/lib" + plugin);

  QLibrary *lib = new QLibrary(file);
  ScriptPlugin *(*so)() = 0;
  so = (ScriptPlugin *(*)()) lib->resolve("createScriptPlugin");
  if (so)
  {
    plug = (*so)();
    _libs.insert(plugin, lib);
//    _plugins.insert(plugin, plug);
  }
  else
    delete lib;
  
  return plug;
}

void ScriptPluginFactory::setPluginList ()
{
  QStringList l;
  getPluginList(_path, l);

  QSettings settings;
  settings.beginGroup("main"); // global setting, no session id appended
  settings.setValue("script_plugin_list", l);
  settings.sync();
}

void ScriptPluginFactory::getPluginList (QString &path, QStringList &list)
{
  list.clear();

  QDir dir(path);
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QFileInfo fi(QString(dir.absolutePath() + "/" + dir[loop]));
    QString s = fi.baseName();
    s.remove(0, 3);
    list.append(s);
  }

  list.sort();
}

