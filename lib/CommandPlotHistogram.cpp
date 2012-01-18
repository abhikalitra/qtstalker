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

#include "CommandPlotHistogram.h"
#include "CurveData.h"
#include "CurveDataKey.h"
#include "CurveBarKey.h"
#include "HistogramStyle.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"
#include "ScriptVerifyCurveValue.h"
#include "Script.h"

#include <QtDebug>

CommandPlotHistogram::CommandPlotHistogram ()
{
  _name = "PLOT_HISTOGRAM";

  Entity::set(QString("OUTPUT"), Data(QString("histogram")));
  Entity::set(QString("CHART"), Data(QString("chart")));
  Entity::set(QString("LABEL"), Data(QString("histogram")));
  Entity::set(QString("COLOR"), Data(QColor(Qt::red)));
  Entity::set(QString("Z"), Data(0));
  Entity::set(QString("PEN"), Data(1));
  Entity::set(QString("HIGH"), Data(QString("high")));
  Entity::set(QString("LOW"), Data(QString("low")));
  
  HistogramStyle st;
  Data td(st.list(), QString("Histogram Bar"));
  Entity::set(QString("STYLE"), td);
}

QString CommandPlotHistogram::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("HIGH"), td);

  ScriptVerifyCurve svc;
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    if (svc.entity(td.toString(), ihigh))
    {
      qDebug() << "CommandPlotHistogram::run: invalid High";
      return _returnCode;
    }
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    if (svc.entity(td.toString(), ilow))
    {
      qDebug() << "CommandPlotHistogram::run: invalid Low";
      return _returnCode;
    }
  }

  // keys
  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys2(ihigh, ilow, keys))
  {
    qDebug() << "CommandPlotHistogram::run: invalid keys";
    return _returnCode;
  }
  
  CurveDataKey cdkeys;
  CurveData line;
  line.set(cdkeys.indexToString(CurveDataKey::_TYPE), Data(QString("Histogram")));
  
  Entity::toData(QString("Z"), td);
  line.set(cdkeys.indexToString(CurveDataKey::_Z), td);
  
  Entity::toData(QString("PEN"), td);
  line.set(cdkeys.indexToString(CurveDataKey::_PEN), td);
  
  Entity::toData(QString("LABEL"), td);
  line.set(cdkeys.indexToString(CurveDataKey::_LABEL), td);
  
  Entity::toData(QString("CHART"), td);
  line.set(cdkeys.indexToString(CurveDataKey::_CHART), td);
  
  Entity::toData(QString("STYLE"), td);
  line.set(cdkeys.indexToString(CurveDataKey::_STYLE), td);

  Data color;
  Entity::toData(QString("COLOR"), color);
  
  CurveBarKey cbkeys;
  ScriptVerifyCurveValue svcv;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    double v = 0;
    if (svcv.getValue(ihigh, keys, loop, 0, v))
      continue;

    double v2 = 0;
    if (svcv.getValue(ilow, keys, loop, 0, v2))
      continue;

    Entity bar;
    bar.set(cbkeys.indexToString(CurveBarKey::_HIGH), Data(v));
    bar.set(cbkeys.indexToString(CurveBarKey::_LOW), Data(v2));
    bar.set(cbkeys.indexToString(CurveBarKey::_COLOR), color);
    
    line.setEntity(keys.at(loop), bar);
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}
