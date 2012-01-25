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

#include "CommandParse.h"

#include <QtDebug>

CommandParse::CommandParse ()
{
}

int CommandParse::parse (QString d)
{
  // COMMAND(VALUE, ...)

  _values.clear();

  // seperate () delimiters
  QStringList tl = d.split("(");
  if (tl.count() != 2)
  {
    qDebug() << "CommandParse::parse: missing (" << d;
    return 1;
  }

  if (! tl.at(1).contains(")"))
  {
    qDebug() << "CommandParse::parse: missing )" << d;
    return 1;
  }
  
  // get values
  QString s = tl.at(1);
  s = s.remove(")");
  s = s.trimmed();
  QStringList vl = s.split(",");
  
  int loop = 0;
  for (; loop < vl.count(); loop++)
    _values << vl.at(loop).trimmed();

  // get command
  _command = tl.at(0).trimmed();

  return 0;
}

int CommandParse::values ()
{
  return _values.count();
}

QString CommandParse::value (int d)
{
  if (d < 0 || d >= _values.count())
    return QString();

  return _values.at(d);
}

QString CommandParse::command ()
{
  return _command;
}
