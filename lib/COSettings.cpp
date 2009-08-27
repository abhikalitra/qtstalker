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

#include "COSettings.h"



COSettings::COSettings (QString i, QString s, QString indi, QString t)
{
  id = i;
  symbol = s;
  indicator = indi;
  type = t;

  dateFormat = "yyyy-MM-dd HH:mm:ss";
}

COSettings::COSettings ()
{
  dateFormat = "yyyy-MM-dd HH:mm:ss";
}

COSettings::~COSettings ()
{
}

void COSettings::getString (QString &k, QString &d)
{
  d.clear();
  if (settings.contains(k))
    d = settings.value(k);
}

void COSettings::setString (QString &k, QString &d)
{
  settings.insert(k, d);
}

void COSettings::getColor (QColor &d)
{
  QString k("Color");
  if (settings.contains(k))
    d.setNamedColor(settings.value(k));
}

void COSettings::setColor (QColor &d)
{
  QString k("Color");
  QString s(d.name());
  settings.insert(k, s);
}

double COSettings::getValue ()
{
  QString k("Value");
  if (settings.contains(k))
    return settings.value(k).toDouble();
  else
    return 0;
}

void COSettings::setValue (double d)
{
  QString k("Value");
  settings.insert(k, QString::number(d));
}

double COSettings::getValue2 ()
{
  QString k("Value2");
  if (settings.contains(k))
    return settings.value(k).toDouble();
  else
    return 0;
}

void COSettings::setValue2 (double d)
{
  QString k("Value2");
  settings.insert(k, QString::number(d));
}

void COSettings::getSettings (QString &s)
{
  s.clear();
  QStringList l;
  QHashIterator<QString, QString> it(settings);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key() + "=" + it.value());
  }
  s = l.join("|");
}

void COSettings::parse (QString &d)
{
  settings.clear();

  QStringList l = d.split("|");

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QStringList l2 = l[loop].split("=");
    if (l2.count() > 1)
      settings.insert(l2[0], l2[1]);
  }
}

void COSettings::getType (QString &d)
{
  d = type;
}

void COSettings::setType (QString &d)
{
  type = d;
}

void COSettings::getID (QString &d)
{
  d = id;
}

void COSettings::setID (QString &d)
{
  id = d;
}

void COSettings::getIndicator (QString &d)
{
  d = indicator;
}

void COSettings::setIndicator (QString &d)
{
  indicator = d;
}

void COSettings::getSymbol (QString &d)
{
  d = symbol;
}

void COSettings::setSymbol (QString &d)
{
  symbol = d;
}

void COSettings::getDate (QDateTime &d)
{
  QString k("Date");
  if (settings.contains(k))
    d = QDateTime::fromString(settings.value(k), dateFormat);
}

void COSettings::setDate (QDateTime &d)
{
  QString k("Date");
  QString s = d.toString(dateFormat);
  settings.insert(k, s);
}

void COSettings::getDate2 (QDateTime &d)
{
  QString k("Date2");
  if (settings.contains(k))
    d = QDateTime::fromString(settings.value(k), dateFormat);
}

void COSettings::setDate2 (QDateTime &d)
{
  QString k("Date2");
  QString s = d.toString(dateFormat);
  settings.insert(k, s);
}

void COSettings::getText (QString &d)
{
  d.clear();
  QString k("Text");
  if (settings.contains(k))
    d = settings.value(k);
}

void COSettings::setText (QString &d)
{
  QString k("Text");
  settings.insert(k, d);
}


