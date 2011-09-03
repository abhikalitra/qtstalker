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

#include "CurveBar.h"

#include <QtDebug>

CurveBar::CurveBar ()
{
  clear();
}

void CurveBar::clear ()
{
  Data::clear();

  _type = "CURVE_BAR";
}

int CurveBar::highLow (double &h, double &l)
{
  int rc = 1;
  h = -99999999;
  l = 99999999;

  QHashIterator<QString, QVariant> it(_data);
  while (it.hasNext())
  {
    it.next();

    int k = it.key().toInt();

    switch ((Parm) k)
    {
      case _OPEN:
      case _HIGH:
      case _LOW:
      case _CLOSE:
      case _VOLUME:
      case _OI:
      case _VALUE:
      {
        double t = it.value().toDouble();

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
