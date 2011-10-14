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

#include "CommandAD.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"

#include <QtDebug>

CommandAD::CommandAD (QObject *p) : Command (p)
{
  _name = "AD";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandAD::CommandAD: error on TA_Initialize");
}

int CommandAD::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandAD::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("HIGH");
  Data *ihigh = vdi.toCurve(script, s);
  if (! ihigh)
  {
    qDebug() << "CommandAD::runScript: invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("LOW");
  Data *ilow = vdi.toCurve(script, s);
  if (! ilow)
  {
    qDebug() << "CommandAD::runScript: invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("CLOSE");
  Data *iclose = vdi.toCurve(script, s);
  if (! iclose)
  {
    qDebug() << "CommandAD::runScript: invalid CLOSE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("VOLUME");
  Data *ivol = vdi.toCurve(script, s);
  if (! ivol)
  {
    qDebug() << "CommandAD::runScript: invalid VOLUME" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose << ivol;

  Data *line = getAD(list);
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

Data * CommandAD::getAD (QList<Data *> &list)
{
  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real volume[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill4(list, keys, &high[0], &low[0], &close[0], &volume[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_AD(0,
                        size - 1,
                        &high[0],
                        &low[0],
                        &close[0],
                        &volume[0],
                        &outBeg,
                        &outNb,
                        &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getAD: TA-Lib error" << rc;
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
