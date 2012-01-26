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

#include "CommandPlotLine.h"
#include "TypeLine.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"
#include "KeyCurveData.h"
#include "KeyCurveBar.h"
#include "Script.h"

#include <QtDebug>

CommandPlotLine::CommandPlotLine ()
{
  _name = "PLOT_LINE";

  Entity::set(QString("OUTPUT"), Data(QString("plot_line")));
  Entity::set(QString("CHART"), Data(QString("chart")));
  Entity::set(QString("LABEL"), Data(QString("line")));
  Entity::set(QString("COLOR"), Data(QColor(Qt::red)));
  Entity::set(QString("Z"), Data(0));
  Entity::set(QString("PEN"), Data(1));
  Entity::set(QString("INPUT"), Data(QString("close")));

  TypeLine ls;
  Data td(ls.list(), QString("Line"));
  Entity::set(QString("STYLE"), td);
}

QString CommandPlotLine::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data input;
  Entity::toData(QString("INPUT"), input);
  
  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, input.toString(), in))
  {
    qDebug() << "CommandPlotLine::run: invalid Input" << input.toString();
    return _returnCode;
  }

  QList<int> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys1(in, keys))
  {
    qDebug() << "CommandPlotLine::run: invalid keys";
    return _returnCode;
  }

  KeyCurveData cdkeys;
  CurveData line;
  line.set(cdkeys.indexToString(KeyCurveData::_TYPE), Data(QString("Line")));
  
  Data td;
  Entity::toData(QString("Z"), td);
  line.set(cdkeys.indexToString(KeyCurveData::_Z), td);
  
  Entity::toData(QString("PEN"), td);
  line.set(cdkeys.indexToString(KeyCurveData::_PEN), td);
  
  Entity::toData(QString("LABEL"), td);
  line.set(cdkeys.indexToString(KeyCurveData::_LABEL), td);
  
  Entity::toData(QString("CHART"), td);
  line.set(cdkeys.indexToString(KeyCurveData::_CHART), td);
  
  Entity::toData(QString("STYLE"), td);
  line.set(cdkeys.indexToString(KeyCurveData::_STYLE), td);

  KeyCurveBar cbkeys;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Entity ibar;
    if (in.toIndex(keys.at(loop), ibar))
      continue;
    
    Data value;
    if (ibar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), value))
      continue;

    CurveBar bar;
    bar.set(cbkeys.indexToString(KeyCurveBar::_VALUE), value);
    
    Entity::toData(QString("COLOR"), td);
    bar.set(cbkeys.indexToString(KeyCurveBar::_COLOR), td);
    
    line.setEntity(keys.at(loop), bar);
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}
