/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "Plugin.h"

Plugin::Plugin ()
{
}

Plugin::~Plugin ()
{
}

QString Plugin::getPluginType ()
{
  return pluginType;
}

QString Plugin::getPluginName ()
{
  return pluginName;
}

QString Plugin::getVersion ()
{
  return QString::number(version);
}

QString Plugin::getAbout ()
{
  QString s = pluginName;
  s.append(" Version ");
  s.append(QString::number(version));
  s.append("\n");
  s.append(COPYRIGHT);
  s.append("\n");
  s.append(about);

  return s;
}

void Plugin::setDataPath (QString d)
{
  dataPath = d;
}

//**********************************************************
//************** indicator plugin interface ****************
//**********************************************************

void Plugin::calculate ()
{
}

void Plugin::setIndicatorInput (QList<Setting>)
{
}

QMemArray<int> Plugin::getAlerts ()
{
  return 0;
}

int Plugin::getIndicatorLines ()
{
  return 0;
}

Setting * Plugin::getIndicatorLineSettings (int)
{
  return 0;
}

QMemArray<double> Plugin::getIndicatorLineArray (int)
{
  return 0;
}

//**********************************************************
//************** quote plugin interface ********************
//**********************************************************

void Plugin::update ()
{
}

void Plugin::cancelUpdate ()
{
}

Setting * Plugin::getCreateDetails ()
{
  return 0;
}

void Plugin::createChart (Setting *)
{
}

bool Plugin::getCreateFlag ()
{
  return FALSE;
}


