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

#include "Indicator.h"
#include <qobject.h>

Indicator::Indicator ()
{
  chartObjects.setAutoDelete(TRUE);
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

void Indicator::addChartObject (Setting *co)
{
  chartObjects.replace(co->getData("Name"), co);
}

void Indicator::deleteChartObject (QString d)
{
  chartObjects.remove(d);
}

Setting * Indicator::getChartObject (QString d)
{
  return chartObjects[d];
}

QStringList Indicator::getChartObjects ()
{
  QStringList l;
  QDictIterator<Setting> it(chartObjects);
  for (; it.current(); ++it)
    l.append(it.currentKey());

  return l;
}

bool Indicator::getMainPlot ()
{
  QString s = getData(QObject::tr("Plot"));
  if (! s.compare(QObject::tr("True")))
    return TRUE;
  else
    return FALSE;
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


