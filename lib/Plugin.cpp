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
}

Plugin::~Plugin ()
{
}

Plugin::PluginType Plugin::getPluginType ()
{
  return pluginType;
}

QString Plugin::getPluginName ()
{
  return pluginName;
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

Indicator * Plugin::getIndicator ()
{
  return 0;
}

void Plugin::clearOutput ()
{
}

int Plugin::indicatorPrefDialog (QWidget *)
{
  return 0;
}

void Plugin::loadIndicatorSettings (QString)
{
}

void Plugin::saveIndicatorSettings (QString)
{
}

PlotLine * Plugin::calculateCustom (QDict<PlotLine> *)
{
  return 0;
}

void Plugin::setIndicatorSettings (Setting)
{
}

Setting Plugin::getIndicatorSettings ()
{
  Setting set;
  return set;
}

void Plugin::setCustomFlag (bool)
{
}

void Plugin::setCustomFunction (QString)
{
}

void Plugin::setPlotType (int)
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

void Plugin::prefDialog (QWidget *)
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

void Plugin::saveSettings ()
{
}

//**********************************************************
//************** db plugin interface ***********************
//**********************************************************

int Plugin::openChart (QString)
{
  return 0;
}

void Plugin::setBarCompression (BarData::BarCompression)
{
}

void Plugin::setBarRange (int)
{
}

void Plugin::dump (QString)
{
}

Bar * Plugin::getBar (QString, QString)
{
  return 0;
}

Bar * Plugin::getLastBar ()
{
  return 0;
}

Bar * Plugin::getFirstBar ()
{
  return 0;
}

QStringList Plugin::getChartObjectsList ()
{
  return QStringList();
}

QPtrList<Setting> * Plugin::getChartObjects ()
{
  return 0;
}

void Plugin::setChartObject (QString, Setting *)
{
}

void Plugin::deleteChartObject (QString)
{
}

QString Plugin::getData (QString)
{
  return QString();
}

void Plugin::setData (QString, QString)
{
}

void Plugin::setBar (BarDate, double, double, double, double, double, double)
{
}

BarData * Plugin::getHistory ()
{
  return 0;
}

void Plugin::deleteData (QString)
{
}

void Plugin::dbPrefDialog ()
{
}

QString Plugin::createNew ()
{
  return QString();
}

void Plugin::setDb (DB *)
{
}

void Plugin::saveDbDefaults (BarData::BarType, QString, QString, QString, QString, QString, QString)
{
}

void Plugin::setDbPath (QString)
{
}


