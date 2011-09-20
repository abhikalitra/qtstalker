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

#include "CommandStochSlow.h"
#include "ta_libc.h"
#include "MAType.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "SettingFactory.h"

#include <QtDebug>

CommandStochSlow::CommandStochSlow (QObject *p) : Command (p)
{
  _type = "STOCH_SLOW";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandStochSlow::CommandStochSlow: error on TA_Initialize");
}

int CommandStochSlow::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  QString s = sg->value("OUTPUT_SLOWK");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_SLOWK";
    return _ERROR;
  }
  Setting *kname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! kname)
  {
    _message << "invalid OUTPUT_SLOWK " + s;
    return _ERROR;
  }

  s = sg->value("OUTPUT_SLOWD");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_SLOWD";
    return _ERROR;
  }
  Setting *dname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! dname)
  {
    _message << "invalid OUTPUT_SLOWD " + s;
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
  Setting *fkperiod = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! fkperiod)
  {
    _message << "invalid PERIOD_FATSK " + s;
    return _ERROR;
  }

  s = sg->value("PERIOD_SLOWK");
  Setting *skperiod = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! skperiod)
  {
    _message << "invalid PERIOD_SLOWK " + s;
    return _ERROR;
  }

  MAType mat;
  s = sg->value("MA_TYPE_SLOWK");
  int kma = mat.fromString(s);
  if (kma == -1)
  {
    _message << "invalid MA_TYPE_SLOWK " + s;
    return _ERROR;
  }

  s = sg->value("PERIOD_SLOWD");
  Setting *sdperiod = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! sdperiod)
  {
    _message << "invalid PERIOD_SLOWD " + s;
    return _ERROR;
  }

  s = sg->value("MA_TYPE_SLOWD");
  int dma = mat.fromString(s);
  if (dma == -1)
  {
    _message << "invalid MA_TYPE_SLOWD " + s;
    return _ERROR;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose;

  QList<Data *> lines = getSTOCHS(list, fkperiod->toInteger(), skperiod->toInteger(), sdperiod->toInteger(), kma, dma);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  script->setData(kname->toString(), lines.at(0));
  script->setData(dname->toString(), lines.at(1));

  return _OK;
}

QList<Data *> CommandStochSlow::getSTOCHS (QList<Data *> &list, int fkperiod, int skperiod, int sdperiod, int kma, int dma)
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

  TA_RetCode rc = TA_STOCH(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           &close[0],
                           fkperiod,
                           skperiod,
                           (TA_MAType) kma,
                           sdperiod,
                           (TA_MAType) dma,
                           &outBeg,
                           &outNb,
                           &out[0],
                           &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getSTOCHS: TA-Lib error" << rc;
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
