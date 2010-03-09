/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "Bar.h"

#include <QtDebug>

Bar::Bar ()
{
  emptyFlag = TRUE;
}

int Bar::setDate (QDateTime &d)
{
  if (! d.isValid())
    return TRUE;

  date = d;
  return FALSE;
}

void Bar::getDate (QDateTime &d)
{
  d = date;
}

void Bar::setOpen (double d)
{
  data.insert(0, d);
  emptyFlag = FALSE;
}

double Bar::getOpen ()
{
  return data.value(0);
}

void Bar::setHigh (double d)
{
  data.insert(1, d);
  emptyFlag = FALSE;
}

double Bar::getHigh ()
{
  return data.value(1);
}

void Bar::setLow (double d)
{
  data.insert(2, d);
  emptyFlag = FALSE;
}

double Bar::getLow ()
{
  return data.value(2);
}

void Bar::setClose (double d)
{
  data.insert(3, d);
  emptyFlag = FALSE;
}

double Bar::getClose ()
{
  return data.value(3);
}

void Bar::setVolume (double d)
{
  data.insert(4, d);
  emptyFlag = FALSE;
}

double Bar::getVolume ()
{
  return data.value(4);
}

void Bar::setOI (double d)
{
  data.insert(5, d);
  emptyFlag = FALSE;
}

double Bar::getOI ()
{
  return data.value(5);
}

bool Bar::getEmptyFlag ()
{
  return emptyFlag;
}

void Bar::getDateString (QString &d)
{
  d = date.toString("yyyy-MM-dd");
}

void Bar::getDateTimeString (QString &d)
{
  d = date.toString(Qt::ISODate);
}

void Bar::getTimeString (QString &d)
{
  d = date.toString("HH:mm:ss");
}

void Bar::setTime (QTime &d)
{
  if (! d.isValid())
    return;

  date.setTime(d);
  return;
}

bool Bar::getValidDate ()
{
  return date.isValid();
}

void Bar::getDateNumber (QString &d)
{
  d = date.toString("yyyyMMddHHmmss");
}

void Bar::clear ()
{
  data.clear();
  emptyFlag = TRUE;
}
