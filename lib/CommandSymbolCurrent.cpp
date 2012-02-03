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

#include "CommandSymbolCurrent.h"
#include "Strip.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"
#include "GlobalSymbol.h"
#include "Script.h"

#include <QtDebug>

CommandSymbolCurrent::CommandSymbolCurrent ()
{
  _name = "SYMBOL_CURRENT";

  Entity::set(QString("DATE"), Data(QString("date")));
  Entity::set(QString("OPEN"), Data(QString("open")));
  Entity::set(QString("HIGH"), Data(QString("high")));
  Entity::set(QString("LOW"), Data(QString("low")));
  Entity::set(QString("CLOSE"), Data(QString("close")));
  Entity::set(QString("VOLUME"), Data(QString("volume")));
  Entity::set(QString("OI"), Data(QString("oi")));
}

QString CommandSymbolCurrent::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  g_currentSymbolMutex.lock();
  Symbol bd = g_currentSymbol;
  g_currentSymbolMutex.unlock();

  // date
  Data date, open, high, low, close, volume, oi;
  Entity::toData(QString("DATE"), date);
  Entity::toData(QString("OPEN"), open);
  Entity::toData(QString("HIGH"), high);
  Entity::toData(QString("LOW"), low);
  Entity::toData(QString("CLOSE"), close);
  Entity::toData(QString("VOLUME"), volume);
  Entity::toData(QString("OI"), oi);

  QList<int> keys;
  bd.ekeys(keys);
  
  CurveData dline, oline, hline, lline, cline, vline, iline;
  KeyCurveBar cbkeys;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity b;
    bd.toIndex(keys.at(loop), b);
    
    // date
    Data td;
    b.toData(cbkeys.indexToString(KeyCurveBar::_DATE), td);

    CurveBar db;
    db.set(cbkeys.indexToString(KeyCurveBar::_DATE), td);
    dline.setEntity(keys.at(loop), db);

    // open
    b.toData(cbkeys.indexToString(KeyCurveBar::_OPEN), td);
    db.set(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    oline.setEntity(keys.at(loop), db);

    // high
    b.toData(cbkeys.indexToString(KeyCurveBar::_HIGH), td);
    db.set(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    hline.setEntity(keys.at(loop), db);

    // low
    b.toData(cbkeys.indexToString(KeyCurveBar::_LOW), td);
    db.set(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    lline.setEntity(keys.at(loop), db);

    // close
    b.toData(cbkeys.indexToString(KeyCurveBar::_CLOSE), td);
    db.set(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    cline.setEntity(keys.at(loop), db);

    // volume
    b.toData(cbkeys.indexToString(KeyCurveBar::_VOLUME), td);
    db.set(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    vline.setEntity(keys.at(loop), db);

    // oi
    b.toData(cbkeys.indexToString(KeyCurveBar::_OI), td);
    db.set(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    iline.setEntity(keys.at(loop), db);
  }

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
