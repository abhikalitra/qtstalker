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

#include "CommandStochFast.h"
#include "ta_libc.h"
#include "MAType.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "SettingFactory.h"

#include <QtDebug>

CommandStochFast::CommandStochFast (QObject *p) : Command (p)
{
  _type = "STOCH_FAST";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandStochFast::CommandStochFast: error on TA_Initialize");
}

int CommandStochFast::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  QString s = sg->value("OUTPUT_FASTK");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_FASTK";
    return _ERROR;
  }
  Setting *kname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! kname)
  {
    _message << "invalid OUTPUT_FASTK " + s;
    return _ERROR;
  }

  s = sg->value("OUTPUT_FASTD");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_FASTD";
    return _ERROR;
  }
  Setting *dname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! dname)
  {
    _message << "invalid OUTPUT_FASTD " + s;
    return _ERROR;
  }

  s = sg->value("HIGH");
  Data *ihigh = vdi.curve(script, s);
  if (! ihigh)
  {
    _message << "invalid HIGH " + s;
    return _ERROR;
  }

  s = sg->value("LOW");
  Data *ilow = vdi.curve(script, s);
  if (! ilow)
  {
    _message << "invalid LOW " + s;
    return _ERROR;
  }

  s = sg->value("CLOSE");
  Data *iclose = vdi.curve(script, s);
  if (! iclose)
  {
    _message << "invalid CLOSE " + s;
    return _ERROR;
  }

  s = sg->value("PERIOD_FASTK");
  Setting *kperiod = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! kperiod)
  {
    _message << "invalid PERIOD_FASTK " + s;
    return _ERROR;
  }

  s = sg->value("PERIOD_FASTD");
  Setting *dperiod = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! dperiod)
  {
    _message << "invalid PERIOD_FASTD " + s;
    return _ERROR;
  }

  MAType mat;
  s = sg->value("MA_TYPE");
  int type = mat.fromString(s);
  if (type == -1)
  {
    _message << "invalid MA_TYPE " + s;
    return _ERROR;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose;

  QList<Data *> lines = getSTOCHF(list, kperiod->toInteger(), dperiod->toInteger(), type);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  script->setData(kname->toString(), lines.at(0));
  script->setData(dname->toString(), lines.at(1));

  return _OK;
}

QList<Data *> CommandStochFast::getSTOCHF (QList<Data *> &list, int kperiod, int dperiod, int type)
{
  QList<Data *> lines;
  if (! list.count())
    return lines;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill3(list, keys, &high[0], &low[0], &close[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_STOCHF(0,
                            size - 1,
                            &high[0],
                            &low[0],
                            &close[0],
                            kperiod,
                            dperiod,
                            (TA_MAType) type,
                            &outBeg,
                            &outNb,
                            &out[0],
                            &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getSTOCHF: TA-Lib error" << rc;
    return lines;
  }

  lines.clear();
  Data *c = new CurveData;
  lines << c;
  c = new CurveData;
  lines << c;

  TALibOutput tao;
  if (tao.fillDouble2(lines, keys, outNb, &out[0], &out2[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}
