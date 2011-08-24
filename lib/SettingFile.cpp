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

#include "SettingFile.h"

#include <QtDebug>

SettingFile::SettingFile (int input, int output, QStringList d)
{
  _type = _FILE;
  _inputType = input;
  _outputType = output;
  _file = d;
}

SettingFile::SettingFile (QStringList d)
{
  _type = _FILE;
  _inputType = _NONE;
  _outputType = _NONE;
  _file = d;
}

void SettingFile::setFile (QStringList d)
{
  _file = d;
}

QStringList SettingFile::getFile ()
{
  return _file;
}

int SettingFile::fromString (QString d)
{
  _file = d.split(",");
  return 0;
}

QString SettingFile::toString ()
{
  return _file.join(",");
}
