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
    qDebug() << "CommandSymbol::runScript: invalid SYMBOL" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QStringList tl = set->toString().split(":");
  if (tl.count() != 2)
  {
    qDebug() << "CommandSymbol::runScript: invalid SYMBOL" << set->toString();
    emit signalResume((void *) this);
    return _ERROR;
  }

  QString exchange = tl.at(0);
  QString symbol = tl.at(1);

  // LENGTH
  s = sg->value("LENGTH");
  Setting *length = vdi.setting(SettingFactory::_BAR_LENGTH, script, s);
  if (! length)
  {
    qDebug() << "CommandSymbol::runScript: invalid LENGTH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // RANGE
  s = sg->value("RANGE");
  Setting *range = vdi.setting(SettingFactory::_DATE_RANGE, script, s);
  if (! range)
  {
    qDebug() << "CommandSymbol::runScript: invalid RANGE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  Symbol bd;
  bd.setExchange(exchange);
  bd.setSymbol(symbol);
  bd.setLength(length->toInteger());
  bd.setStartDate(QDateTime());
  bd.setEndDate(QDateTime());
  bd.setRange(range->toInteger());

  // load quotes
  QuoteDataBase db;
  if (db.getBars(&bd))
  {
    qDebug() << "CommandSymbol::runScript: QuoteDataBase error" << "EXCHANGE=" << exchange << "SYMBOL=" << symbol;
    qDebug() << "CommandSymbol::runScript: LENGTH=" << length->toString() << "RANGE=" << range->toString();
    emit signalResume((void *) this);
    return _ERROR;
  }

  // date
  s = sg->value("DATE");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    qDebug() << "CommandSymbol::runScript: invalid DATE" << s;
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
    qDebug() << "CommandSymbol::runScript: invalid OPEN" << s;
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
    qDebug() << "CommandSymbol::runScript: invalid HIGH" << s;
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
    qDebug() << "CommandSymbol::runScript: invalid LOW" << s;
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
    qDebug() << "CommandSymbol::runScript: invalid CLOSE" << s;
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
    qDebug() << "CommandSymbol::runScript: invalid VOLUME" << s;
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
    qDebug() << "CommandSymbol::runScript: invalid OI" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *iline = new CurveData;
  script->setData(set->toString(), iline);

  int loop = 0;
  QList<int> barKeys = bd.barKeys();
  for (; loop < barKeys.count(); loop++)
  {
    Data *b = bd.getData(barKeys.at(loop));

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

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
