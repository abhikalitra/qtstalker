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

#include "FiboLineObject.h"
#include <qpoint.h>

FiboLineObject::FiboLineObject (QString indicator, QString n, BarDate sd, double h, BarDate ed, double l)
{
  init();
  plot = indicator;
  name = n;
  startDate = sd;
  endDate = ed;
  high = h;
  low = l;
}

FiboLineObject::FiboLineObject ()
{
  init();
}

FiboLineObject::~FiboLineObject ()
{
}

void FiboLineObject::init ()
{
  color.setNamedColor("red");
  saveFlag = FALSE;
  status = Plot;
  grabHandles.setAutoDelete(TRUE);
  selectionArea.setAutoDelete(TRUE);
  line1 = 0.382;
  line2 = 0.5;
  line3 = 0.618;
  line4 = 0;
  line5 = 0;
  line6 = 0;
  extend = FALSE;
}

void FiboLineObject::setSaveFlag (bool d)
{
  saveFlag = d;
}

bool FiboLineObject::getSaveFlag ()
{
  return saveFlag;
}

void FiboLineObject::clearGrabHandles ()
{
  grabHandles.clear();
}

void FiboLineObject::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void FiboLineObject::clearSelectionArea ()
{
  selectionArea.clear();
}

void FiboLineObject::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

QString FiboLineObject::getPlot ()
{
  return plot;
}

QString FiboLineObject::getName ()
{
  return name;
}

void FiboLineObject::setStartDate (BarDate d)
{
  startDate = d;
}

BarDate FiboLineObject::getStartDate ()
{
  return startDate;
}

void FiboLineObject::setEndDate (BarDate d)
{
  endDate = d;
}

BarDate FiboLineObject::getEndDate ()
{
  return endDate;
}

void FiboLineObject::setColor (QColor d)
{
  color =d;
}

QColor FiboLineObject::getColor ()
{
  return color;
}

void FiboLineObject::setHigh (double d)
{
  high = d;
}

double FiboLineObject::getHigh ()
{
  return high;
}

void FiboLineObject::setLow (double d)
{
  low = d;
}

double FiboLineObject::getLow ()
{
  return low;
}

void FiboLineObject::setLine (int i, double d)
{
  switch (i)
  {
    case 1:
      line1 = d;
      break;
    case 2:
      line2 = d;
      break;
    case 3:
      line3 = d;
      break;
    case 4:
      line4 = d;
      break;
    case 5:
      line5 = d;
      break;
    case 6:
      line6 = d;
      break;
    default:
      break;
  }
}

double FiboLineObject::getLine (int i)
{
  double v = 0.0;
  
  switch (i)
  {
    case 1:
      v = line1;
      break;
    case 2:
      v = line2;
      break;
    case 3:
      v = line3;
      break;
    case 4:
      v = line4;
      break;
    case 5:
      v = line5;
      break;
    case 6:
      v = line6;
      break;
    default:
      break;
  }
  
  return v;
}

bool FiboLineObject::isSelected (QPoint point)
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

int FiboLineObject::isGrabSelected (QPoint point)
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

void FiboLineObject::setStatus (Status d)
{
  status = d;
}

FiboLineObject::Status FiboLineObject::getStatus ()
{
  return status;
}

void FiboLineObject::getSettings (Setting &set)
{
  QString s;
  set.setData("Color", color.name());
  set.setData("Plot", plot);
  set.setData("Name", name);
  set.setData("High", QString::number(high));
  set.setData("Low", QString::number(low));
  startDate.getDateTimeString(FALSE, s);
  set.setData("Start Date", s);
  endDate.getDateTimeString(FALSE, s);
  set.setData("End Date", s);
  set.setData("Line 1", QString::number(line1));
  set.setData("Line 2", QString::number(line2));
  set.setData("Line 3", QString::number(line3));
  set.setData("Line 4", QString::number(line4));
  set.setData("Line 5", QString::number(line5));
  set.setData("Line 6", QString::number(line6));
  set.setData("Extend", QString::number(extend));
  set.setData("Plugin", "FiboLine");
}

void FiboLineObject::setSettings (Setting &set)
{
  color.setNamedColor(set.getData("Color"));
  plot = set.getData("Plot");
  name = set.getData("Name");
  high = set.getFloat("High");
  low = set.getFloat("Low");
  QString s = set.getData("Start Date");
  startDate.setDate(s);
  s = set.getData("End Date");
  endDate.setDate(s);
  line1 = set.getFloat("Line 1");
  line2 = set.getFloat("Line 2");
  line3 = set.getFloat("Line 3");
  line4 = set.getFloat("Line 4");
  line5 = set.getFloat("Line 5");
  line6 = set.getFloat("Line 6");
  extend = set.getInt("Extend");
}

bool FiboLineObject::getExtend ()
{
  return extend;
}

void FiboLineObject::setExtend (bool d)
{
  extend = d;
}

