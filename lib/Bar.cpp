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

#include "Bar.h"

Bar::Bar ()
{
  max = -9999999999.0;
  min = 9999999999.0;
  data.setAutoDelete(TRUE);
}

Bar::~Bar ()
{
}

int Bar::setDate (BarDate d)
{
  date = d;
  return FALSE;
}

int Bar::setDate (QString d)
{
  if (date.setDate(d))
    return TRUE;
  else
    return FALSE;
}

BarDate Bar::getDate ()
{
  return date;
}

void Bar::setOpen (double d)
{
  setData("Open", d);
}

double Bar::getOpen ()
{
  BarItem *r = data["Open"];
  if (r)
    return r->v;
  else
    return 0;
}

void Bar::setHigh (double d)
{
  setData("High", d);
}

double Bar::getHigh ()
{
  BarItem *r = data["High"];
  if (r)
    return r->v;
  else
    return 0;
}

void Bar::setLow (double d)
{
  setData("Low", d);
}

double Bar::getLow ()
{
  BarItem *r = data["Low"];
  if (r)
    return r->v;
  else
    return 0;
}

void Bar::setClose (double d)
{
  setData("Close", d);
}

double Bar::getClose ()
{
  BarItem *r = data["Close"];
  if (r)
    return r->v;
  else
    return 0;
}

void Bar::setVolume (double d)
{
  setData("Volume", d);
}

double Bar::getVolume ()
{
  BarItem *r = data["Volume"];
  if (r)
    return r->v;
  else
    return 0;
}

void Bar::setOI (int d)
{
  setData("OI", d);
}

double Bar::getOI ()
{
  BarItem *r = data["OI"];
  if (r)
    return r->v;
  else
    return 0;
}

QString Bar::getString ()
{
  QString s = date.getDateTimeString(TRUE);
  
  QDictIterator<BarItem> it(data);
  for(; it.current(); ++it)
  {
    s.append(" ");
    s.append(QString::number(it.current()->v, 'g'));
  }
  return s;
}

QStringList Bar::getFields ()
{
  QStringList l;
  QDictIterator<BarItem> it(data);
  for(; it.current(); ++it)
    l.append(it.currentKey());  
  return l;
}

double Bar::getData (QString d)
{
  BarItem *r = data[d];
  if (r)
    return r->v;
  else
    return 0;
}

void Bar::setData (QString k, double d)
{
  BarItem *r = new BarItem;
  r->v = d;
  data.replace(k, r);
  
  if (d > max)
    max = d;
  if (d < min)
    min = d;
}

void Bar::copy (Bar *d)
{
  QDictIterator<BarItem> it(data);
  for(; it.current(); ++it)
    d->setData(it.currentKey(), it.current()->v);
  d->setDate(date);
}

double Bar::getMin ()
{
  return min;
}

double Bar::getMax ()
{
  return max;
}

