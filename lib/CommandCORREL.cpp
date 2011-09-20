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
#include "SettingFactory.h"
#include "SettingDouble.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingDateTime.h"

#include <QtDebug>
#include <QSettings>

CommandCORREL::CommandCORREL (QObject *p) : Command (p)
{
  _type = "CORREL";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandCORREL::CommandCORREL: error on TA_Initialize");
}

int CommandCORREL::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  QString s = sg->value("OUTPUT");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT";
    return _ERROR;
  }
  Setting *name = vdi.setting(SettingFactory::_STRING, script, s);
  if (! name)
  {
    _message << "invalid OUTPUT " + s;
    return _ERROR;
  }

  s = sg->value("INPUT");
  Data *in = vdi.curve(script, s);
  if (! in)
  {
    _message << "INPUT missing " + s;
    return _ERROR;
  }

  s = sg->value("INDEX");
  Data *in2 = getIndex(s, script);
  if (! in2)
  {
    _message << "invalid INDEX " + s;
    return _ERROR;
  }
  script->setTData(in2);

  s = sg->value("PERIOD");
  Setting *period = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! period)
  {
    _message << "invalid PERIOD " + s;
    return _ERROR;
  }

  QList<Data *> list;
  list << in << in2;

  Data *line = getCORREL(list, period->toInteger());
  if (! line)
    return _ERROR;

  script->setData(name->toString(), line);

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

  QSettings settings("QtStalker/qtstalkerrc" + script->session());

  Data *bd = new Symbol;
  bd->set(Symbol::_EXCHANGE, new SettingString(tl.at(0)));
  bd->set(Symbol::_SYMBOL, new SettingString(tl.at(1)));
  bd->set(Symbol::_LENGTH, new SettingInteger(settings.value("bar_length").toInt()));
  bd->set(Symbol::_START_DATE, new SettingDateTime(QDateTime()));
  bd->set(Symbol::_END_DATE, new SettingDateTime(QDateTime()));
  bd->set(Symbol::_RANGE, new SettingInteger(settings.value("date_range").toInt()));

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
    cb->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_CLOSE)->toDouble()));
    line->set(loop, cb);
  }

  delete bd;

  return line;
}
