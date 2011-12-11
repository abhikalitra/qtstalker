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
#include "CurveData.h"
#include "CurveBar.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataMA.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandStochFast::CommandStochFast (QObject *p) : Command (p)
{
  _name = "STOCH_FAST";

  _values.insert(_ParmTypeK, new DataString());
  _values.insert(_ParmTypeD, new DataString());
  _values.insert(_ParmTypeLow, new DataString());
  _values.insert(_ParmTypeClose, new DataString());
  _values.insert(_ParmTypePeriodK, new DataInteger(5));
  _values.insert(_ParmTypePeriodD, new DataInteger(3));
  _values.insert(_ParmTypeMA, new DataMA("EMA"));

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandStochFast::CommandStochFast: error on TA_Initialize");
}

void CommandStochFast::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandStochFast::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *ihigh = svc.toCurve(script, _values.value(_ParmTypeHigh)->toString(), toffset);
  if (! ihigh)
  {
    Command::error("CommandStochFast::runScript: invalid High");
    return;
  }

  Data *ilow = svc.toCurve(script, _values.value(_ParmTypeLow)->toString(), toffset);
  if (! ilow)
  {
    Command::error("CommandStochFast::runScript: invalid Low");
    return;
  }

  Data *iclose = svc.toCurve(script, _values.value(_ParmTypeClose)->toString(), toffset);
  if (! iclose)
  {
    Command::error("CommandStochFast::runScript: invalid Close");
    return;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose;

  QList<Data *> lines = getSTOCHF(list,
				  _values.value(_ParmTypePeriodK)->toInteger(),
				  _values.value(_ParmTypePeriodD)->toInteger(),
				  _values.value(_ParmTypeMA)->toInteger());
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    Command::error("CommandStochFast::runScript: getSTOCHF error");
    return;
  }

  script->setData(_values.value(_ParmTypeK)->toString(), lines.at(0));
  script->setData(_values.value(_ParmTypeD)->toString(), lines.at(1));

  Command::done(QString());
}

QList<Data *> CommandStochFast::getSTOCHF (QList<Data *> &list, int kperiod, int dperiod, int type)
{
  QList<Data *> lines;
  if (! list.count())
    return lines;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
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

  TALibOutput tao;
  if (tao.fillDouble2(lines, keys, outNb, &out[0], &out2[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}
