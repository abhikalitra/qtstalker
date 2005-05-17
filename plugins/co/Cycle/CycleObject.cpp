/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#include "CycleObject.h"
#include <qpoint.h>

CycleObject::CycleObject (QString indicator, QString n, BarDate d, int i)
{
  init();
  plot = indicator;
  name = n;
  date = d;
  interval = i;
}

CycleObject::CycleObject ()
{
  init();
}

CycleObject::~CycleObject ()
{
}

void CycleObject::init ()
{
  color.setNamedColor("red");
  saveFlag = FALSE;
  status = Plot;
  grabHandles.setAutoDelete(TRUE);
  selectionArea.setAutoDelete(TRUE);
  interval = 10;
  grabPosition = -1;
}

void CycleObject::setSaveFlag (bool d)
{
  saveFlag = d;
}

bool CycleObject::getSaveFlag ()
{
  return saveFlag;
}

void CycleObject::clearGrabHandles ()
{
  grabHandles.clear();
}

void CycleObject::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void CycleObject::clearSelectionArea ()
{
  selectionArea.clear();
}

void CycleObject::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

QString CycleObject::getPlot ()
{
  return plot;
}

QString CycleObject::getName ()
{
  return name;
}

void CycleObject::setDate (BarDate d)
{
  date = d;
}

BarDate CycleObject::getDate ()
{
  return date;
}

void CycleObject::setColor (QColor d)
{
  color = d;
}

QColor CycleObject::getColor ()
{
  return color;
}

void CycleObject::setInterval (int d)
{
  interval = d;
}

int CycleObject::getInterval ()
{
  return interval;
}

bool CycleObject::isSelected (QPoint point)
{
  int loop;
  for (loop = 0; loop < (int) selectionArea.count(); loop++)
  {
    QRegion *r = selectionArea.at(loop);
    if (r->contains(point))
      return TRUE;
  }
  
  return FALSE;
}

bool CycleObject::isGrabSelected (QPoint point)
{
  grabPosition = -1;
  int loop;
  for (loop = 0; loop < (int) grabHandles.count(); loop++)
  {
    QRegion *r = grabHandles.at(loop);
    if (r->contains(point))
    {
      grabPosition = loop;
      return TRUE;
    }
  }
  
  return FALSE;
}

int CycleObject::getGrabPosition ()
{
  return grabPosition;
}

void CycleObject::setStatus (Status d)
{
  status = d;
}

CycleObject::Status CycleObject::getStatus ()
{
  return status;
}

void CycleObject::getSettings (Setting &set)
{
  QString s;
  date.getDateTimeString(FALSE, s);
  set.setData("Date", s);
  set.setData("Color", color.name());
  set.setData("Plot", plot);
  set.setData("Name", name);
  set.setData("Interval", QString::number(interval));
  set.setData("Plugin", "Cycle");
}

void CycleObject::setSettings (Setting &set)
{
  QString s = set.getData("Date");
  date.setDate(s);
  color.setNamedColor(set.getData("Color"));
  plot = set.getData("Plot");
  name = set.getData("Name");
  interval = set.getInt("Interval");
}

