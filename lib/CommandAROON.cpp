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
#include "TALibInput.h"
#include "TALibOutput.h"
#include "DataString.h"
#include "DataInteger.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandAROON::CommandAROON (QObject *p) : Command (p)
{
  _name = "AROON";

  _values.insert(_ParmTypeUpper, new DataString());
  _values.insert(_ParmTypeLower, new DataString());
  _values.insert(_ParmTypeHigh, new DataString());
  _values.insert(_ParmTypeLow, new DataString());
  _values.insert(_ParmTypeClose, new DataString());
  _values.insert(_ParmTypePeriod, new DataInteger(10));

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandAROON::CommandAROON: error on TA_Initialize");
}

void CommandAROON::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandAROON::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *ihigh = svc.toCurve(script, _values.value(_ParmTypeHigh)->toString(), toffset);
  if (! ihigh)
  {
    Command::error("CommandAROON::runScript: invalid High");
    return;
  }

  Data *ilow = svc.toCurve(script, _values.value(_ParmTypeLow)->toString(), toffset);
  if (! ilow)
  {
    Command::error("CommandAROON::runScript: invalid Low");
    return;
  }

  QList<Data *> list;
  list << ihigh << ilow;

  QList<Data *> lines = getAROON(list, _values.value(_ParmTypePeriod)->toInteger());
  if (lines.count() != 2)
  {
    Command::error("CommandAROON::runScript: getAROON error");
    return;
  }

  script->setData(_values.value(_ParmTypeUpper)->toString(), lines.at(0));
  script->setData(_values.value(_ParmTypeLower)->toString(), lines.at(1));

  Command::done(QString());
}

QList<Data *> CommandAROON::getAROON (QList<Data *> &list, int period)
{
  QList<Data *> lines;
  QList<int> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys(list, keys))
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
