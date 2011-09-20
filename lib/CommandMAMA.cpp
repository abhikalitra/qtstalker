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

#include "CommandMAMA.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "SettingFactory.h"

#include <QtDebug>

CommandMAMA::CommandMAMA (QObject *p) : Command (p)
{
  _type = "MAMA";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandMAMA::CommandMAMA: error on TA_Initialize");
}

int CommandMAMA::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  QString s = sg->value("OUTPUT_MAMA");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_MAMA";
    return _ERROR;
  }
  Setting *mname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! mname)
  {
    _message << "invalid OUTPUT_MAMA " + s;
    return _ERROR;
  }

  s = sg->value("OUTPUT_FAMA");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_FAMA";
    return _ERROR;
  }
  Setting *fname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! fname)
  {
    _message << "invalid OUTPUT_FAMA " + s;
    return _ERROR;
  }

  s = sg->value("INPUT");
  Data *in = vdi.curve(script, s);
  if (! in)
  {
    _message << "INPUT missing " + s;
    return _ERROR;
  }

  s = sg->value("LIMIT_FAST");
  Setting *flimit = vdi.setting(SettingFactory::_DOUBLE, script, s);
  if (! flimit)
  {
    _message << "invalid LIMIT_FAST " + s;
    return _ERROR;
  }

  s = sg->value("LIMIT_SLOW");
  Setting *slimit = vdi.setting(SettingFactory::_DOUBLE, script, s);
  if (! slimit)
  {
    _message << "invalid LIMIT_SLOW " + s;
    return _ERROR;
  }

  QList<Data *> list;
  list << in;

  QList<Data *> lines = getMAMA(list, flimit->toDouble(), slimit->toDouble());
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  script->setData(mname->toString(), lines.at(0));
  script->setData(fname->toString(), lines.at(1));

  return _OK;
}

QList<Data *> CommandMAMA::getMAMA (QList<Data *> &list, double flimit, double slimit)
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
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill1(list, keys, &in[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_MAMA(0,
                          size - 1,
                          &in[0],
                          flimit,
                          slimit,
                          &outBeg,
                          &outNb,
                          &out[0],
                          &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getMAMA: TA-Lib error" << rc;
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
