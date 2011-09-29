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
#include "MAType.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "SettingFactory.h"

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
  QString s = sg->value("OUTPUT_UPPER");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_UPPER";
    emit signalResume((void *) this);
    return _ERROR;
  }
  Setting *uname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! uname)
  {
    _message << "invalid OUTPUT_UPPER " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("OUTPUT_MIDDLE");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_MIDDLE";
    emit signalResume((void *) this);
    return _ERROR;
  }
  Setting *mname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! mname)
  {
    _message << "invalid OUTPUT_MIDDLE " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("OUTPUT_LOWER");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_LOWER";
    emit signalResume((void *) this);
    return _ERROR;
  }
  Setting *lname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! lname)
  {
    _message << "invalid OUTPUT_LOWER " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("INPUT");
  Data *in = vdi.curve(script, s);
  if (! in)
  {
    _message << "INPUT missing " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("PERIOD");
  Setting *period = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! period)
  {
    _message << "invalid PERIOD " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  MAType types;
  s = sg->value("MA_TYPE");
  int type = types.fromString(s);
  if (type == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("DEV_UP");
  Setting *udev = vdi.setting(SettingFactory::_DOUBLE, script, s);
  if (! udev)
  {
    _message << "invalid DEV_UP " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("DEV_DOWN");
  Setting *ldev = vdi.setting(SettingFactory::_DOUBLE, script, s);
  if (! ldev)
  {
    _message << "invalid DEV_DOWN " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QList<Data *> list;
  list << in;

  QList<Data *> lines = getBBANDS(list, period->toInteger(), udev->toDouble(), ldev->toDouble(), type);
  if (lines.count() != 3)
  {
    qDeleteAll(lines);
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(uname->toString(), lines.at(0));
  script->setData(mname->toString(), lines.at(1));
  script->setData(lname->toString(), lines.at(2));

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
