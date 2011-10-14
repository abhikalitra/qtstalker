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

#include "CommandBBANDS.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"

#include <QtDebug>

CommandBBANDS::CommandBBANDS (QObject *p) : Command (p)
{
  _name = "BBANDS";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandBBANDS::CommandBBANDS: error on TA_Initialize");
}

int CommandBBANDS::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT_UPPER
  QString uname;
  QString s = sg->value("OUTPUT_UPPER");
  if (vdi.toString(script, s, uname))
  {
    qDebug() << "CommandBBANDS::runScript: invalid OUTPUT_UPPER" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // OUTPUT_MIDDLE
  QString mname;
  s = sg->value("OUTPUT_MIDDLE");
  if (vdi.toString(script, s, mname))
  {
    qDebug() << "CommandBBANDS::runScript: invalid OUTPUT_MIDDLE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // OUTPUT_LOWER
  QString lname;
  s = sg->value("OUTPUT_LOWER");
  if (vdi.toString(script, s, lname))
  {
    qDebug() << "CommandBBANDS::runScript: invalid OUTPUT_LOWER" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INPUT
  s = sg->value("INPUT");
  Data *in = vdi.toCurve(script, s);
  if (! in)
  {
    qDebug() << "CommandBBANDS::runScript: invalid INPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // PERIOD
  int period = 20;
  s = sg->value("PERIOD");
  if (vdi.toInteger(script, s, period))
  {
    qDebug() << "CommandBBANDS::runScript: invalid PERIOD, using default" << s;
    period = 20;
  }

  // MA_TYPE
  int type = 0;
  s = sg->value("MA_TYPE");
  if (vdi.toMA(script, s, type))
  {
    qDebug() << "CommandBBANDS::runScript: invalid MA_TYPE, using default" << s;
    type = 0;
  }

  // DEV_UP
  double udev = 2;
  s = sg->value("DEV_UP");
  if (vdi.toDouble(script, s, udev))
  {
    qDebug() << "CommandBBANDS::runScript: invalid DEV_UP, using default" << s;
    udev = 2;
  }

  // DEV_DOWN
  double ddev = 2;
  s = sg->value("DEV_DOWN");
  if (vdi.toDouble(script, s, ddev))
  {
    qDebug() << "CommandBBANDS::runScript: invalid DEV_DOWN, using default" << s;
    udev = 2;
  }

  QList<Data *> list;
  list << in;

  QList<Data *> lines = getBBANDS(list, period, udev, ddev, type);
  if (lines.count() != 3)
  {
    qDeleteAll(lines);
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(uname, lines.at(0));
  script->setData(mname, lines.at(1));
  script->setData(lname, lines.at(2));

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

QList<Data *> CommandBBANDS::getBBANDS (QList<Data *> &list, int period, double udev, double ldev, int type)
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

  TA_RetCode rc = TA_BBANDS(0,
                            size - 1,
                            &in[0],
                            period,
                            udev,
                            ldev,
                            (TA_MAType) type,
                            &outBeg,
                            &outNb,
                            &out[0],
                            &out2[0],
			    &out3[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getBBANDS: TA-Lib error" << rc;
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
