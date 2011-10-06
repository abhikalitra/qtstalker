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

#include <QtDebug>

ChartObjectData::ChartObjectData ()
{
  clear();
}

void ChartObjectData::clear ()
{
  Data::clear();
  _type = DataFactory::_CHART_OBJECT;
}

int ChartObjectData::highLow (double &h, double &l)
{
  int rc = 1;
  h = -99999999;
  l = 99999999;

  QHashIterator<QString, Setting *> it(_data);
  while (it.hasNext())
  {
    it.next();
    int k = it.key().toInt();

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