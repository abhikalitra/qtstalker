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

#include "Indicator.h"



Indicator::Indicator ()
{
  setData(IndicatorParmEnable, 0);
  setData(IndicatorParmTabRow, 1);
  setData(IndicatorParmDate, 1);
  setData(IndicatorParmLog, 0);
}

void Indicator::setData (IndicatorParm k, QString &d)
{
  data.insert(k, d);
}

void Indicator::getData (IndicatorParm k, QString &d)
{
  d.clear();
  d = data.value(k);
}

void Indicator::setData (IndicatorParm k, int d)
{
  data.insert(k, QString::number(d));
}

int Indicator::getIntData (IndicatorParm k)
{
  int d = 0;
  d = data.value(k).toInt();
  return d;
}

