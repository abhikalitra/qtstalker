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

#include "CommandBETA.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "Symbol.h"
#include "QuoteDataBase.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "DataDouble.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataSymbol.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandBETA::CommandBETA (QObject *p) : Command (p)
{
  _name = "BETA";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeInput, new DataString());

  QStringList tl;
  tl << "YAHOO:^GSP500";
  _values.insert(_ParmTypeIndex, new DataSymbol(tl));

  _values.insert(_ParmTypePeriod, new DataInteger(10));

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandBETA::CommandBETA: error on TA_Initialize");
}

void CommandBETA::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandBETA::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *in = svc.toCurve(script, _values.value(_ParmTypeInput)->toString(), toffset);
  if (! in)
  {
    Command::error("CommandBETA::runScript: invalid Input");
    return;
  }

  Data *in2 = getIndex(_values.value(_ParmTypeIndex)->toString(), script);
  if (! in2)
  {
    Command::error("CommandBETA::runScript: invalid Index");
    return;
  }
  Command::setTData(in2);

  QList<Data *> list;
  list << in << in2;

  Data *line = getBETA(list, _values.value(_ParmTypePeriod)->toInteger());
  if (! line)
  {
    Command::error("CommandBETA::runScript: getBETA error");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandBETA::getBETA (QList<Data *> &list, int period)
{
  if (list.count() != 2)
    return 0;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill2(list, keys, &input[0], &input2[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_BETA(0,
                          size - 1,
                          &input[0],
                          &input2[0],
                          period,
                          &outBeg,
                          &outNb,
                          &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getBETA: TA-Lib error" << rc;
    return 0;
  }

  QList<Data *> outs;
  Data *c = new CurveData;
  outs.append(c);

  TALibOutput tao;
  if (tao.fillDouble1(outs, keys, outNb, &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}

Data * CommandBETA::getIndex (QString d, Script *script)
{
  if (d.isEmpty())
    return 0;

  QStringList tl = d.split(":");
  if (tl.count() != 2)
    return 0;

  Symbol *symbol = script->symbol();
  if (! symbol)
  {
    qDebug() << "CommandBETA::getIndex: invalid symbol";
    return 0;
  }

  Symbol *bd = new Symbol;
  bd->setExchange(tl.at(0));
  bd->setSymbol(tl.at(1));
  bd->setLength(symbol->length());
  bd->setStartDate(QDateTime());
  bd->setEndDate(QDateTime());
  bd->setRange(symbol->range());

  // load quotes
  QuoteDataBase db;
  if (db.getBars(bd))
  {
    qDebug() << _type << "::getIndex: QuoteDataBase error";
    delete bd;
    return 0;
  }

  Data *line = new CurveData;

  int loop = 0;
  QList<int> barKeys = bd->barKeys();
  for (; loop < barKeys.count(); loop++)
  {
    Data *b = bd->getData(barKeys.at(loop));

    Data *cb = new CurveBar;
    cb->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_CLOSE)->toDouble()));
    line->set(loop, cb);
  }

  delete bd;

  return line;
}
