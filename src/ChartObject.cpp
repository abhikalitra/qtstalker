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

#include "ChartObject.h"

ChartObject::ChartObject ()
{
  data = 0;
  scaler = 0;
  buffer = 0;
}

ChartObject::~ChartObject ()
{
}

void ChartObject::draw (int, int)
{
}

QString ChartObject::getDate ()
{
  return QString();
}

QString ChartObject::getDate2 ()
{
  return QString();
}

void ChartObject::setData (QString d)
{
  settings.clear();
  settings.parse(d);
}

QString ChartObject::getData (QString d)
{
  return settings.getData(d);
}

QString ChartObject::getString ()
{
  return settings.getString();
}

