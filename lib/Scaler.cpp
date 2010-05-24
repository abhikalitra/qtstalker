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

#include "Scaler.h"

#include <cmath>
#include <QString>

Scaler::Scaler ()
{
  _height = 0;
  _logScale = 0;
  _scaleHigh = 0;
  _scaleLow = 0;
  _logScaleHigh = 0;
  _logRange = 0;
  _range = 0;
  _scaler = 0;

  _scaleList.append(".00001");
  _scaleList.append(".00002");
  _scaleList.append(".00005");
  _scaleList.append(".0001");
  _scaleList.append(".0002");
  _scaleList.append(".0005");
  _scaleList.append(".001");
  _scaleList.append(".002");
  _scaleList.append(".005");
  _scaleList.append(".01");
  _scaleList.append(".02");
  _scaleList.append(".05");
  _scaleList.append(".1");
  _scaleList.append(".2");
  _scaleList.append(".5");
  _scaleList.append("1");
  _scaleList.append("2");
  _scaleList.append("5");
  _scaleList.append("10");
  _scaleList.append("25");
  _scaleList.append("50");
  _scaleList.append("100");
  _scaleList.append("250");
  _scaleList.append("500");
  _scaleList.append("1000");
  _scaleList.append("2500");
  _scaleList.append("5000");
  _scaleList.append("10000");
  _scaleList.append("25000");
  _scaleList.append("50000");
  _scaleList.append("100000");
  _scaleList.append("250000");
  _scaleList.append("500000");
  _scaleList.append("1000000");
  _scaleList.append("2500000");
  _scaleList.append("5000000");
  _scaleList.append("10000000");
  _scaleList.append("25000000");
  _scaleList.append("50000000");
  _scaleList.append("100000000");
  _scaleList.append("250000000");
  _scaleList.append("500000000");
  _scaleList.append("1000000000");
  _scaleList.append("2500000000");
  _scaleList.append("5000000000");
  _scaleList.append("10000000000");
  _scaleList.append("25000000000");
  _scaleList.append("50000000000");
  _scaleList.append("100000000000");
  _scaleList.append("250000000000");
  _scaleList.append("500000000000");
}

void Scaler::set (int ht, double h, double l, double lh, double lr, bool lf)
{
  _height = 0;
  _logScale = 0;
  _scaleHigh = 0;
  _scaleLow = 0;
  _logScaleHigh = 0;
  _logRange = 0;
  _range = 0;
  _scaler = 0;

  if (h - l == 0)
    return;
    
  _height = ht;
  _logScale = lf;
  _scaleHigh = h;
  _scaleLow = l;
  _logScaleHigh = lh;
  _logRange = lr;

  _range = _scaleHigh - _scaleLow;
  _scaler = _height / _range;
}

int Scaler::convertToY (double val)
{
  if (_logScale)
  {
    if (val <= 0.0)
      return _height;
    else
      return (int) (_height * (_logScaleHigh - log(val)) / _logRange);
  }

  double t = val - _scaleLow;
  int y = (int) (t * _scaler);
  y = _height - y;
  if (y > _height)
    y = _height;
  return y;
}

double Scaler::convertToVal (int y)
{
  if (_logScale)
  {
    if (y >= _height)
      return _scaleLow;
    else
      return exp(_logScaleHigh - ((y * _logRange) / _height));
  }

  if (_height == 0)
    return 0;
    
  int p = _height - y;
  double val = _scaleLow + (p / _scaler) ;
  return val;
}

void Scaler::scaleArray (QList<double> &scaleArray)
{
  int ticks;
  for (ticks = 2; (ticks * 15) < _height; ticks++)
    ;
  ticks--;
  if (ticks > 10)
    ticks = 10;
    
  double interval = 0;
  int loop;
  for (loop = 0; loop < (int) _scaleList.count(); loop++)
  {
    interval = _scaleList.at(loop).toDouble();
    if ((_range / interval) < ticks)
      break;
  }

  double t = 0 - (ticks * interval);
  
  if (interval > 0)
  {
    while (t <= _scaleHigh)
    {
      t += interval;

      if (t >= _scaleLow)
        scaleArray.append(t);
    }
  }
}

double Scaler::logScaleHigh ()
{
  return _logScaleHigh;
}

double Scaler::logRange ()
{
  return _logRange;
}

int Scaler::height ()
{
  return _height;
}

bool Scaler::logFlag ()
{
  return _logScale;
}

double Scaler::low ()
{
  return _scaleLow;
}

