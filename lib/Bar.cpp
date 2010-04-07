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
#include <QObject>

Bar::Bar ()
{
  error = RC_None;
  rangeFlag = FALSE;
}

int Bar::setDate (QDateTime &d)
{
  if (! d.isValid())
    return TRUE;

  if (rangeFlag)
  {
    if (d > displayDate)
      displayDate = d;
  }
  else
    startDate = d;
  
  return FALSE;
}

void Bar::setDateRange (QDateTime &dt, Bar::BarLength l)
{
  length = l;
  startDate = dt;
  rangeFlag = TRUE;

  switch (length)
  {
    case Bar::Minute1:
      startDate.setTime(QTime(startDate.time().hour(), startDate.time().minute(), 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(59);
      displayDate = endDate;
      displayDate = displayDate.addSecs(1);
      break;
    case Bar::Minute5:
    {
      int tint = startDate.time().minute() / 5;
      startDate.setTime(QTime(startDate.time().hour(), tint * 5, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(299);
      displayDate = endDate;
      displayDate = displayDate.addSecs(1);
      break;
    }
    case Bar::Minute10:
    {
      int tint = startDate.time().minute() / 10;
      startDate.setTime(QTime(startDate.time().hour(), tint * 10, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(599);
      displayDate = endDate;
      displayDate = displayDate.addSecs(1);
      break;
    }
    case Bar::Minute15:
    {
      int tint = startDate.time().minute() / 15;
      startDate.setTime(QTime(startDate.time().hour(), tint * 15, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(899);
      displayDate = endDate;
      displayDate = displayDate.addSecs(1);
      break;
    }
    case Bar::Minute30:
    {
      int tint = startDate.time().minute() / 30;
      startDate.setTime(QTime(startDate.time().hour(), tint * 30, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(1799);
      displayDate = endDate;
      displayDate = displayDate.addSecs(1);
      break;
    }
    case Bar::Minute60:
      startDate.setTime(QTime(startDate.time().hour(), 0, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(3599);
      displayDate = endDate;
      displayDate = displayDate.addSecs(1);
      break;
    case Bar::DailyBar:
      startDate.setTime(QTime(0, 0, 0, 0));
      endDate = startDate;
      endDate = endDate.addDays(1);
      endDate = endDate.addSecs(-1);
      displayDate = dt;
      break;
    case Bar::WeeklyBar:
      startDate.setTime(QTime(0, 0, 0, 0));
      startDate = startDate.addDays(- startDate.date().dayOfWeek());
      endDate = startDate;
      endDate = endDate.addDays(7);
      endDate = endDate.addSecs(-1);
      displayDate = dt;
      break;
    case Bar::MonthlyBar:
      startDate.setTime(QTime(0, 0, 0, 0));
      startDate = startDate.addDays(- (startDate.date().day() - 1));
      endDate = startDate;
      endDate = endDate.addDays(endDate.date().daysInMonth());
      endDate = endDate.addSecs(-1);
      displayDate = dt;
      break;
    default:
      break;
  }
}

QDateTime & Bar::getDate ()
{
  if (rangeFlag)
    return displayDate;
  else
    return startDate;
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
  d.clear();
  if (rangeFlag)
    d = displayDate.toString("yyyy-MM-dd");
  else
    d = startDate.toString("yyyy-MM-dd");
}

void Bar::getDateTimeString (QString &d)
{
  d.clear();
  if (rangeFlag)
    d = displayDate.toString(Qt::ISODate);
  else
    d = startDate.toString(Qt::ISODate);
}

void Bar::getTimeString (QString &d)
{
  d.clear();
  if (rangeFlag)
    d = displayDate.toString("HH:mm:ss");
  else
    d = startDate.toString("HH:mm:ss");
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

void Bar::getRangeKey (QString &d)
{
  d.clear();
  if (rangeFlag)
    d = startDate.toString(Qt::ISODate) + endDate.toString(Qt::ISODate);
}

void Bar::getBarLengthList (QStringList &l)
{
  l.clear();
  l << QObject::tr("1 Minute");
  l << QObject::tr("5 Minute");
  l << QObject::tr("10 Minute");
  l << QObject::tr("15 Minute");
  l << QObject::tr("30 Minute");
  l << QObject::tr("60 Minute");
  l << QObject::tr("Daily");
  l << QObject::tr("Weekly");
  l << QObject::tr("Monthly");
}

