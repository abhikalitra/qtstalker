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

#include "CommandAROON.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "SettingFactory.h"

#include <QtDebug>

CommandAROON::CommandAROON (QObject *p) : Command (p)
{
  _type = "AROON";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandAROON::CommandAROON: error on TA_Initialize");
}

int CommandAROON::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  QString s = sg->value("OUTPUT_UPPER");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_UPPER";
    return _ERROR;
  }
  Setting *uname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! uname)
  {
    _message << "invalid OUTPUT_UPPER " + s;
    return _ERROR;
  }

  s = sg->value("OUTPUT_LOWER");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT_LOWER";
    return _ERROR;
  }
  Setting *lname = vdi.setting(SettingFactory::_STRING, script, s);
  if (! lname)
  {
    _message << "invalid OUTPUT_LOWER " + s;
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

  s = sg->value("PERIOD");
  Setting *period = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! period)
  {
    _message << "invalid PERIOD " + s;
    return _ERROR;
  }

  QList<Data *> list;
  list << ihigh << ilow;

  QList<Data *> lines = getAROON(list, period->toInteger());
  if (! lines.count())
    return _ERROR;

  script->setData(uname->toString(), lines.at(0));

  if (lines.count() == 2)
    script->setData(lname->toString(), lines.at(1));

  return _OK;
}

QList<Data *> CommandAROON::getAROON (QList<Data *> &list, int period)
{
  VerifyDataInput vdi;
  QList<Data *> lines;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill2(list, keys, &high[0], &low[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_AROON(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           period,
                           &outBeg,
                           &outNb,
                           &out[0],
                           &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getAROON: TA-Lib error" << rc;
    return lines;
  }

  Data *c = new CurveData;
  lines.append(c);
  c = new CurveData;
  lines.append(c);

  TALibOutput tao;
  if (tao.fillDouble2(lines, keys, outNb, &out[0], &out2[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}
