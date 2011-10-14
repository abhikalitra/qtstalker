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

#include "CommandSINE.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"

#include <QtDebug>

CommandSINE::CommandSINE (QObject *p) : Command (p)
{
  _name = "HT_SINE";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandSINE::CommandSINE: error on TA_Initialize");
}

int CommandSINE::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT_SINE
  QString sname;
  QString s = sg->value("OUTPUT_SINE");
  if (vdi.toString(script, s, sname))
  {
    qDebug() << "CommandSINE::runScript: invalid OUTPUT_SINE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // OUTPUT_LEAD
  QString lname;
  s = sg->value("OUTPUT_LEAD");
  if (vdi.toString(script, s, lname))
  {
    qDebug() << "CommandSINE::runScript: invalid OUTPUT_LEAD" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INPUT
  s = sg->value("INPUT");
  Data *in = vdi.toCurve(script, s);
  if (! in)
  {
    qDebug() << "CommandSINE::runScript: invalid INPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QList<Data *> list;
  list << in;

  QList<Data *> lines = getSINE(list);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(sname, lines.at(0));
  script->setData(lname, lines.at(1));

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

QList<Data *> CommandSINE::getSINE (QList<Data *> &list)
{
  QList<Data *> lines;
  if (! list.count())
    return lines;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill1(list, keys, &input[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_HT_SINE (0,
                              size - 1,
                              &input[0],
                              &outBeg,
                              &outNb,
                              &out[0],
                              &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getSINE: TA-Lib error" << rc;
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
