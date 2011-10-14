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

#include "ChartObjectData.h"
#include "DataFactory.h"
#include "DataBool.h"
#include "DataInteger.h"

#include <QtDebug>

ChartObjectData::ChartObjectData ()
{
  clear();

  set(_RO, new DataBool(FALSE));
  set(_Z, new DataInteger(1));
  set(_PEN, new DataInteger(1));
}

void ChartObjectData::clear ()
{
  _type = DataFactory::_CHART_OBJECT;

  qDeleteAll(_data);
  _data.clear();
}

int ChartObjectData::highLow (double &h, double &l)
{
  int rc = 1;
  h = -99999999;
  l = 99999999;

  QHashIterator<int, Data *> it(_data);
  while (it.hasNext())
  {
    it.next();
    int k = it.key();

    switch ((Parm) k)
    {
      case _PRICE:
      case _PRICE_2:
      case _HIGH:
      case _LOW:
      case _LEVEL_1:
      case _LEVEL_2:
      case _LEVEL_3:
      case _LEVEL_4:
      case _LEVEL_5:
      case _LEVEL_6:
      {
        double t = it.value()->toDouble();

        if (t > h)
        {
          h = t;
          rc = 0;
        }

        if (t < l)
        {
          l = t;
          rc = 0;
        }

        break;
      }
      default:
        break;
    }
  }

  return rc;
}

int ChartObjectData::set (int k, Data * d)
{
  Data *td = _data.value(k);
  if (td)
    delete td;

  _data.insert(k, d);

  return 0;
}

Data * ChartObjectData::toData (int k)
{
  return _data.value(k);
}

QString ChartObjectData::toSaveString ()
{
  QStringList l;

  QHashIterator<int, Data *> it(_data);
  while (it.hasNext())
  {
    it.next();
    if (! it.value()->toString().isEmpty())
      l << QString::number(it.key()) + ";" + QString::number(it.value()->type()) + ";" + it.value()->toString();
  }

  return l.join("\n");
}

int ChartObjectData::fromSaveString (QString d)
{
  clear();

  QStringList l = d.split("\n");

  DataFactory fac;
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList tl = l.at(loop).split(";");
    if (tl.count() != 3)
    {
      qDebug() << "ChartObjectData::fromString: " << l.at(loop);
      continue;
    }

    Data *setting = fac.data(tl.at(1));
    if (! setting)
    {
      qDebug() << "ChartObjectData::fromString: invalid Setting::Type" << tl.at(1);
      continue;
    }

    if (setting->set(tl.at(2)))
    {
      qDebug() << "ChartObjectData::fromString: Setting::fromString error" << tl.at(2);
      delete setting;
      continue;
    }

    set(tl.at(0).toInt(), setting);
  }

  return 0;
}
