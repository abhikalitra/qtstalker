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

#include "Scaler.h"
#include <math.h>

Scaler::Scaler ()
{
  dateHeight = DATE_HEIGHT;

  scaleList.append(".01");
  scaleList.append(".02");
  scaleList.append(".05");
  scaleList.append(".1");
  scaleList.append(".2");
  scaleList.append(".5");
  scaleList.append("1");
  scaleList.append("2");
  scaleList.append("5");
  scaleList.append("10");
  scaleList.append("25");
  scaleList.append("50");
  scaleList.append("100");
  scaleList.append("250");
  scaleList.append("500");
  scaleList.append("1000");
  scaleList.append("2500");
  scaleList.append("5000");
  scaleList.append("10000");
  scaleList.append("25000");
  scaleList.append("50000");
  scaleList.append("100000");
  scaleList.append("250000");
  scaleList.append("500000");
  scaleList.append("1000000");
  scaleList.append("2500000");
  scaleList.append("5000000");
  scaleList.append("10000000");
  scaleList.append("25000000");
  scaleList.append("50000000");
  scaleList.append("100000000");
  scaleList.append("250000000");
  scaleList.append("500000000");
  scaleList.append("1000000000");
  scaleList.append("2500000000");
  scaleList.append("5000000000");
  scaleList.append("10000000000");
  scaleList.append("25000000000");
  scaleList.append("50000000000");
  scaleList.append("100000000000");
  scaleList.append("250000000000");
  scaleList.append("500000000000");
}

Scaler::~Scaler ()
{
}

void Scaler::set (int ht, double h, double l, double lh, double lr, bool df, bool lf)
{
  height = ht;
  logScale = lf;
  dateFlag = df;
  scaleHigh = h;
  scaleLow = l;
  logScaleHigh = lh;
  logRange = lr;

  range = scaleHigh - scaleLow;
  scaler = height / range;
}

int Scaler::convertToY (double val)
{
  if (logScale)
  {
    if (val <= 0.0)
      return height;
    else
      return (int) (height * (logScaleHigh - log(val)) / logRange);
  }

  double t = val - scaleLow;
  int y = (int) (t * scaler);
  y = height - y;
  if (y > height)
    y = height;
  return y;
}

double Scaler::convertToVal (int y)
{
  if (logScale)
  {
    if (y >= height)
      return scaleLow;
    else
      return exp(logScaleHigh - ((y * logRange) / height));
  }

  int p = height - y;
  double val = scaleLow + (p / scaler) ;
  return val;
}

QArray<double> Scaler::getScaleArray ()
{
  int ticks;
  for (ticks = 2; (ticks * 15) < height; ticks++)
    ;
  ticks--;
  if (ticks > 10)
    ticks = 10;

  double interval = 0;
  int loop;
  for (loop = 0; loop < (int) scaleList.count(); loop++)
  {
    QString t = scaleList[loop];
    if ((range / t.toDouble()) < ticks)
    {
      interval = t.toDouble();
      break;
    }
  }

  scaleArray.resize(11);

  loop = 0;
  double t = 0 - (ticks * interval);
  while (t <= scaleHigh)
  {
    t = t + interval;

    if (t >= scaleLow)
    {
      scaleArray[loop] = t;
      loop++;
    }
  }

  scaleArray.resize(loop);

  return scaleArray;
}

double Scaler::getLogScaleHigh ()
{
  return logScaleHigh;
}

double Scaler::getLogRange ()
{
  return logRange;
}

int Scaler::getHeight ()
{
  return height;
}

bool Scaler::getDateFlag ()
{
  return dateFlag;
}

bool Scaler::getLogFlag ()
{
  return logScale;
}



