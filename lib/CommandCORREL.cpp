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

#include "CommandCORREL.h"
#include "Symbol.h"
#include "KeySymbol.h"
#include "QuoteDataBase.h"
#include "ScriptVerifyCurve.h"
#include "TALibFunction.h"
#include "TypeTALibFunction.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"
#include "GlobalSymbol.h"

#include <QtDebug>

CommandCORREL::CommandCORREL ()
{
  _name = "CORREL";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);

  td = Data(QString("YAHOO:^GSPC"));
  td.setLabel(QObject::tr("Index Symbol"));
  Entity::set(QString("INDEX"), td);

  td = Data(10);
  td.setLabel(QObject::tr("Period"));
  Entity::set(QString("PERIOD"), td);
}

QString CommandCORREL::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandCORREL::run: invalid INPUT" << td.toString();
    return _returnCode;
  }

  CurveData in2;
  Entity::toData(QString("INDEX"), td);
  if (getIndex(td.toString(), in2))
  {
    qDebug() << "CommandCORREL::run: invalid Index";
    return _returnCode;
  }

  Data period;
  Entity::toData(QString("PERIOD"), period);
  
  Entity parms;
  parms.set(QString("FUNCTION"), Data(TypeTALibFunction::_CORREL));
  parms.set(QString("PERIOD"), period);
  
  CurveData line;
  TALibFunction func;
  if (func.run(parms, 2, in, in2, in2, in2, 1, line, line, line))
  {
    qDebug() << "CommandCORREL::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}

int CommandCORREL::getIndex (QString d, Entity &line)
{
  QStringList tl = d.split(":");
  if (tl.count() != 2)
  {
    qDebug() << "CommandCORREL::getIndex: invalid index symbol" << d;
    return 1;
  }

  KeySymbol skeys;
  Symbol bd;
  bd.set(skeys.indexToString(KeySymbol::_SYMBOL), Data(d));
  
  g_currentSymbolMutex.lock();
  Data td;
  g_currentSymbol.toData(skeys.indexToString(KeySymbol::_LENGTH), td);
  bd.set(skeys.indexToString(KeySymbol::_LENGTH), td);
  
  g_currentSymbol.toData(skeys.indexToString(KeySymbol::_RANGE), td);
  bd.set(skeys.indexToString(KeySymbol::_RANGE), td);
  g_currentSymbolMutex.unlock();

  bd.set(skeys.indexToString(KeySymbol::_START_DATE), Data(QDateTime()));
  bd.set(skeys.indexToString(KeySymbol::_END_DATE), Data(QDateTime()));

  // load quotes
  QuoteDataBase db;
  if (db.getBars(bd))
  {
    qDebug() << "CommandCORREL::getIndex: QuoteDataBase error";
    return 1;
  }

  QList<int> barKeys;
  bd.ekeys(barKeys);
  
  KeyCurveBar cbkeys;
  int loop = 0;
  for (; loop < barKeys.size(); loop++)
  {
    Entity b;
    bd.toIndex(barKeys.at(loop), b);
    
    Data close;
    if (b.toData(cbkeys.indexToString(KeyCurveBar::_CLOSE), close))
      continue;

    CurveBar cb;
    cb.set(cbkeys.indexToString(KeyCurveBar::_VALUE), close);
    line.setEntity(barKeys.at(loop), cb);
  }

  return 0;
}
