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
  hour = 0;
  min = 0;
  sec = 0;
}

BarDate::~BarDate ()
{
}

int BarDate::setDate (QString d)
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
    
  date = QDate(s.left(4).toInt(), s.mid(4, 2).toInt(), s.mid(6, 2).toInt());
  if (! date.isValid())
  {
    qDebug("BarDate::setDate: invalid date %s", s.latin1());
    return TRUE;
  }
  
  hour = s.mid(8, 2).toInt();
  if (hour < 0 || hour > 23)
  {
    qDebug("BarDate::setDate: hour out of range %i", hour);
    return TRUE;
  }
    
  min = s.mid(10, 2).toInt();
  if (min < 0 || min > 59)
  {
    qDebug("BarDate::setDate: minute out of range %i", min);
    return TRUE;
  }
  
  sec = s.mid(12, 2).toInt();
  if (sec < 0 || sec > 59)
  {
    qDebug("BarDate::setDate: second out of range %i", sec);
    return TRUE;
  }
    
  return FALSE;
}

int BarDate::setDate (QDate d)
{
  date = d;
  if (date.isValid())
    return FALSE;
  else
    return TRUE;
}

QDate BarDate::getDate ()
{
  return date;
}

QString BarDate::getDateString (bool sepFlag)
{
  if (sepFlag)
    return date.toString("yyyy-MM-dd");
  else
    return date.toString("yyyyMMdd");
}

QString BarDate::getDateTimeString (bool sepFlag)
{
  QString s = getDateString(sepFlag);
  
  if (sepFlag)
    s.append(" ");
    
  s.append(getTimeString(sepFlag));
  
  return s;
}

QString BarDate::getTimeString (bool sepFlag)
{
  QString s;
  
  if (hour < 10)
    s.append("0");
  s.append(QString::number(hour));
  
  if (sepFlag)
    s.append(":");

  if (min < 10)
    s.append("0");
  s.append(QString::number(min));
  
  if (sepFlag)
    s.append(":");
  
  if (sec < 10)
    s.append("0");
  s.append(QString::number(sec));

  return s;
}

int BarDate::setTime (int h, int m, int s)
{
  if (h < 0 || h > 23)
    return TRUE;
  hour = h;
    
  if (m < 0 || m > 59)
    return TRUE;
  min = m;
  
  if (s < 0 || s > 59)
    return TRUE;
  sec = s;
  
  return FALSE;
}

int BarDate::getHour ()
{
  return hour;
}

int BarDate::getMinute ()
{
  return min;
}

double BarDate::getDateValue ()
{
  QString s = getDateTimeString(FALSE);
  return s.toDouble();
}

void BarDate::addMinutes (int d)
{
  int loop;
  for (loop = 0; loop < d; loop++)
  {
    min++;
    if (min > 59)
    {
      min = 0;
      hour++;
      if (hour > 23)
      {
        date = date.addDays(1);
	hour = 0;
      }
    }
  }
}

void BarDate::subMinutes (int d)
{
  int loop;
  for (loop = 0; loop < d; loop++)
  {
    min--;
    if (min < 0)
    {
      min = 59;
      hour--;
      if (hour < 0)
      {
        date = date.addDays(-1);
	hour = 23;
      }
    }
  }
}



