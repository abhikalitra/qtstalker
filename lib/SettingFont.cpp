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

#include "SettingFont.h"

#include <QtDebug>

SettingFont::SettingFont (int input, int output, QFont d)
{
  _type = _FONT;
  _inputType = input;
  _outputType = output;
  _data = d;
}

SettingFont::SettingFont (QFont d)
{
  _type = _FONT;
  _inputType = _NONE;
  _outputType = _NONE;
  _data = d;
}

void SettingFont::setFont (QFont d)
{
  _data = d;
}

QFont SettingFont::getFont ()
{
  return _data;
}

int SettingFont::fromString (QString d)
{
  QFont f;
  if (! f.fromString(d))
    return 1;

  _data = f;

  return 0;
}

QString SettingFont::toString ()
{
  return _data.toString();
}
