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

#include "CommandULTOSC.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"

#include <QtDebug>

CommandULTOSC::CommandULTOSC (QObject *p) : Command (p)
{
  _name = "ULTOSC";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandULTOSC::CommandULTOSC: error on TA_Initialize");
}

int CommandULTOSC::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandULTOSC::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // HIGH
  s = sg->value("HIGH");
  Data *ihigh = vdi.toCurve(script, s);
  if (! ihigh)
  {
    qDebug() << "CommandULTOSC::runScript: invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // LOW
  s = sg->value("LOW");
  Data *ilow = vdi.toCurve(script, s);
  if (! ilow)
  {
    qDebug() << "CommandULTOSC::runScript: invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // CLOSE
  s = sg->value("CLOSE");
  Data *iclose = vdi.toCurve(script, s);
  if (! iclose)
  {
    qDebug() << "CommandULTOSC::runScript: invalid CLOSE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // PERIOD
  int sp = 7;
  s = sg->value("PERIOD_SHORT");
  if (vdi.toInteger(script, s, sp))
  {
    qDebug() << "CommandULTOSC::runScript: invalid PERIOD_SHORT, using default" << s;
    sp = 7;
  }

  // PERIOD_MED
  int mp = 14;
  s = sg->value("PERIOD_MED");
  if (vdi.toInteger(script, s, mp))
  {
    qDebug() << "CommandULTOSC::runScript: invalid PERIOD_MED, using default" << s;
    mp = 14;
  }

  // PERIOD_LONG
  int lp = 28;
  s = sg->value("PERIOD_LONG");
  if (vdi.toInteger(script, s, lp))
  {
    qDebug() << "CommandULTOSC::runScript: invalid PERIOD_LONG, using default" << s;
    lp = 28;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose;

  Data *line = getULTOSC(list, sp, mp, lp);
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

Data * CommandULTOSC::getULTOSC (QList<Data *> &list, int sp, int mp, int lp)
{
  if (list.count() != 3)
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill3(list, keys, &high[0], &low[0], &close[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_ULTOSC(0,
                            size - 1,
                            &high[0],
                            &low[0],
                            &close[0],
			    sp,
			    mp,
			    lp,
                            &outBeg,
                            &outNb,
                            &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getULTOSC: TA-Lib error" << rc;
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
