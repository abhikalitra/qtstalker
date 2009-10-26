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


#include "IndicatorSettings.h"

#include <QStringList>


IndicatorSettings::IndicatorSettings ()
{
}

void IndicatorSettings::addItem (QString &name, int type, QString &val)
{
  Item item;
  item.name = name;
  item.type = type;
  item.val = val;
  list.append(item);
}

void IndicatorSettings::addItem (QString &name, int type, int val)
{
  Item item;
  item.name = name;
  item.type = type;
  item.val = QString::number(val);
  list.append(item);
}

void IndicatorSettings::addItem (QString &name, int type, double val)
{
  Item item;
  item.name = name;
  item.type = type;
  item.val = QString::number(val);
  list.append(item);
}

void IndicatorSettings::getItem (int index, QString &name, int &type, QString &val)
{
  Item item = list.at(index);
  name = item.name;
  type = item.type;
  val = item.val;
}

void IndicatorSettings::clear ()
{
  list.clear();
}

int IndicatorSettings::count ()
{
  return list.count();
}

void IndicatorSettings::setIndicator (QString &n)
{
  indicator = n;
}

void IndicatorSettings::getIndicator (QString &n)
{
  n = indicator;
}

void IndicatorSettings::getString (QString &s)
{
  s.clear();

  QStringList l;
  int loop;
  for (loop = 0; loop < list.count(); loop++)
  {
    Item item = list.at(loop);
    l.append(item.name);
    l.append(QString::number(item.type));
    l.append(item.val);
  }

  if (l.count())
    s = l.join(",");
}

void IndicatorSettings::parse (QString &s)
{
  list.clear();

  QStringList l = s.split(",");
  int loop;
  for (loop = 0; loop < l.count(); loop = loop + 3)
  {
    Item item;
    item.name = l[loop];
    item.type = l[loop + 1].toInt();
    item.val = l[loop + 2];
    list.append(item);
  }
}

void IndicatorSettings::load (QStringList &l)
{
  int loop;
  for (loop = 2; loop < l.count(); loop++)
  {
    Item item = list.at(loop - 2);
    item.val = l[loop];
    list.replace(loop - 2, item);
  }
}

double IndicatorSettings::getItemDouble (int index)
{
  double d = 0;
  Item item = list.at(index);
  d = item.val.toDouble();
  return d;
}

int IndicatorSettings::getItemInt (int index)
{
  int d = 0;
  Item item = list.at(index);
  d = item.val.toInt();
  return d;
}

void IndicatorSettings::getItemVal (int index, QString &val)
{
  val.clear();
  
  Item item = list.at(index);
  val = item.val;
}

void IndicatorSettings::getItemInput (QString &val)
{
  val.clear();
  
  int loop;
  for (loop = 0; loop < list.count(); loop++)
  {
    Item item = list.at(loop);
    if (item.type == (Type) TypeInput)
    {
      val = item.val;
      break;
    }
  }
}

void IndicatorSettings::getItemInput2 (QString &val)
{
  val.clear();
  
  int loop;
  for (loop = 0; loop < list.count(); loop++)
  {
    Item item = list.at(loop);
    if (item.type == (Type) TypeInput2)
    {
      val = item.val;
      break;
    }
  }
}

void IndicatorSettings::getItemOutput (QString &val)
{
  val.clear();
  
  int loop;
  for (loop = 0; loop < list.count(); loop++)
  {
    Item item = list.at(loop);
    if (item.type == (Type) TypeOutput)
    {
      val = item.val;
      break;
    }
  }
}

void IndicatorSettings::getItemOutput2 (QString &val)
{
  val.clear();
  
  int loop;
  for (loop = 0; loop < list.count(); loop++)
  {
    Item item = list.at(loop);
    if (item.type == (Type) TypeOutput2)
    {
      val = item.val;
      break;
    }
  }
}

void IndicatorSettings::getItemOutput3 (QString &val)
{
  val.clear();
  
  int loop;
  for (loop = 0; loop < list.count(); loop++)
  {
    Item item = list.at(loop);
    if (item.type == (Type) TypeOutput3)
    {
      val = item.val;
      break;
    }
  }
}

void IndicatorSettings::getName (QString &s)
{
  s = name;
}

void IndicatorSettings::setName (QString &s)
{
  name = s;
}

