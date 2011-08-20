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

#include "SettingInteger.h"

#include <QtDebug>

SettingInteger::SettingInteger (int input, int output, int d, int high, int low)
{
  _type = _INTEGER;
  _inputType = input;
  _outputType = output;
  _data = d;
  _high = high;
  _low = low;
}

SettingInteger::SettingInteger (int d)
{
  _type = _INTEGER;
  _inputType = _NONE;
  _outputType = _NONE;
  _data = d;
  _high = 99999999;
  _low = -99999999;
}

void SettingInteger::setInteger (int d)
{
  _data = d;
}

int SettingInteger::getInteger ()
{
  return _data;
}

int SettingInteger::fromString (QString d)
{
  bool ok;
  int t = d.toInt(&ok);
  if (! ok)
    return 1;

  _data = t;

  return 0;
}

QString SettingInteger::toString ()
{
  return QString::number(_data);
}

int SettingInteger::getIntegerHigh ()
{
  return _high;
}

int SettingInteger::getIntegerLow ()
{
  return _low;
}
