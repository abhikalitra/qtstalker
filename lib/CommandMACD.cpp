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

#include "CommandMACD.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"

#include <QtDebug>

CommandMACD::CommandMACD (QObject *p) : Command (p)
{
  _name = "MACD";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandMACD::CommandMACD: error on TA_Initialize");
}

int CommandMACD::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT_MACD
  QString mname;
  QString s = sg->value("OUTPUT_MACD");
  if (vdi.toString(script, s, mname))
  {
    qDebug() << "CommandMACD::runScript: invalid OUTPUT_UPPER" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // OUTPUT_SIGNAL
  QString sname;
  s = sg->value("OUTPUT_SIGNAL");
  if (vdi.toString(script, s, sname))
  {
    qDebug() << "CommandMACD::runScript: invalid OUTPUT_MIDDLE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // OUTPUT_HIST
  QString hname;
  s = sg->value("OUTPUT_HIST");
  if (vdi.toString(script, s, hname))
  {
    qDebug() << "CommandMACD::runScript: invalid OUTPUT_LOWER" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INPUT
  s = sg->value("INPUT");
  Data *in = vdi.toCurve(script, s);
  if (! in)
  {
    qDebug() << "CommandMACD::runScript: invalid INPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // PERIOD_FAST
  int fperiod = 12;
  s = sg->value("PERIOD_FAST");
  if (vdi.toInteger(script, s, fperiod))
  {
    qDebug() << "CommandMACD::runScript: invalid PERIOD_FAST, using default" << s;
    fperiod = 12;
  }

  // MA_TYPE_FAST
  int ftype = 0;
  s = sg->value("MA_TYPE_FAST");
  if (vdi.toMA(script, s, ftype))
  {
    qDebug() << "CommandMACD::runScript: invalid MA_TYPE_FAST, using default" << s;
    ftype = 0;
  }

  // PERIOD_SLOW
  int speriod = 24;
  s = sg->value("PERIOD_SLOW");
  if (vdi.toInteger(script, s, speriod))
  {
    qDebug() << "CommandMACD::runScript: invalid PERIOD_SLOW, using default" << s;
    speriod = 24;
  }

  // MA_TYPE_SLOW
  int stype = 0;
  s = sg->value("MA_TYPE_SLOW");
  if (vdi.toMA(script, s, stype))
  {
    qDebug() << "CommandMACD::runScript: invalid MA_TYPE_SLOW, using default" << s;
    stype = 0;
  }

  // PERIOD_SIGNAL
  int sigperiod = 9;
  s = sg->value("PERIOD_SIGNAL");
  if (vdi.toInteger(script, s, sigperiod))
  {
    qDebug() << "CommandMACD::runScript: invalid PERIOD_SIGNAL, using default" << s;
    sigperiod = 9;
  }

  // MA_TYPE_SIGNAL
  int sigtype = 0;
  s = sg->value("MA_TYPE_SIGNAL");
  if (vdi.toMA(script, s, sigtype))
  {
    qDebug() << "CommandMACD::runScript: invalid MA_TYPE_SIGNAL, using default" << s;
    sigtype = 0;
  }

  QList<Data *> list;
  list << in;

  QList<Data *> lines = getMACD(list, fperiod, speriod, sigperiod, ftype, stype, sigtype);
  if (lines.count() != 3)
  {
    qDeleteAll(lines);
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(mname, lines.at(0));
  script->setData(sname, lines.at(1));
  script->setData(hname, lines.at(2));

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

QList<Data *> CommandMACD::getMACD (QList<Data *> &list, int fp, int sp, int sigp, int fma, int sma, int sigma)
{
  QList<Data *> lines;
  if (! list.count())
    return lines;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real in[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill1(list, keys, &in[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_MACDEXT(0,
                             size - 1,
                             &in[0],
                             fp,
                             (TA_MAType) fma,
                             sp,
                             (TA_MAType) sma,
                             sigp,
                             (TA_MAType) sigma,
                             &outBeg,
                             &outNb,
                             &out[0],
                             &out2[0],
                             &out3[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getMACD: TA-Lib error" << rc;
    return lines;
  }

  lines.clear();

  Data *c = new CurveData;
  lines << c;
  c = new CurveData;
  lines << c;
  c = new CurveData;
  lines << c;

  TALibOutput tao;
  if (tao.fillDouble3(lines, keys, outNb, &out[0], &out2[0], &out3[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}
