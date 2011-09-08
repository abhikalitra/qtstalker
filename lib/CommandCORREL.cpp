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
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "Symbol.h"
#include "QuoteDataBase.h"

#include <QtDebug>
#include <QSettings>

CommandCORREL::CommandCORREL (QObject *p) : Command (p)
{
  _type = "CORREL";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandCORREL::CommandCORREL: error on TA_Initialize");
}

int CommandCORREL::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT").toString();
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("INPUT").toString();
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT missing" << s;
    return _ERROR;
  }

  s = sg->get("INDEX").toString();
  Data *in2 = getIndex(s, script);
  if (! in2)
  {
    qDebug() << _type << "::runScript: INDEX missing" << s;
    return _ERROR;
  }

  int period = sg->get("PERIOD").toInt();

  QList<Data *> list;
  list << in << in2;
  line = getCORREL(list, period);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  delete in2;

  return _OK;
}

Data * CommandCORREL::getCORREL (QList<Data *> &list, int period)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &input[0], &input2[0], &input2[0], &input2[0]);
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
  if (it.outputs(outs, keys, outNb, &out[0], &out[0], &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}

Data * CommandCORREL::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QVariant(QString()));
  sg->set("INPUT", QVariant(QString()));
  sg->set("INDEX", QVariant(QString()));
  sg->set("PERIOD", QVariant(30));
  return sg;
}

Data * CommandCORREL::getIndex (QString d, Script *script)
{
  if (d.isEmpty())
    return 0;

  QStringList tl = d.split(":");
  if (tl.count() != 2)
    return 0;

  QSettings settings("QtStalker/qtstalkerrc" + script->session());

  Data *bd = new Symbol;
  bd->set(Symbol::_EXCHANGE, QVariant(tl.at(0)));
  bd->set(Symbol::_SYMBOL, QVariant(tl.at(1)));
  bd->set(Symbol::_LENGTH, QVariant(settings.value("bar_length").toInt()));
  bd->set(Symbol::_START_DATE, QVariant(QDateTime()));
  bd->set(Symbol::_END_DATE, QVariant(QDateTime()));
  bd->set(Symbol::_RANGE, QVariant(settings.value("date_range").toInt()));

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

    CurveBar *cb = new CurveBar;
    cb->set(CurveBar::_VALUE, b->get(CurveBar::_CLOSE));
    line->set(loop, cb);
  }

  delete bd;

  return line;
}
