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
  open = 0;
  high = 0;
  low = 0;
  close = 0;
  volume = 0;
  oi = 0;
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
  open = d;
}

double Bar::getOpen ()
{
  return open;
}

void Bar::setHigh (double d)
{
  high = d;
}

double Bar::getHigh ()
{
  return high;
}

void Bar::setLow (double d)
{
  low = d;
}

double Bar::getLow ()
{
  return low;
}

void Bar::setClose (double d)
{
  close = d;
}

double Bar::getClose ()
{
  return close;
}

void Bar::setVolume (double d)
{
  volume = d;
}

double Bar::getVolume ()
{
  return volume;
}

void Bar::setOI (int d)
{
  oi = d;
}

int Bar::getOI ()
{
  return oi;
}

QString Bar::getString ()
{
  QString s = date.getDateTimeString(TRUE);
  s.append(" ");
  s.append(QString::number(open));
  s.append(" ");
  s.append(QString::number(high));
  s.append(" ");
  s.append(QString::number(low));
  s.append(" ");
  s.append(QString::number(close));
  s.append(" ");
  s.append(QString::number(volume));
  s.append(" ");
  s.append(QString::number(oi));
  
  return s;
}

