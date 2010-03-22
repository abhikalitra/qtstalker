/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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
  error = RC_None;
}

int Bar::setDate (QDateTime &d)
{
  if (! d.isValid())
    return TRUE;

  date = d;
  return FALSE;
}

QDateTime & Bar::getDate ()
{
  return date;
}

void Bar::setOpen (double d)
{
  data.insert(BarFieldOpen, d);
}

void Bar::setOpen (QString &s)
{
  bool ok;
  double d = s.toDouble(&ok);
  if (! ok)
  {
    error = RC_InvalidOpen;
    return;
  }

  data.insert(BarFieldOpen, d);
}

double Bar::getOpen ()
{
  return data.value(BarFieldOpen);
}

void Bar::setHigh (double d)
{
  data.insert(BarFieldHigh, d);
}

void Bar::setHigh (QString &s)
{
  bool ok;
  double d = s.toDouble(&ok);
  if (! ok)
  {
    error = RC_InvalidHigh;
    return;
  }

  data.insert(BarFieldHigh, d);
}

double Bar::getHigh ()
{
  return data.value(BarFieldHigh);
}

void Bar::setLow (double d)
{
  data.insert(BarFieldLow, d);
}

void Bar::setLow (QString &s)
{
  bool ok;
  double d = s.toDouble(&ok);
  if (! ok)
  {
    error = RC_InvalidLow;
    return;
  }

  data.insert(BarFieldLow, d);
}

double Bar::getLow ()
{
  return data.value(BarFieldLow);
}

void Bar::setClose (double d)
{
  data.insert(BarFieldClose, d);
}

void Bar::setClose (QString &s)
{
  bool ok;
  double d = s.toDouble(&ok);
  if (! ok)
  {
    error = RC_InvalidClose;
    return;
  }

  data.insert(BarFieldClose, d);
}

double Bar::getClose ()
{
  return data.value(BarFieldClose);
}

void Bar::setVolume (double d)
{
  data.insert(BarFieldVolume, d);
}

void Bar::setVolume (QString &s)
{
  bool ok;
  double d = s.toDouble(&ok);
  if (! ok)
  {
    error = RC_InvalidVolume;
    return;
  }

  data.insert(BarFieldVolume, d);
}

double Bar::getVolume ()
{
  return data.value(BarFieldVolume);
}

void Bar::setOI (double d)
{
  data.insert(BarFieldOI, d);
}

void Bar::setOI (QString &s)
{
  bool ok;
  double d = s.toDouble(&ok);
  if (! ok)
  {
    error = RC_InvalidOI;
    return;
  }

  data.insert(BarFieldOI, d);
}

double Bar::getOI ()
{
  return data.value(BarFieldOI);
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

void Bar::verify ()
{
  double open = getOpen();
  double high = getHigh();
  if (open > high)
  {
    error = RC_OGTH;
    return;
  }
    
  double low = getLow();
  if (open < low)
  {
    error = RC_OLTL;
    return;
  }
    
  double close = getClose();
  if (close > high)
  {
    error = RC_CGTH;
    return;
  }
    
  if (close < low)
  {
    error = RC_CLTL;
    return;
  }
    
  if (getVolume() < 0)
  {
    error = RC_VLT0;
    return;
  }
    
  if (low > high)
  {
    error = RC_LGTH;
    return;
  }
    
  if (high < low)
  {
    error = RC_HLTL;
    return;
  }
  
  if (data.contains(BarFieldOI))
  {
    if (getOI() < 0)
    {
      error = RC_OILT0;
      return;
    }
  }
}

int Bar::getError ()
{
  return error;
}

