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
#include "MAType.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "SettingFactory.h"

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
  QString s = sg->value("OUTPUT_MACD");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_MACD";
    emit signalResume((void *) this);
    return _ERROR;
  }
  Setting *mname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! mname)
  {
    _message << "invalid OUTPUT_MACD " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("OUTPUT_SIGNAL");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_SIGNAL";
    emit signalResume((void *) this);
    return _ERROR;
  }
  Setting *sname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! sname)
  {
    _message << "invalid OUTPUT_SIGNAL " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("OUTPUT_HIST");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_HIST";
    emit signalResume((void *) this);
    return _ERROR;
  }
  Setting *hname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! hname)
  {
    _message << "invalid OUTPUT_HIST " + s;
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

  s = sg->value("PERIOD_FAST");
  Setting *fperiod = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! fperiod)
  {
    _message << "invalid PERIOD_FAST " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  MAType mat;
  s = sg->value("MA_TYPE_FAST");
  int ftype = mat.fromString(s);
  if (ftype == -1)
  {
    _message << "invalid MA_TYPE_FAST " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("PERIOD_SLOW");
  Setting *speriod = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! speriod)
  {
    _message << "invalid PERIOD_SLOW " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("MA_TYPE_SLOW");
  int stype = mat.fromString(s);
  if (stype == -1)
  {
    _message << "invalid MA_TYPE_SLOW " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("PERIOD_SIGNAL");
  Setting *sigperiod = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! sigperiod)
  {
    _message << "invalid PERIOD_SIGNAL " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("MA_TYPE_SIGNAL");
  int sigtype = mat.fromString(s);
  if (sigtype == -1)
  {
    _message << "invalid MA_TYPE_SIGNAL " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QList<Data *> list;
  list << in;

  QList<Data *> lines = getMACD(list, fperiod->toInteger(), speriod->toInteger(), sigperiod->toInteger(), ftype, stype, sigtype);
  if (lines.count() != 3)
  {
    qDeleteAll(lines);
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(mname->toString(), lines.at(0));
  script->setData(sname->toString(), lines.at(1));
  script->setData(hname->toString(), lines.at(2));

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
