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

#include "PLUGIN.h"
#include "MiscPluginFactory.h"
#include "MiscPlugin.h"
#include "Globals.h"

#include <QtDebug>

PLUGIN::PLUGIN ()
{
}

int PLUGIN::command (QStringList &l, Indicator &, BarData &, QByteArray &ba)
{
  // PLUGIN,NAME,*
  //   0     1
  
  int rc = 1;
  ba.clear();
  ba.append("ERROR\n");

  if (l.count() < 2)
  {
    qDebug() << "PLUGIN::command: invalid parm count" << l.count();
    return rc;
  }

  MiscPluginFactory fac;
  MiscPlugin *plug = fac.plugin(l[1]);
  if (! plug)
  {
    qDebug() << "PLUGIN::command: invalid plugin" << l.at(1);
    return rc;
  }

  rc = plug->script(l);

  if (! rc)
  {
    ba.clear();
    ba.append("0\n");
  }

  return rc;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  PLUGIN *o = new PLUGIN;
  return ((ScriptPlugin *) o);
}

