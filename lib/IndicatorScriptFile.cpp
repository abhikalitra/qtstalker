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

#include "IndicatorScriptFile.h"

#include <QDebug>

IndicatorScriptFile::IndicatorScriptFile ()
{
  _q = '"';
  _reply = "$rc = <STDIN>; chomp($rc); if ($rc eq " + _q + "ERROR" + _q + ") { exit; }";
}

int IndicatorScriptFile::open (QString d)
{
  _file.setFileName(d);
  if (! _file.open(QIODevice::WriteOnly | QIODevice::Text))
    return 1;

  _stream.setDevice(&_file);

  _stream << "$|=1;\n\n";

  return 0;
}

void IndicatorScriptFile::close ()
{
  _file.close();
}

void IndicatorScriptFile::append (QString d)
{
  _stream << "print STDOUT " << _q << d << _q << ";\n";
  _stream << _reply;
  _stream << "\n\n";
}
