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

#include "TrendLineObject.h"
#include <qpoint.h>

TrendLineObject::TrendLineObject (QString indicator, QString n, BarDate d, double v, BarDate d2, double v2)
{
  init();
  plot = indicator;
  name = n;
  date = d;
  date2 = d2;
  value = v;
  value2 = v2;
}

TrendLineObject::TrendLineObject ()
{
  init();
}

TrendLineObject::~TrendLineObject ()
{
}

void TrendLineObject::init ()
{
  color.setNamedColor("red");
  saveFlag = FALSE;
  status = Plot;
  grabHandles.setAutoDelete(TRUE);
  selectionArea.setAutoDelete(TRUE);
  extend = TRUE;
  usebar = FALSE;
  bar = "Close";
}

void TrendLineObject::setSaveFlag (bool d)
{
  saveFlag = d;
}

bool TrendLineObject::getSaveFlag ()
{
  return saveFlag;
}

void TrendLineObject::clearGrabHandles ()
{
  grabHandles.clear();
}

void TrendLineObject::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void TrendLineObject::clearSelectionArea ()
{
  selectionArea.clear();
}

void TrendLineObject::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

QString TrendLineObject::getPlot ()
{
  return plot;
}

QString TrendLineObject::getName ()
{
  return name;
}

void TrendLineObject::setDate (BarDate d)
{
  date = d;
}

BarDate TrendLineObject::getDate ()
{
  return date;
}

void TrendLineObject::setDate2 (BarDate d)
{
  date2 = d;
}

BarDate TrendLineObject::getDate2 ()
{
  return date2;
}

void TrendLineObject::setColor (QColor d)
{
  color =d;
}

QColor TrendLineObject::getColor ()
{
  return color;
}

void TrendLineObject::setValue (double d)
{
  value = d;
}

double TrendLineObject::getValue ()
{
  return value;
}

void TrendLineObject::setValue2 (double d)
{
  value2 = d;
}

double TrendLineObject::getValue2 ()
{
  return value2;
}

void TrendLineObject::setExtend (bool d)
{
  extend = d;
}

bool TrendLineObject::getExtend ()
{
  return extend;
}

void TrendLineObject::setUseBar (bool d)
{
  usebar = d;
}

bool TrendLineObject::getUseBar ()
{
  return usebar;
}

void TrendLineObject::setBar (QString d)
{
  bar = d;
}

QString TrendLineObject::getBar ()
{
  return bar;
}

bool TrendLineObject::isSelected (QPoint point)
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

int TrendLineObject::isGrabSelected (QPoint point)
{
  int loop;
  for (loop = 0; loop < (int) grabHandles.count(); loop++)
  {
    QRegion *r = grabHandles.at(loop);
    if (r->contains(point))
      return loop + 1;
  }
  
  return 0;
}

void TrendLineObject::setStatus (Status d)
{
  status = d;
}

TrendLineObject::Status TrendLineObject::getStatus ()
{
  return status;
}

void TrendLineObject::getSettings (Setting &set)
{
  set.setData("Start Date", date.getDateTimeString(FALSE));
  set.setData("End Date", date2.getDateTimeString(FALSE));
  set.setData("Start Value", QString::number(value));
  set.setData("End Value", QString::number(value2));
  set.setData("Bar Field", bar);
  set.setData("Use Bar", QString::number(usebar));
  set.setData("Extend Line", QString::number(extend));
  set.setData("Color", color.name());
  set.setData("Plot", plot);
  set.setData("Name", name);
  set.setData("Plugin", "TrendLine");
}

void TrendLineObject::setSettings (Setting &set)
{
  color.setNamedColor(set.getData("Color"));
  date.setDate(set.getData("Start Date"));
  date2.setDate(set.getData("End Date"));
  value = set.getFloat("Start Value");
  value2 = set.getFloat("End Value");
  bar = set.getData("Bar Field");
  usebar = set.getInt("Use Bar");
  extend = set.getInt("Extend Line");
  plot = set.getData("Plot");
  name = set.getData("Name");
}

