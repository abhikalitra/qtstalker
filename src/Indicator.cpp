/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "Indicator.h"
#include <qobject.h>

Indicator::Indicator ()
{
  lines.setAutoDelete(TRUE);
  enable = TRUE;
}

Indicator::~Indicator ()
{
}

int Indicator::getLines ()
{
  return (int) lines.count();
}

PlotLine * Indicator::getLine (int i)
{
  return lines.at(i);
}

void Indicator::addLine (PlotLine *l)
{
  lines.append(l);
}

void Indicator::setMainPlot (bool d)
{
  plot = d;
}

bool Indicator::getMainPlot ()
{
  return plot;
}

QMemArray<int> Indicator::getAlerts ()
{
  return alerts;
}

void Indicator::setAlerts (QMemArray<int> d)
{
  alerts = d;
}

void Indicator::clearLines ()
{
  lines.clear();
}

int Indicator::getAlert (int i)
{
  return alerts[i];
}

void Indicator::setEnable (bool status)
{
  enable = status;
}

bool Indicator::getEnable ()
{
  return enable;
}

void Indicator::setName (QString d)
{
  name = d;
}

QString Indicator::getName ()
{
  return name;
}

void Indicator::setType (QString d)
{
  type = d;
}

QString Indicator::getType ()
{
  return type;
}

void Indicator::setFile (QString d)
{
  file = d;
}

QString Indicator::getFile ()
{
  return file;
}

