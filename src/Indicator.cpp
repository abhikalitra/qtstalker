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
  plotType = TabPlot;
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

void Indicator::setPlotType (Indicator::PlotType d)
{
  plotType = d;
}

Indicator::PlotType Indicator::getPlotType ()
{
  return plotType;
}

void Indicator::clearLines ()
{
  lines.clear();
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

QStringList Indicator::getPlotTypes ()
{
  QStringList l;
  l.append(QObject::tr("Main"));
  l.append(QObject::tr("Tabbed"));
  l.append(QObject::tr("Stacked"));
  return l;
}

Indicator::PlotType Indicator::getPlotType (QString d)
{
  PlotType type = MainPlot;
  
  while (1)
  {
    if (! d.compare(QObject::tr("Tabbed")))
    {
      type = TabPlot;
      break;
    }
  
    if (! d.compare(QObject::tr("Stacked")))
    {
      type = StackedPlot;
      break;
    }
      
    break;
  }
  
  return type;
}

