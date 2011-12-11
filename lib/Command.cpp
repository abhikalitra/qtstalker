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

#include "Command.h"
#include "DataFactory.h"

#include <QtDebug>

Command::Command (QObject *p) : QObject (p)
{
  _type = _THREAD;
//  _type = _WAIT;
}

Command::~Command ()
{
  qDeleteAll(_values);
  qDeleteAll(_tdata);

//  qDebug() << "Command::~Command" << _name << "deleted";
}

void Command::runScript (CommandParse, Script *)
{
}

int Command::type ()
{
  return _type;
}

QString Command::name ()
{
  return _name;
}

int Command::parse (CommandParse sg, Script *script)
{
  if (sg.values() != _values.count())
  {
    qDebug() << "Command::parse: invalid number of values";
    return 1;
  }

  int loop = 0;
  for (; loop < sg.values(); loop++)
  {
    if (parse1(script, sg.value(loop), loop))
      return 1;
  }

  return 0;
}


int Command::parse1 (Script *script, QString d, int pos)
{
  // is it a Data object?
  Data *sv = script->data(d);
  if (sv)
  {
    Data *v = _values.value(pos);

    // if its a curve, ignore it and just copy the value
    if (sv->type() == DataFactory::_CURVE)
    {
      if (v->set(d))
      {
        qDebug() << "Command::parse1: invalid value" << d <<"at pos" << pos;
        return 1;
      }
    }
    else
    {
      // not a curve, try to copy the object value
      if (v->set(sv->toString()))
      {
        qDebug() << "Command::parse1: invalid value" << d <<"at pos" << pos;
        return 1;
      }
    }
  }
  else
  {
    // its a string, try to convert to proper type
    Data *v = _values.value(pos);
    if (v->set(d))
    {
      qDebug() << "Command::parse1: invalid value" << d <<"at pos" << pos;
      return 1;
    }
  }

  return 0;
}

void Command::setTData (Data *d)
{
  _tdata << d;
}

void Command::done (QString d)
{
  QString s;
  if (d.isEmpty())
    s = "OK";
  else
    s = d;
  s.append("\n");

  emit signalDone(s);

  deleteLater();
}

void Command::error (QString d)
{
  qDebug() << d;
  QString s = "ERROR\n";
  emit signalDone(s);
  deleteLater();
}
