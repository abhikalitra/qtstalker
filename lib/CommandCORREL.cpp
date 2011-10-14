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
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "Symbol.h"
#include "QuoteDataBase.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "DataDouble.h"

#include <QtDebug>

CommandCORREL::CommandCORREL (QObject *p) : Command (p)
{
  _name = "CORREL";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandCORREL::CommandCORREL: error on TA_Initialize");
}

int CommandCORREL::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandCORREL::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INPUT
  s = sg->value("INPUT");
  Data *in = vdi.toCurve(script, s);
  if (! in)
  {
    qDebug() << "CommandCORREL::runScript: invalid INPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INDEX
  QString index;
  s = sg->value("INDEX");
  if (vdi.toString(script, s, index))
  {
    qDebug() << "CommandCORREL::runScript: invalid INDEX" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  Data *in2 = getIndex(index, script);
  if (! in2)
  {
    qDebug() << "CommandCORREL::runScript: invalid INDEX " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  script->setTData(in2);

  // PERIOD
  int period = 10;
  s = sg->value("PERIOD");
  if (vdi.toInteger(script, s, period))
  {
    qDebug() << "CommandCORREL::runScript: invalid PERIOD, using default" << s;
    period = 10;
  }

  QList<Data *> list;
  list << in << in2;

  Data *line = getCORREL(list, period);
  if (! line)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(name, line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

Data * CommandCORREL::getCORREL (QList<Data *> &list, int period)
{
  if (list.count() != 2)
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
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

  TA_RetCode rc = TA_CORREL(0,
                            size - 1,
                            &input[0],
                            &input2[0],
                            period,
                            &outBeg,
                            &outNb,
                            &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getCORREL: TA-Lib error" << rc;
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

Data * CommandCORREL::getIndex (QString d, Script *script)
{
  if (d.isEmpty())
    return 0;

  QStringList tl = d.split(":");
  if (tl.count() != 2)
    return 0;

  Symbol *symbol = script->symbol();
  if (! symbol)
  {
    qDebug() << "CommandCORREL::getIndex: invalid symbol";
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
    _message << "QuoteDataBase error";
    delete bd;
    return 0;
  }

  Data *line = new CurveData;

  int loop = 0;
  QList<int> barKeys = bd->barKeys();
  for (; loop < barKeys.count(); loop++)
  {
    Data *b = bd->getData(barKeys.at(loop));

    CurveBar *cb = new CurveBar;
    cb->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_CLOSE)->toDouble()));
    line->set(loop, cb);
  }

  delete bd;

  return line;
}
