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

#include "CommandSymbol.h"
#include "Symbol.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "QuoteDataBase.h"
#include "BarLengthType.h"
#include "DateRangeType.h"
#include "Script.h"
#include "SymbolKey.h"
#include "CurveBarKey.h"

#include <QtDebug>

CommandSymbol::CommandSymbol ()
{
  _name = "SYMBOL";

  Data td;
  td.setLabel(QObject::tr("Symbol"));
  Entity::set(QString("SYMBOL"), td);
  
  BarLengthType bl;
  QStringList l = bl.list();
  td = Data(l, l.at(0));
  td.setLabel(QObject::tr("Bar Length"));
  Entity::set(QString("LENGTH"), td);
  
  DateRangeType dr;
  l = dr.list();
  td = Data(l, l.at(0));
  td.setLabel(QObject::tr("Date Range"));
  Entity::set(QString("RANGE"), td);
  
  td = Data(QString("date"));
  td.setLabel(QObject::tr("Date Input"));
  Entity::set(QString("DATE"), td);
  
  td = Data(QString("open"));
  td.setLabel(QObject::tr("Open Input"));
  Entity::set(QString("OPEN"), td);
  
  td = Data(QString("high"));
  td.setLabel(QObject::tr("High Input"));
  Entity::set(QString("HIGH"), td);
  
  td = Data(QString("low"));
  td.setLabel(QObject::tr("Low Input"));
  Entity::set(QString("LOW"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Close Input"));
  Entity::set(QString("CLOSE"), td);
  
  td = Data(QString("volume"));
  td.setLabel(QObject::tr("Volume Input"));
  Entity::set(QString("VOLUME"), td);
  
  td = Data(QString("oi"));
  td.setLabel(QObject::tr("OI Input"));
  Entity::set(QString("OI"), td);
}

QString CommandSymbol::run (CommandParse &, void *d)
{
  // verify SYMBOL
  Data tsymbol;
  Entity::toData(QString("SYMBOL"), tsymbol);
  QStringList tl = tsymbol.toString().split(":");
  if (tl.count() != 2)
  {
    qDebug() << "CommandSymbol::run: invalid Symbol" << tsymbol.toString();
    return _returnCode;
  }
  QString exchange = tl.at(0);
  QString symbol = tl.at(1);
  
  // load quotes
  SymbolKey symkeys;
  Symbol bd;
  bd.set(symkeys.indexToString(SymbolKey::_SYMBOL), Data(tsymbol.toString()));
  
  Data td;
  Entity::toData(QString("LENGTH"), td);
  bd.set(symkeys.indexToString(SymbolKey::_LENGTH), Data(td.toInteger()));
  
  bd.set(symkeys.indexToString(SymbolKey::_START_DATE), Data(QDateTime()));
  bd.set(symkeys.indexToString(SymbolKey::_END_DATE), Data(QDateTime()));
  
  Entity::toData(QString("RANGE"), td);
  bd.set(symkeys.indexToString(SymbolKey::_RANGE), Data(td.toInteger()));

  QuoteDataBase db;
  if (db.getBars(bd))
  {
    qDebug() << "CommandSymbol::run: QuoteDataBase error";
    return _returnCode;
  }
  
  Data date, open, high, low, close, volume, oi;
  Entity::toData(QString("DATE"), date);
  Entity::toData(QString("OPEN"), open);
  Entity::toData(QString("HIGH"), high);
  Entity::toData(QString("LOW"), low);
  Entity::toData(QString("CLOSE"), close);
  Entity::toData(QString("VOLUME"), volume);
  Entity::toData(QString("OI"), oi);
  
  CurveData dline;
  CurveData oline;
  CurveData hline;
  CurveData lline;
  CurveData cline;
  CurveData vline;
  CurveData iline;
  CurveBarKey cbkeys;
  int loop = 0;
  QList<QString> keys = bd.ekeys();
  for (; loop < keys.size(); loop++)
  {
    Entity b;
    bd.toEntity(keys.at(loop), b);
    
    // date
    Data td;
    b.toData(cbkeys.indexToString(CurveBarKey::_DATE), td);

    CurveBar db;
    db.set(cbkeys.indexToString(CurveBarKey::_DATE), td);
    dline.setEntity(keys.at(loop), db);

    // open
    b.toData(cbkeys.indexToString(CurveBarKey::_OPEN), td);
    db.set(cbkeys.indexToString(CurveBarKey::_VALUE), td);
    oline.setEntity(keys.at(loop), db);

    // high
    b.toData(cbkeys.indexToString(CurveBarKey::_HIGH), td);
    db.set(cbkeys.indexToString(CurveBarKey::_VALUE), td);
    hline.setEntity(keys.at(loop), db);

    // low
    b.toData(cbkeys.indexToString(CurveBarKey::_LOW), td);
    db.set(cbkeys.indexToString(CurveBarKey::_VALUE), td);
    lline.setEntity(keys.at(loop), db);

    // close
    b.toData(cbkeys.indexToString(CurveBarKey::_CLOSE), td);
    db.set(cbkeys.indexToString(CurveBarKey::_VALUE), td);
    cline.setEntity(keys.at(loop), db);

    // volume
    b.toData(cbkeys.indexToString(CurveBarKey::_VOLUME), td);
    db.set(cbkeys.indexToString(CurveBarKey::_VALUE), td);
    vline.setEntity(keys.at(loop), db);

    // oi
    b.toData(cbkeys.indexToString(CurveBarKey::_OI), td);
    db.set(cbkeys.indexToString(CurveBarKey::_VALUE), td);
    iline.setEntity(keys.at(loop), db);
  }

  Script *script = (Script *) d;
  script->setData(date.toString(), dline);
  script->setData(open.toString(), oline);
  script->setData(high.toString(), hline);
  script->setData(low.toString(), lline);
  script->setData(close.toString(), cline);
  script->setData(volume.toString(), vline);
  script->setData(oi.toString(), iline);
  
  _returnCode = "OK";
  return _returnCode;
}
