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

#include "BarDate.h"

BarDate::BarDate ()
{
  tickFlag = FALSE;
}

BarDate::~BarDate ()
{
}

int BarDate::setDate (QString &d)
{
  QString s = d;
  while (s.contains("-"))
    s = s.remove(s.find("-", 0, TRUE), 1);
  
  while (s.contains(":"))
    s = s.remove(s.find(":", 0, TRUE), 1);

  while (s.contains(" "))
    s = s.remove(s.find(" ", 0, TRUE), 1);
  
  if (s.length() != 14)
  {
    qDebug("BarDate::setDate:bad string length %i", s.length());
    return TRUE;
  }
  
  QDate dt = QDate(s.left(4).toInt(), s.mid(4, 2).toInt(), s.mid(6, 2).toInt());
  if (! dt.isValid())
  {
    qDebug("BarDate::setDate: invalid date %s", s.latin1());
    return TRUE;
  }
  
  int hour = s.mid(8, 2).toInt();
  if (hour < 0 || hour > 23)
  {
    qDebug("BarDate::setDate: hour out of range %i", hour);
    return TRUE;
  }
    
  int min = s.mid(10, 2).toInt();
  if (min < 0 || min > 59)
  {
    qDebug("BarDate::setDate: minute out of range %i", min);
    return TRUE;
  }
  
  int sec = 0;
  QTime t(hour, min, sec, 0);
  if (! t.isValid())
  {
    qDebug("BarDate::setDate: invalid time");
    return TRUE;
  }
  
  date.setDate(dt);
  date.setTime(t);
    
  return FALSE;
}

int BarDate::setDate (QDate &d)
{
  date.setDate(d);
  if (date.date().isValid())
    return FALSE;
  else
    return TRUE;
}

int BarDate::setTime (QTime &d)
{
  date.setTime(QTime(d.hour(), d.minute(), 0, 0));
  if (date.time().isValid())
    return FALSE;
  else
    return TRUE;
}

QDate BarDate::getDate ()
{
  return date.date();
}

QTime BarDate::getTime ()
{
  return date.time();
}

void BarDate::getDateString (bool sepFlag, QString &d)
{
  if (sepFlag)
    d = date.date().toString("yyyy-MM-dd");
  else
    d = date.date().toString("yyyyMMdd");
}

void BarDate::getDateTimeString (bool sepFlag, QString &d)
{
  QString s;
  getDateString(sepFlag, s);
  
  if (sepFlag)
    s.append(" ");
    
  QString s2;
  getTimeString(sepFlag, s2);
  s.append(s2);
  d = s;
}

void BarDate::getTimeString (bool sepFlag, QString &d)
{
  if (sepFlag)
    d = date.time().toString("hh:mm:ss");
  else
    d = date.time().toString("hhmmss");
}

int BarDate::setTime (int h, int m, int)
{
  date.setTime(QTime(h, m, 0, 0));
  if (date.time().isValid())
    return FALSE;
  else
    return TRUE;
}

double BarDate::getDateValue ()
{
  QString s;
  getDateTimeString(FALSE, s);
  return s.toDouble();
}

void BarDate::addSecs (int d)
{
  date = date.addSecs(d);
}

void BarDate::addDays (int d)
{
  date = date.addDays(d);
}

void BarDate::addMonths (int d)
{
  date = date.addMonths(d);
}

bool BarDate::getTickFlag ()
{
  return tickFlag;
}

void BarDate::setTickFlag (bool d)
{
  tickFlag = d;
}

int BarDate::getHour ()
{
  return date.time().hour();
}

void BarDate::getDateTime (QDateTime &d)
{
  d = date;
}


