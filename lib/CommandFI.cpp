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
 *  MERCHANTABILITY or CommandFITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "CommandFI.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandFI::CommandFI ()
{
  _name = "FI";

  Data td(QString("adx"));
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input Close"));
  Entity::set(QString("CLOSE"), td);
  
  td = Data(QString("volume"));
  td.setLabel(QObject::tr("Input Volume"));
  Entity::set(QString("VOLUME"), td);
}

QString CommandFI::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("CLOSE"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandFI::run: invalid CLOSE" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("VOLUME"), td);
  Entity in2;
  if (svc.curve(script, td.toString(), in2))
  {
    qDebug() << "CommandFI::run: invalid VOLUME" << td.toString();
    return _returnCode;
  }

  CurveData line;
  if (getFI(in, in2, line))
  {
    qDebug() << "CommandFI::run: getFI error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}

int CommandFI::getFI (Entity &in, Entity &in2, Entity &line)
{
  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys2(in, in2, keys))
    return 1;

  KeyCurveBar cbkeys;
  int loop = 1;
  double force = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity cbar;
    if (in.toEntity(keys.at(loop), cbar))
      continue;
    Data close;
    if (cbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), close))
      continue;

    Entity ycbar;
    if (in.toEntity(keys.at(loop - 1), ycbar))
      continue;
    Data yclose;
    if (ycbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), yclose))
      continue;

    Entity vbar;
    if (in2.toEntity(keys.at(loop), vbar))
      continue;
    Data vol;
    if (vbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), vol))
      continue;

    double cdiff = close.toDouble() - yclose.toDouble();
    force = vol.toDouble() * cdiff;

    CurveBar b;
    b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(force));
    line.setEntity(keys.at(loop), b);
  }

  return 0;
}
