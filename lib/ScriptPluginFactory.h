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

#ifndef SCRIPT_PLUGIN_FACTORY_HPP
#define SCRIPT_PLUGIN_FACTORY_HPP

#include "ScriptPlugin.h"

#include <QHash>
#include <QLibrary>
#include <QStringList>

class ScriptPluginFactory
{
  public:
    ScriptPluginFactory ();
    ~ScriptPluginFactory ();
    ScriptPlugin * plugin (QString plugin);
    void setPluginList ();
    void getPluginList (QString &path, QStringList &list);
    
  protected:
    QString _path;
    QHash<QString, ScriptPlugin *> _plugins;
    QHash<QString, QLibrary *> _libs;
};

#endif
