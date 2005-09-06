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

#include "HorizontalLineObject.h"
#include <qpoint.h>

HorizontalLineObject::HorizontalLineObject (QString indicator, QString n, double v)
{
  init();
  plot = indicator;
  name = n;
  value = v;
}

HorizontalLineObject::HorizontalLineObject ()
{
  init();
}

HorizontalLineObject::~HorizontalLineObject ()
{
}

void HorizontalLineObject::init ()
{
  color.setNamedColor("red");
  saveFlag = FALSE;
  status = Plot;
  text="";
  grabHandles.setAutoDelete(TRUE);
  selectionArea.setAutoDelete(TRUE);
}

void HorizontalLineObject::setSaveFlag (bool d)
{
  saveFlag = d;
}

bool HorizontalLineObject::getSaveFlag ()
{
  return saveFlag;
}

void HorizontalLineObject::clearGrabHandles ()
{
  grabHandles.clear();
}

void HorizontalLineObject::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void HorizontalLineObject::clearSelectionArea ()
{
  selectionArea.clear();
}

void HorizontalLineObject::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

QString HorizontalLineObject::getPlot ()
{
  return plot;
}

QString HorizontalLineObject::getName ()
{
  return name;
}

QString HorizontalLineObject::getText ()
{
  return text;
}

void HorizontalLineObject::setText(QString t)
{
  text=t;
}

void HorizontalLineObject::setColor (QColor d)
{
  color =d;
}

QColor HorizontalLineObject::getColor ()
{
  return color;
}

void HorizontalLineObject::setValue (double d)
{
  value = d;
}

double HorizontalLineObject::getValue ()
{
  return value;
}

bool HorizontalLineObject::isSelected (QPoint point)
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

bool HorizontalLineObject::isGrabSelected (QPoint point)
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

void HorizontalLineObject::setStatus (Status d)
{
  status = d;
}

HorizontalLineObject::Status HorizontalLineObject::getStatus ()
{
  return status;
}

void HorizontalLineObject::getSettings (Setting &set)
{
  set.setData("Value", QString::number(value));
  set.setData("Color", color.name());
  set.setData("Plot", plot);
  set.setData("Name", name);
  set.setData("Text", text);
  set.setData("Plugin", "HorizontalLine");
}

void HorizontalLineObject::setSettings (Setting &set)
{
  value = set.getFloat("Value");
  color.setNamedColor(set.getData("Color"));
  plot = set.getData("Plot");
  name = set.getData("Name");
  text = set.getData("Text");
}

