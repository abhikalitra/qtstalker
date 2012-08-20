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

#include <QtDebug>
#include <QPluginLoader>
#include <QSettings>
#include <QDir>
#include "Global.h"

PluginFactory::PluginFactory ()
{
}

Plugin *
PluginFactory::load (QString d)
{

    qDebug() << "PluginFactory::loading" << d;
    QSettings settings(g_settings);
    QString plugindir = settings.value("plugin_directory").toString();
    QDir plugins(plugindir);

    if (!plugins.exists()) {
        qDebug() << "Cant locate plugin dir:" << plugindir;
        return 0;
    }

    QString pluginpath = plugins.filePath(d);

#if defined(Q_OS_WIN32)
    pluginpath.append(".dll");
#endif
#if defined(Q_OS_MAC)  
  s.append(".dylib");
#endif
#if defined(Q_OS_UNIX)  
  s.append(".so");
#endif
  
  qDebug() << "PluginFactory::loading" << pluginpath;
  
  QPluginLoader pluginLoader(pluginpath);
  QObject *tp = pluginLoader.instance();
  if (! tp)
  {
    qDebug() << "PluginFactory::load:" << pluginLoader.errorString();
    return 0;
  }
  else {
      qDebug() << "Loaded ok";
  }

  Plugin *plugin = qobject_cast<Plugin *>(tp);
  if (! plugin)
    qDebug() << "PluginFactory::load: error casting Plugin";

  return plugin;
}

/*
QObject *
PluginFactory::load (QString d)
{
  QString s = "/usr/local/lib/OTA/plugins/lib" + d;
#if defined(Q_OS_MAC)  
  s.append(".dylib");
#endif
#if defined(Q_OS_UNIX)  
  s.append(".so");
#endif
  
//qDebug() << "PluginFactory::loadDialog: loading" << s;
  
  QPluginLoader pluginLoader(s);
  QObject *tp = pluginLoader.instance();
  if (! tp)
    qDebug() << "PluginFactory::load:" << pluginLoader.errorString();

  return plugin;
}

PluginDialog *
PluginFactory::loadDialog (QString d)
{
  QObject *tp = load(d);
  if (! tp)
    return 0;

  PluginDialog *plugin = qobject_cast<PluginDialog *>(tp);
  if (! plugin)
    qDebug() << "PluginFactory::loadDialog: error casting PluginDialog";

  return plugin;
}

PluginDB *
PluginFactory::loadDB (QString d)
{
  QObject *tp = load(d);
  if (! tp)
    return 0;

  PluginDB *plugin = qobject_cast<PluginDB *>(tp);
  if (! plugin)
    qDebug() << "PluginFactory::loadDB: error casting PluginDB";

  return plugin;
}

PluginCurve *
PluginFactory::loadCurve (QString d)
{
  QObject *tp = load(d);
  if (! tp)
    return 0;

  PluginCurve *plugin = qobject_cast<PluginCurve *>(tp);
  if (! plugin)
    qDebug() << "PluginFactory::loadCurve: error casting PluginCurve";

  return plugin;
}

PluginMarker *
PluginFactory::loadMarker (QString d)
{
  QObject *tp = load(d);
  if (! tp)
    return 0;

  PluginMarker *plugin = qobject_cast<PluginMarker *>(tp);
  if (! plugin)
    qDebug() << "PluginFactory::loadMarker: error casting PluginMarker";

  return plugin;
}

PluginIndicator *
PluginFactory::loadIndicator (QString d)
{
  QObject *tp = load(d);
  if (! tp)
    return 0;

  PluginIndicator *plugin = qobject_cast<PluginIndicator *>(tp);
  if (! plugin)
    qDebug() << "PluginFactory::loadIndicator: error casting PluginIndicator";

  return plugin;
}

PluginScript *
PluginFactory::loadScript (QString d)
{
  QObject *tp = load(d);
  if (! tp)
    return 0;

  PluginScript *plugin = qobject_cast<PluginScript *>(tp);
  if (! plugin)
    qDebug() << "PluginFactory::loadScript: error casting PluginScript";

  return plugin;
}
*/
