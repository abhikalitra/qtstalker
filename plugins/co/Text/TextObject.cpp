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

#include "TextObject.h"
#include <qpoint.h>

TextObject::TextObject (QString indicator, QString n, BarDate d, double v, QString l, QFont f)
{
  init();
  
  plot = indicator;
  name = n;
  date = d;
  label = l;
  font = f;
  value = v;
}

TextObject::TextObject ()
{
  init();
}

TextObject::~TextObject ()
{
}

void TextObject::init ()
{
  color.setNamedColor("white");
  saveFlag = FALSE;
  status = Plot;
  grabHandles.setAutoDelete(TRUE);
  selectionArea.setAutoDelete(TRUE);
  label = "Text";
  value = 0;
}

void TextObject::setSaveFlag (bool d)
{
  saveFlag = d;
}

bool TextObject::getSaveFlag ()
{
  return saveFlag;
}

void TextObject::clearGrabHandles ()
{
  grabHandles.clear();
}

void TextObject::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void TextObject::clearSelectionArea ()
{
  selectionArea.clear();
}

void TextObject::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

QString TextObject::getPlot ()
{
  return plot;
}

QString TextObject::getName ()
{
  return name;
}

void TextObject::setDate (BarDate d)
{
  date = d;
}

BarDate TextObject::getDate ()
{
  return date;
}

void TextObject::setColor (QColor d)
{
  color =d;
}

QColor TextObject::getColor ()
{
  return color;
}

void TextObject::setLabel (QString d)
{
  label = d;
}

QString TextObject::getLabel ()
{
  return label;
}

void TextObject::setValue (double d)
{
  value = d;
}

double TextObject::getValue ()
{
  return value;
}

void TextObject::setFont (QFont d)
{
  font = d;
}

QFont TextObject::getFont ()
{
  return font;
}

bool TextObject::isSelected (QPoint point)
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

bool TextObject::isGrabSelected (QPoint point)
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

void TextObject::setStatus (Status d)
{
  status = d;
}

TextObject::Status TextObject::getStatus ()
{
  return status;
}

Setting * TextObject::getSettings ()
{
  Setting *set = new Setting;
  set->setData("Date", date.getDateTimeString(FALSE));
  set->setData("Value", QString::number(value));
  set->setData("Color", color.name());
  set->setData("Plot", plot);
  set->setData("Name", name);
  set->setData("Label", label);
  set->setData("Plugin", "Text");
  
  QString s = font.family();
  s.append(" ");
  s.append(QString::number(font.pointSize()));
  s.append(" ");
  s.append(QString::number(font.weight()));
  set->setData("Font", s);
  
  return set;
}

void TextObject::setSettings (Setting *set)
{
  date.setDate(set->getData("Date"));
  value = set->getFloat("Value");
  color.setNamedColor(set->getData("Color"));
  plot = set->getData("Plot");
  name = set->getData("Name");
  label = set->getData("Label");
  
  QStringList l = QStringList::split(" ", set->getData("Font"), FALSE);
  if (l.count())
    font = QFont(l[0], l[1].toInt(), l[2].toInt());
}

