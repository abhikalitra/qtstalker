/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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
  version = 0.3;
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
  s.append(about);

  return s;
}

void Plugin::setDataPath (QString d)
{
  dataPath = d;
}

Setting * Plugin::getPluginSettings ()
{
  Setting *set = new Setting;

  QStringList key = getKeyList();

  int loop;
  for(loop = 0; loop < (int) key.count(); loop++)
  {
    set->set(key[loop], getData(key[loop]), getType(key[loop]));
    set->setList(key[loop], getList(key[loop]));
  }

  return set;
}

//**********************************************************
//************** indicator plugin interface ****************
//**********************************************************

void Plugin::calculate ()
{
}

void Plugin::setIndicatorInput (BarData *)
{
}

QMemArray<int> Plugin::getAlerts ()
{
  return 0;
}

QList<QColor> Plugin::getColorBars (QString, QString, QString)
{
  QList<QColor> l;
  return l;
}

int Plugin::getIndicatorLines ()
{
  return 0;
}

PlotLine * Plugin::getIndicatorLine (int)
{
  return 0;
}

void Plugin::clearOutput ()
{
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


//**********************************************************
//************** chart plugin interface ********************
//**********************************************************

void Plugin::drawChart (int, int, int)
{
}

void Plugin::setChartInput (BarData *, Scaler *, QPixmap *)
{
}

void Plugin::prefDialog ()
{
}

int Plugin::getMinPixelspace ()
{
  return 0;
}

int Plugin::getStartX ()
{
  return 0;
}

bool Plugin::getIndicatorFlag ()
{
  return FALSE;
}

