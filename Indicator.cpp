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

#include "Indicator.h"
#include <qobject.h>

Indicator::Indicator ()
{
}

Indicator::~Indicator ()
{
}

QMemArray<int> Indicator::getLines ()
{
  return lines;
}

void Indicator::addLine (int d)
{
  lines.resize(lines.size() + 1);
  lines[lines.size() - 1] = d;
}

QMemArray<int> Indicator::getChartObjects ()
{
  QMemArray<int> a(chartObjects.count());
  QStringList keys = chartObjects.getKeyList();
  int loop;
  for (loop = 0; loop < (int) keys.count(); loop++)
    a[loop] = chartObjects.getInt(keys[loop]);

  return a;
}

void Indicator::addChartObject (QString n, int d)
{
  chartObjects.set(n, QString::number(d), Setting::None);
}

void Indicator::deleteChartObject (QString d)
{
  chartObjects.remove(d);
}

int Indicator::getChartObject (QString d)
{
  return chartObjects.getInt(d);
}

QStringList Indicator::getChartObjectNames ()
{
  return chartObjects.getKeyList();
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


