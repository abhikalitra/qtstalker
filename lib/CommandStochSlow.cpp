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
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"

#include <QtDebug>

CommandStochSlow::CommandStochSlow (QObject *p) : Command (p)
{
  _name = "STOCH_SLOW";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandStochSlow::CommandStochSlow: error on TA_Initialize");
}

int CommandStochSlow::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT_SLOWK
  QString kname;
  QString s = sg->value("OUTPUT_SLOWK");
  if (vdi.toString(script, s, kname))
  {
    qDebug() << "CommandStochSlow::runScript: invalid OUTPUT_SLOWK" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // OUTPUT_SLOWD
  QString dname;
  s = sg->value("OUTPUT_SLOWD");
  if (vdi.toString(script, s, dname))
  {
    qDebug() << "CommandStochSlow::runScript: invalid OUTPUT_SLOWD" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // HIGH
  s = sg->value("HIGH");
  Data *ihigh = vdi.toCurve(script, s);
  if (! ihigh)
  {
    qDebug() << "CommandStochSlow::runScript: invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // LOW
  s = sg->value("LOW");
  Data *ilow = vdi.toCurve(script, s);
  if (! ilow)
  {
    qDebug() << "CommandStochSlow::runScript: invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // CLOSE
  s = sg->value("CLOSE");
  Data *iclose = vdi.toCurve(script, s);
  if (! iclose)
  {
    qDebug() << "CommandStochSlow::runScript: invalid CLOSE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // PERIOD_FASTK
  int fkperiod = 5;
  s = sg->value("PERIOD_FASTK");
  if (vdi.toInteger(script, s, fkperiod))
  {
    qDebug() << "CommandStochSlow::runScript: invalid PERIOD_FASTK, using default" << s;
    fkperiod = 5;
  }

  // PERIOD_SLOWK
  int skperiod = 3;
  s = sg->value("PERIOD_SLOWK");
  if (vdi.toInteger(script, s, skperiod))
  {
    qDebug() << "CommandStochSlow::runScript: invalid PERIOD_SLOWK, using default" << s;
    skperiod = 3;
  }

  // MA_TYPE_SLOWK
  int kma = 0;
  s = sg->value("MA_TYPE_SLOWK");
  if (vdi.toMA(script, s, kma))
  {
    qDebug() << "CommandStochSlow::runScript: invalid MA_TYPE_SLOWK, using default" << s;
    kma = 0;
  }

  // PERIOD_SLOWD
  int sdperiod = 3;
  s = sg->value("PERIOD_SLOWD");
  if (vdi.toInteger(script, s, sdperiod))
  {
    qDebug() << "CommandStochSlow::runScript: invalid PERIOD_SLOWD, using default" << s;
    sdperiod = 3;
  }

  // MA_TYPE_SLOWD
  int dma = 0;
  s = sg->value("MA_TYPE_SLOWD");
  if (vdi.toMA(script, s, dma))
  {
    qDebug() << "CommandStochSlow::runScript: invalid MA_TYPE_SLOWD, using default" << s;
    dma = 0;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose;

  QList<Data *> lines = getSTOCHS(list, fkperiod, skperiod, sdperiod, kma, dma);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(kname, lines.at(0));
  script->setData(dname, lines.at(1));

  _returnString = "OK";

  emit signalResume((void *) this);

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
