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
  QString k("Open");
  QString s = QString::number(d);
  data.insert(k, s);
  emptyFlag = FALSE;
}

double Bar::getOpen ()
{
  QString k("Open");
  return data.value(k).toDouble();
}

void Bar::setHigh (double d)
{
  QString k("High");
  QString s = QString::number(d);
  data.insert(k, s);
  emptyFlag = FALSE;
}

double Bar::getHigh ()
{
  QString k("High");
  return data.value(k).toDouble();
}

void Bar::setLow (double d)
{
  QString k("Low");
  QString s = QString::number(d);
  data.insert(k, s);
  emptyFlag = FALSE;
}

double Bar::getLow ()
{
  QString k("Low");
  return data.value(k).toDouble();
}

void Bar::setClose (double d)
{
  QString k("Close");
  QString s = QString::number(d);
  data.insert(k, s);
  emptyFlag = FALSE;
}

double Bar::getClose ()
{
  QString k("Close");
  return data.value(k).toDouble();
}

void Bar::setVolume (double d)
{
  QString k("Volume");
  QString s = QString::number(d);
  data.insert(k, s);
  emptyFlag = FALSE;
}

double Bar::getVolume ()
{
  QString k("Volume");
  return data.value(k).toDouble();
}

void Bar::setOI (double d)
{
  QString k("OI");
  QString s = QString::number(d);
  data.insert(k, s);
  emptyFlag = FALSE;
}

double Bar::getOI ()
{
  QString k("OI");
  return data.value(k).toDouble();
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
  d = date.toString("HH:mm:ss");
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
  emptyFlag = FALSE;
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
  d = date.toString("yyyyMMddHHmmss");
}

void Bar::copy (Bar *bar)
{
  bar->getDate(date);

  QStringList keys;
  bar->getKeys(keys);

  int loop;
  for (loop = 0; loop < keys.count(); loop++)
  {
    QString s;
    bar->getData(keys[loop], s);
    setData(keys[loop], s);
  }

  emptyFlag = FALSE;
}

void Bar::getKeys (QStringList &l)
{
  l.clear();
  QHashIterator<QString, QString> i(data);
  while (i.hasNext())
  {
    i.next();
    l.append(i.key());
  }  
}

void Bar::clear ()
{
  data.clear();
  emptyFlag = TRUE;
}
