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
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandStochSlow::CommandStochSlow (QObject *p) : Command (p)
{
  _type = "STOCH_SLOW";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandStochSlow::CommandStochSlow: error on TA_Initialize");
}

int CommandStochSlow::runScript (Data *sg, Script *script)
{
  QString kname = sg->get("OUTPUT_SLOWK");
  Data *line = script->data(kname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_SLOWK" << kname;
    return _ERROR;
  }

  QString dname = sg->get("OUTPUT_SLOWD");
  line = script->data(dname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_SLOWD" << dname;
    return _ERROR;
  }

  QString s = sg->get("HIGH");
  Data *ihigh = script->data(s);
  if (! ihigh)
  {
    qDebug() << _type << "::runScript: invalid HIGH" << s;
    return _ERROR;
  }

  s = sg->get("LOW");
  Data *ilow = script->data(s);
  if (! ilow)
  {
    qDebug() << _type << "::runScript: invalid LOW" << s;
    return _ERROR;
  }

  s = sg->get("CLOSE");
  Data *iclose = script->data(s);
  if (! iclose)
  {
    qDebug() << _type << "::runScript: invalid CLOSE" << s;
    return _ERROR;
  }

  int fkperiod = sg->getInteger("PERIOD_FASTK");

  int skperiod = sg->getInteger("PERIOD_SLOWK");

  MAType mat;
  s = sg->get("MA_TYPE_SLOWK");
  int kma = mat.fromString(s);
  if (kma == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE_SLOWK" << s;
    return _ERROR;
  }

  int sdperiod = sg->getInteger("PERIOD_SLOWD");

  s = sg->get("MA_TYPE_SLOWD");
  int dma = mat.fromString(s);
  if (dma == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE_SLOWD" << s;
    return _ERROR;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose;
  QList<Data *> lines = getSTOCHS(list, fkperiod, skperiod, sdperiod, kma, dma);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  Data *kline = lines.at(0);
  script->setData(kname, kline);

  Data *dline = lines.at(1);
  script->setData(dname, dline);

  return _OK;
}

QList<Data *> CommandStochSlow::getSTOCHS (QList<Data *> &list, int fkperiod, int skperiod, int sdperiod, int kma, int dma)
{
  QList<Data *> lines;
  if (! list.count())
    return lines;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &high[0], &low[0], &close[0], &close[0]);
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
  if (it.outputs(lines, keys, outNb, &out[0], &out2[0], &out2[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}

Data * CommandStochSlow::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT_SLOWK", QString());
  sg->set("OUTPUT_SLOWD", QString());
  sg->set("HIGH", QString());
  sg->set("LOW", QString());
  sg->set("CLOSE", QString());
  sg->set("PERIOD_FASTK", 5);
  sg->set("PERIOD_SLOWK", 3);
  sg->set("PERIOD_SLOWD", 3);
  sg->set("MA_TYPE_SLOWK", QString("EMA"));
  sg->set("MA_TYPE_SLOWD", QString("EMA"));
  return sg;
}
