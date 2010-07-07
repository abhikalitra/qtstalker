/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
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

#include "QSPluginFactory.h"

#include <QDir>
#include <QDebug>
#include <QFileInfo>

QSPluginFactory::QSPluginFactory ()
{
  _path = "/usr/local/lib/QuoteServer/plugins";
}

QSPluginFactory::~QSPluginFactory ()
{
  qDeleteAll(_plugins);
  qDeleteAll(_libs);
}

QSPlugin * QSPluginFactory::plugin (QString &plugin)
{
  QSPlugin *plug = _plugins.value(plugin);
  if (plug)
    return plug;

  QString file = _path;
  file.append("/lib" + plugin);

  QLibrary *lib = new QLibrary(file);
  QSPlugin *(*so)() = 0;
  so = (QSPlugin *(*)()) lib->resolve("createPlugin");
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

void QSPluginFactory::pluginList (QStringList &list)
{
  list.clear();

  QDir dir(_path);
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

