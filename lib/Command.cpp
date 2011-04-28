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
  _returnData.clear();
  _returnCode = "ERROR";
  _indicator = 0;
  _barData = 0;
}

void Command::parse (QString d)
{
  _parms.clear();
  _returnCode = "ERROR";
  
  QStringList l = d.split(",", QString::SkipEmptyParts);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList l2 = l.at(loop).split("=", QString::SkipEmptyParts);
    if (l2.count() != 2)
      continue;

    strip(l2[0]);
    strip(l2[1]);

    _parms.insert(l2.at(0), l2.at(1));
  }
}

void Command::setParm (QString k, QString d)
{
  strip(k);
  strip(d);
  _parms.insert(k, d);
}

QString Command::plugin ()
{
  return _parms.value("PLUGIN");
}

void Command::setReturnData (QString k, QString d)
{
  _returnData.insert(k, d);
}

QString Command::returnData (QString k)
{
  return _returnData.value(k);
}

QByteArray Command::arrayReturnData (QString k)
{
  QByteArray ba;
  ba.append(_returnData.value(k));
  ba.append('\n');
  return ba;
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

void Command::strip (QString &d)
{
  d = d.remove(QString("="), Qt::CaseSensitive);
  d = d.remove(QString("|"), Qt::CaseSensitive);
  d = d.remove(QString("'"), Qt::CaseSensitive);
}

int Command::count ()
{
  return _parms.count();
}

QString Command::parm (QString k)
{
  return _parms.value(k);
}

void Command::setReturnCode (QString d)
{
  _returnCode = d;
}

QByteArray Command::returnCode ()
{
  QByteArray ba;
  ba.append(_returnCode);
  ba.append('\n');
  return ba;
}

void Command::setName (QString d)
{
  _name = d;
}

QString Command::name ()
{
  return _name;
}
