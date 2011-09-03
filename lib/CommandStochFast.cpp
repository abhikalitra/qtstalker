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
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandStochFast::CommandStochFast (QObject *p) : Command (p)
{
  _type = "STOCH_FAST";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandStochFast::CommandStochFast: error on TA_Initialize");
}

int CommandStochFast::runScript (Data *sg, Script *script)
{
  QString kname = sg->get("OUTPUT_FASTK").toString();
  Data *line = script->data(kname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_FASTK" << kname;
    return _ERROR;
  }

  QString dname = sg->get("OUTPUT_FASTD").toString();
  line = script->data(dname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_FASTD" << dname;
    return _ERROR;
  }

  QString s = sg->get("HIGH").toString();
  Data *ihigh = script->data(s);
  if (! ihigh)
  {
    qDebug() << _type << "::runScript: invalid HIGH" << s;
    return _ERROR;
  }

  s = sg->get("LOW").toString();
  Data *ilow = script->data(s);
  if (! ilow)
  {
    qDebug() << _type << "::runScript: invalid LOW" << s;
    return _ERROR;
  }

  s = sg->get("CLOSE").toString();
  Data *iclose = script->data(s);
  if (! iclose)
  {
    qDebug() << _type << "::runScript: invalid CLOSE" << s;
    return _ERROR;
  }

  int kperiod = sg->get("PERIOD_FASTK").toInt();

  int dperiod = sg->get("PERIOD_FASTD").toInt();

  MAType mat;
  s = sg->get("MA_TYPE").toString();
  int type = mat.fromString(s);
  if (type == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE" << s;
    return _ERROR;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose;
  QList<Data *> lines = getSTOCHF(list, kperiod, dperiod, type);
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

QList<Data *> CommandStochFast::getSTOCHF (QList<Data *> &list, int kperiod, int dperiod, int type)
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
  if (it.outputs(lines, keys, outNb, &out[0], &out2[0], &out2[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}

Data * CommandStochFast::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT_FASTK", QVariant(QString()));
  sg->set("OUTPUT_FASTD", QVariant(QString()));
  sg->set("HIGH", QVariant(QString()));
  sg->set("LOW", QVariant(QString()));
  sg->set("CLOSE", QVariant(QString()));
  sg->set("PERIOD_FASTK", QVariant(5));
  sg->set("PERIOD_FASTD", QVariant(3));
  sg->set("MA_TYPE", QVariant(QString("EMA")));
  return sg;
}
