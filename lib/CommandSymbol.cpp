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
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingDouble.h"
#include "SettingDateTime.h"
#include "VerifyDataInput.h"
#include "SettingFactory.h"
#include "QuoteDataBase.h"
#include "DateRange.h"
#include "BarLength.h"

#include <QtDebug>
#include <QSharedMemory>

CommandSymbol::CommandSymbol (QObject *p) : Command (p)
{
  _name = "SYMBOL";
}

int CommandSymbol::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // SYMBOL
  QString s = sg->value("SYMBOL");
  Setting *set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid SYMBOL " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QStringList tl = set->toString().split(":");
  if (tl.count() != 2)
  {
    _message << "invalid SYMBOL " + set->toString();
    emit signalResume((void *) this);
    return _ERROR;
  }

  QString exchange = tl.at(0);
  QString symbol = tl.at(1);

  // LENGTH
  s = sg->value("LENGTH");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid LENGTH " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  BarLength bl;
  int length = bl.stringToType(set->toString());
  if (length == -1)
  {
    _message << "invalid LENGTH " + set->toString();
    emit signalResume((void *) this);
    return _ERROR;
  }

  // RANGE
  s = sg->value("RANGE");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid RANGE " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  DateRange dr;
  int range = dr.toType(set->toString());
  if (range == -1)
  {
    _message << "invalid RANGE " + set->toString();
    emit signalResume((void *) this);
    return _ERROR;
  }

  Data *bd = new Symbol;
  bd->set(Symbol::_EXCHANGE, new SettingString(exchange));
  bd->set(Symbol::_SYMBOL, new SettingString(symbol));
  bd->set(Symbol::_LENGTH, new SettingInteger(length));
  bd->set(Symbol::_START_DATE, new SettingDateTime(QDateTime()));
  bd->set(Symbol::_END_DATE, new SettingDateTime(QDateTime()));
  bd->set(Symbol::_RANGE, new SettingInteger(range));

  // load quotes
  QuoteDataBase db;
  if (db.getBars(bd))
  {
    _message << "QuoteDataBase error" << "EXCHANGE=" + exchange << "SYMBOL=" + symbol;
    _message << "LENGTH=" + QString::number(length) << "RANGE=" + QString::number(range);
    delete bd;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // date
  s = sg->value("DATE");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid DATE " + s;
    delete bd;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *dline = new CurveData;
  script->setData(set->toString(), dline);

  // open
  s = sg->value("OPEN");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid OPEN " + s;
    delete bd;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *oline = new CurveData;
  script->setData(set->toString(), oline);

  // high
  s = sg->value("HIGH");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid HIGH " + s;
    delete bd;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *hline = new CurveData;
  script->setData(set->toString(), hline);

  // low
  s = sg->value("LOW");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid LOW " + s;
    delete bd;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *lline = new CurveData;
  script->setData(set->toString(), lline);

  // close
  s = sg->value("CLOSE");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid CLOSE " + s;
    delete bd;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *cline = new CurveData;
  script->setData(set->toString(), cline);

  // volume
  s = sg->value("VOLUME");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid VOLUME " + s;
    delete bd;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *vline = new CurveData;
  script->setData(set->toString(), vline);

  // oi
  s = sg->value("OI");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid OI " + s;
    delete bd;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *iline = new CurveData;
  script->setData(set->toString(), iline);

  int loop = 0;
  QList<int> barKeys = bd->barKeys();
  for (; loop < barKeys.count(); loop++)
  {
    Data *b = bd->getData(barKeys.at(loop));

    Data *db = new CurveBar;
    db->set(CurveBar::_DATE, new SettingDateTime(b->get(CurveBar::_DATE)->toDateTime()));
    dline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_OPEN)->toDouble()));
    oline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_HIGH)->toDouble()));
    hline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_LOW)->toDouble()));
    lline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_CLOSE)->toDouble()));
    cline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_VOLUME)->toDouble()));
    vline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_OI)->toDouble()));
    iline->set(loop, db);
  }

  delete bd;

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
