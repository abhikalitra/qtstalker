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

#include "SettingColor.h"

#include <QtDebug>

SettingColor::SettingColor (int input, int output, QColor d)
{
  _type = _COLOR;
  _inputType = input;
  _outputType = output;
  _color = d;
}

SettingColor::SettingColor (QColor d)
{
  _type = _COLOR;
  _inputType = _NONE;
  _outputType = _NONE;
  _color = d;
}

void SettingColor::setColor (QColor d)
{
  _color = d;
}

QColor SettingColor::getColor ()
{
  return _color;
}

int SettingColor::fromString (QString d)
{
  QColor c(d);
  if (! c.isValid())
    return 1;

  _color = d;

  return 0;
}

QString SettingColor::toString ()
{
  return _color.name();
}
