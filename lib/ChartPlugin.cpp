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

#include "ChartPlugin.h"

ChartPlugin::ChartPlugin ()
{
  data = 0;
  scaler = 0;
  buffer = 0;
  minPixelspace = 0;
  startX = 0;
  indicatorFlag = FALSE;
  saveFlag = FALSE;
}

ChartPlugin::~ChartPlugin ()
{
}

void ChartPlugin::setChartInput (BarData *d, Scaler *s, QPixmap *p)
{
  data = d;
  scaler = s;
  buffer = p;
}

int ChartPlugin::getMinPixelspace ()
{
  return minPixelspace;
}

int ChartPlugin::getStartX ()
{
  return startX;
}

bool ChartPlugin::getIndicatorFlag ()
{
  return indicatorFlag;
}

QString ChartPlugin::getPluginName ()
{
  return pluginName;
}

//**********************************************************
//***************** VIRTUAL OVERRIDES **********************
//**********************************************************

void ChartPlugin::drawChart (int, int, int)
{
}

void ChartPlugin::saveSettings ()
{
}

void ChartPlugin::prefDialog (QWidget *)
{
}



