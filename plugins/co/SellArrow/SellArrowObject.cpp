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

#include "SellArrowObject.h"
#include <qpoint.h>

SellArrowObject::SellArrowObject (QString indicator, QString n, BarDate d, double v)
{
  init();
  plot = indicator;
  name = n;
  date = d;
  value = v;
}

SellArrowObject::SellArrowObject ()
{
  init();
}

SellArrowObject::~SellArrowObject ()
{
}

void SellArrowObject::init ()
{
  color.setNamedColor("red");
  saveFlag = FALSE;
  status = Plot;
  grabHandles.setAutoDelete(TRUE);
  selectionArea.setAutoDelete(TRUE);
}

void SellArrowObject::setSaveFlag (bool d)
{
  saveFlag = d;
}

bool SellArrowObject::getSaveFlag ()
{
  return saveFlag;
}

void SellArrowObject::clearGrabHandles ()
{
  grabHandles.clear();
}

void SellArrowObject::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void SellArrowObject::clearSelectionArea ()
{
  selectionArea.clear();
}

void SellArrowObject::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

QString SellArrowObject::getPlot ()
{
  return plot;
}

QString SellArrowObject::getName ()
{
  return name;
}

void SellArrowObject::setDate (BarDate d)
{
  date = d;
}

BarDate SellArrowObject::getDate ()
{
  return date;
}

void SellArrowObject::setColor (QColor d)
{
  color =d;
}

QColor SellArrowObject::getColor ()
{
  return color;
}

void SellArrowObject::setValue (double d)
{
  value = d;
}

double SellArrowObject::getValue ()
{
  return value;
}

bool SellArrowObject::isSelected (QPoint point)
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

bool SellArrowObject::isGrabSelected (QPoint point)
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

void SellArrowObject::setStatus (Status d)
{
  status = d;
}

SellArrowObject::Status SellArrowObject::getStatus ()
{
  return status;
}

void SellArrowObject::getSettings (Setting &set)
{
  set.setData("Date", date.getDateTimeString(FALSE));
  set.setData("Value", QString::number(value));
  set.setData("Color", color.name());
  set.setData("Plot", plot);
  set.setData("Name", name);
  set.setData("Plugin", "SellArrow");
}

void SellArrowObject::setSettings (Setting &set)
{
  date.setDate(set.getData("Date"));
  value = set.getFloat("Value");
  color.setNamedColor(set.getData("Color"));
  plot = set.getData("Plot");
  name = set.getData("Name");
}

