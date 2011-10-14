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

#include "CommandSAR.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"

#include <QtDebug>

CommandSAR::CommandSAR (QObject *p) : Command (p)
{
  _name = "SAR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandSAR::CommandSAR: error on TA_Initialize");
}

int CommandSAR::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandSAR::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // HIGH
  s = sg->value("HIGH");
  Data *ihigh = vdi.toCurve(script, s);
  if (! ihigh)
  {
    qDebug() << "CommandSAR::runScript: invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // LOW
  s = sg->value("LOW");
  Data *ilow = vdi.toCurve(script, s);
  if (! ilow)
  {
    qDebug() << "CommandSAR::runScript: invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // STEP_INITIAL
  double init = 0.02;
  s = sg->value("STEP_INITIAL");
  if (vdi.toDouble(script, s, init))
  {
    qDebug() << "CommandSAR::runScript: invalid STEP_INITIAL, using default" << s;
    init = 0.02;
  }

  // STEP_MAX
  double max = 0.02;
  s = sg->value("STEP_MAX");
  if (vdi.toDouble(script, s, max))
  {
    qDebug() << "CommandSAR::runScript: invalid STEP_INITIAL, using default" << s;
    max = 0.2;
  }

  QList<Data *> list;
  list << ihigh << ilow;

  Data *line = getSAR(list, init, max);
  if (! line)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(name, line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

Data * CommandSAR::getSAR (QList<Data *> &list, double init, double max)
{
  if (list.count() != 2)
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill2(list, keys, &high[0], &low[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SAR(0,
                         size - 1,
                         &high[0],
                         &low[0],
			 init,
			 max,
                         &outBeg,
                         &outNb,
                         &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getSAR: TA-Lib error" << rc;
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
