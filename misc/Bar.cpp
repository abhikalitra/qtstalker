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
}

Bar::~Bar ()
{
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
  QString s;
  getDateString(s);
  
  s.append(" ");
    
  QString s2;
  getTimeString(s2);
  s.append(s2);
  d = s;
}

void Bar::getTimeString (QString &d)
{
  d = date.toString("HH:mm:ss.zzz");
}

void Bar::clear ()
{
  date = QDateTime::currentDateTime();
  date.setTime(QTime(0,0,0,0));
  data.clear();
  emptyFlag = TRUE;
}

void Bar::setSymbol (QString &d)
{
  QString k("Symbol");
  data.insert(k, d);
}

void Bar::getSymbol (QString &d)
{
  d.clear();
  QString k("Symbol");
  d = data.value(k);
}

void Bar::setTime (QTime &d)
{
  if (! d.isValid())
    return;

  date.setTime(d);
  return;
}

void Bar::setData (QString &k, QString &d)
{
  data.insert(k, d);
}

void Bar::getData (QString &k, QString &d)
{
  d.clear();
  d = data.value(k);
}

bool Bar::getValidDate ()
{
  return date.isValid();
}

void Bar::getDateNumber (QString &d)
{
  d = date.toString("yyyyMMddHHmmsszzz");
}

void Bar::getDataString (QString &format, QString &s)
{
  QStringList l, l2;
  l = format.split(",");

  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    QString d;
    getData(l[loop], d);
    l2.append(d);
  }

  s = l2.join(",");
}


