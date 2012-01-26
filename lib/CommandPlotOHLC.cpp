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

#include "CommandPlotOHLC.h"
#include "TypeOHLC.h"
#include "CurveData.h"
#include "KeyCurveData.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"
#include "ScriptVerifyCurveKeys.h"
#include "Script.h"
#include "ScriptVerifyCurve.h"

#include <QtDebug>

CommandPlotOHLC::CommandPlotOHLC ()
{
  _name = "PLOT_OHLC";

  Entity::set(QString("OUTPUT"), Data(QString("ohlc")));
  Entity::set(QString("CHART"), Data(QString("OHLC")));
  Entity::set(QString("LABEL"), Data(QString("OHLC")));
  Entity::set(QString("COLOR"), Data(QColor(Qt::red)));
  Entity::set(QString("Z"), Data(0));
  Entity::set(QString("PEN"), Data(1));
  Entity::set(QString("OPEN"), Data(QString("open")));
  Entity::set(QString("HIGH"), Data(QString("high")));
  Entity::set(QString("LOW"), Data(QString("low")));
  Entity::set(QString("CLOSE"), Data(QString("close")));

  TypeOHLC st;
  Data td(st.list(), QString("OHLC"));
  Entity::set(QString("STYLE"), td);
}

QString CommandPlotOHLC::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  ScriptVerifyCurve svc;
  Data td;
  Entity::toData(QString("OPEN"), td);
  Entity iopen;
  if (svc.curve(script, td.toString(), iopen))
  {
    qDebug() << "CommandPlotOHLC::run invalid OPEN";
    return _returnCode;
  }

  Entity::toData(QString("HIGH"), td);
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandPlotOHLC::run invalid HIGH";
    return _returnCode;
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandPlotOHLC::run invalid LOW";
    return _returnCode;
  }

  Entity::toData(QString("CLOSE"), td);
  Entity iclose;
  if (svc.curve(script, td.toString(), iclose))
  {
    qDebug() << "CommandPlotOHLC::run invalid CLOSE";
    return _returnCode;
  }

  // keys
  QList<int> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys4(iopen, ihigh, ilow, iclose, keys))
  {
    qDebug() << "CommandPlotOHLC::run invalid keys";
    return _returnCode;
  }

  KeyCurveData cdkeys;
  CurveData line;
  line.set(cdkeys.indexToString(KeyCurveData::_TYPE), Data(QString("OHLC")));
  
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

  Data color;
  Entity::toData(QString("COLOR"), color);
  
  KeyCurveBar cbkeys;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity obar;
    if (iopen.toIndex(keys.at(loop), obar))
      continue;
    Data ovalue;
    if (obar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), ovalue))
      continue;
    
    Entity hbar;
    if (ihigh.toIndex(keys.at(loop), hbar))
      continue;
    Data hvalue;
    if (hbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), hvalue))
      continue;

    Entity lbar;
    if (ilow.toIndex(keys.at(loop), lbar))
      continue;
    Data lvalue;
    if (lbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), lvalue))
      continue;

    Entity cbar;
    if (iclose.toIndex(keys.at(loop), cbar))
      continue;
    Data cvalue;
    if (cbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), cvalue))
      continue;

    Entity bar;
    bar.set(cbkeys.indexToString(KeyCurveBar::_OPEN), ovalue);
    bar.set(cbkeys.indexToString(KeyCurveBar::_HIGH), hvalue);
    bar.set(cbkeys.indexToString(KeyCurveBar::_LOW), lvalue);
    bar.set(cbkeys.indexToString(KeyCurveBar::_CLOSE), cvalue);
    bar.set(cbkeys.indexToString(KeyCurveBar::_COLOR), color);
    
    line.setEntity(keys.at(loop), bar);
  }
  
  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}
