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

#include "BuyArrowObject.h"
#include <qpoint.h>

BuyArrowObject::BuyArrowObject (QString indicator, QString n, BarDate d, double v)
{
  init();
  plot = indicator;
  name = n;
  date = d;
  value = v;
}

BuyArrowObject::BuyArrowObject ()
{
  init();
}

BuyArrowObject::~BuyArrowObject ()
{
}

void BuyArrowObject::init ()
{
  color.setNamedColor("green");
  saveFlag = FALSE;
  status = Plot;
  grabHandles.setAutoDelete(TRUE);
  selectionArea.setAutoDelete(TRUE);
}

void BuyArrowObject::setSaveFlag (bool d)
{
  saveFlag = d;
}

bool BuyArrowObject::getSaveFlag ()
{
  return saveFlag;
}

void BuyArrowObject::clearGrabHandles ()
{
  grabHandles.clear();
}

void BuyArrowObject::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void BuyArrowObject::clearSelectionArea ()
{
  selectionArea.clear();
}

void BuyArrowObject::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

QString BuyArrowObject::getPlot ()
{
  return plot;
}

QString BuyArrowObject::getName ()
{
  return name;
}

void BuyArrowObject::setDate (BarDate d)
{
  date = d;
}

BarDate BuyArrowObject::getDate ()
{
  return date;
}

void BuyArrowObject::setColor (QColor d)
{
  color =d;
}

QColor BuyArrowObject::getColor ()
{
  return color;
}

void BuyArrowObject::setValue (double d)
{
  value = d;
}

double BuyArrowObject::getValue ()
{
  return value;
}

bool BuyArrowObject::isSelected (QPoint point)
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

bool BuyArrowObject::isGrabSelected (QPoint point)
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

void BuyArrowObject::setStatus (Status d)
{
  status = d;
}

BuyArrowObject::Status BuyArrowObject::getStatus ()
{
  return status;
}

Setting * BuyArrowObject::getSettings ()
{
  Setting *set = new Setting;
  set->setData("Date", date.getDateTimeString(FALSE));
  set->setData("Value", QString::number(value));
  set->setData("Color", color.name());
  set->setData("Plot", plot);
  set->setData("Name", name);
  set->setData("Plugin", "BuyArrow");
  return set;
}

void BuyArrowObject::setSettings (Setting *set)
{
  date.setDate(set->getData("Date"));
  value = set->getFloat("Value");
  color.setNamedColor(set->getData("Color"));
  plot = set->getData("Plot");
  name = set->getData("Name");
}

