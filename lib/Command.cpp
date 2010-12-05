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
#include "ScriptPluginFactory.h"

#include <QtDebug>

Command::Command ()
{
  clear();
}

Command::Command (QString d)
{
  parse(d);
}

void Command::clear ()
{
  _parms.clear();
  _scriptFlag = 0;
  _returnData = "ERROR";
  _indicator = 0;
  _barData = 0;
}

void Command::parse (QString d)
{
  clear();
  _parms = d.split(",", QString::SkipEmptyParts);
}

QString Command::plugin ()
{
  QString s;
  if (_parms.count())
    s = _parms.at(0);
  return s;
}

void Command::setScriptFlag (int d)
{
  _scriptFlag = d;
}

int Command::scriptFlag ()
{
  return _scriptFlag;
}

int Command::count ()
{
  return _parms.count();
}

void Command::setReturnData (QString d)
{
  _returnData = d;
}

QByteArray Command::arrayData ()
{
  QByteArray ba;
  ba.append(_returnData);
  ba.append('\n');
  return ba;
}

QString & Command::stringData ()
{
  return _returnData;
}

QString Command::parm (int index)
{
  return _parms.at(index);
}

Indicator * Command::indicator ()
{
  return _indicator;
}

void Command::setIndicator (Indicator *i)
{
  _indicator = i;
}

BarData * Command::barData ()
{
  return _barData;
}

void Command::setBarData (BarData *d)
{
  _barData = d;
}
