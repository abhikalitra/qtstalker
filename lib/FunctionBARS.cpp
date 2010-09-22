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

#include "FunctionBARS.h"
#include "Globals.h"

#include <QtDebug>

FunctionBARS::FunctionBARS ()
{
}

Curve * FunctionBARS::getBARS (QColor &_up, QColor &_down, QColor &_neutral)
{
  if (g_barData.count() < 2)
    return 0;

  Curve *line = new Curve(Curve::OHLC);

  int size = g_barData.count();

  int loop;
  for (loop = 0; loop < size; loop++)
  {
    CurveBar *bar = new CurveBar;
    Bar tbar = g_barData.getBar(loop);
    bar->setData(0, tbar.getData(Bar::BarFieldOpen));
    bar->setData(1, tbar.getData(Bar::BarFieldHigh));
    bar->setData(2, tbar.getData(Bar::BarFieldLow));
    bar->setData(3, tbar.getData(Bar::BarFieldClose));

    if (loop > 0)
    {
      Bar pbar = g_barData.getBar(loop - 1);
      if (tbar.getData(Bar::BarFieldClose) > pbar.getData(Bar::BarFieldClose))
        bar->setColor(_up);
      else
      {
        if (tbar.getData(Bar::BarFieldClose) < pbar.getData(Bar::BarFieldClose))
          bar->setColor(_down);
        else
          bar->setColor(_neutral);
      }
    }
    else
      bar->setColor(_neutral);

    line->setBar(loop, bar);
  }

  return line;
}

