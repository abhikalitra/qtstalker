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

#include "VerticalLineObject.h"
#include <qpoint.h>

VerticalLineObject::VerticalLineObject (QString indicator, QString n, BarDate d)
{
  init();
  plot = indicator;
  name = n;
  date = d;
}

VerticalLineObject::VerticalLineObject ()
{
  init();
}

VerticalLineObject::~VerticalLineObject ()
{
}

void VerticalLineObject::init ()
{
  color.setNamedColor("red");
  saveFlag = FALSE;
  status = Plot;
  grabHandles.setAutoDelete(TRUE);
  selectionArea.setAutoDelete(TRUE);
}

void VerticalLineObject::setSaveFlag (bool d)
{
  saveFlag = d;
}

bool VerticalLineObject::getSaveFlag ()
{
  return saveFlag;
}

void VerticalLineObject::clearGrabHandles ()
{
  grabHandles.clear();
}

void VerticalLineObject::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void VerticalLineObject::clearSelectionArea ()
{
  selectionArea.clear();
}

void VerticalLineObject::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

QString VerticalLineObject::getPlot ()
{
  return plot;
}

QString VerticalLineObject::getName ()
{
  return name;
}

void VerticalLineObject::setDate (BarDate d)
{
  date = d;
}

BarDate VerticalLineObject::getDate ()
{
  return date;
}

void VerticalLineObject::setColor (QColor d)
{
  color =d;
}

QColor VerticalLineObject::getColor ()
{
  return color;
}

bool VerticalLineObject::isSelected (QPoint point)
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

bool VerticalLineObject::isGrabSelected (QPoint point)
{
  int loop;
  for (loop = 0; loop < (int) grabHandles.count(); loop++)
  {
    QRegion *r = grabHandles.at(loop);
    if (r->contains(point))
      return TRUE;
  }
  
  return FALSE;
}

void VerticalLineObject::setStatus (Status d)
{
  status = d;
}

VerticalLineObject::Status VerticalLineObject::getStatus ()
{
  return status;
}

void VerticalLineObject::getSettings (Setting &set)
{
  QString s;
  date.getDateTimeString(FALSE, s);
  set.setData("Date", s);
  set.setData("Color", color.name());
  set.setData("Plot", plot);
  set.setData("Name", name);
  set.setData("Plugin", "VerticalLine");
}

void VerticalLineObject::setSettings (Setting &set)
{
  QString s = set.getData("Date");
  date.setDate(s);
  color.setNamedColor(set.getData("Color"));
  plot = set.getData("Plot");
  name = set.getData("Name");
}

