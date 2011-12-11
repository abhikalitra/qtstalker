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

#include "CommandMFI.h"
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

CommandMFI::CommandMFI (QObject *p) : Command (p)
{
  _name = "MFI";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeHigh, new DataString());
  _values.insert(_ParmTypeLow, new DataString());
  _values.insert(_ParmTypeClose, new DataString());
  _values.insert(_ParmTypeVolume, new DataString());
  _values.insert(_ParmTypePeriod, new DataInteger(14));

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandMFI::CommandMFI: error on TA_Initialize");
}

void CommandMFI::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandMFI::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *ihigh = svc.toCurve(script, _values.value(_ParmTypeHigh)->toString(), toffset);
  if (! ihigh)
  {
    Command::error("CommandMFI::runScript: invalid High");
    return;
  }

  Data *ilow = svc.toCurve(script, _values.value(_ParmTypeLow)->toString(), toffset);
  if (! ilow)
  {
    Command::error("CommandMFI::runScript: invalid Low");
    return;
  }

  Data *iclose = svc.toCurve(script, _values.value(_ParmTypeClose)->toString(), toffset);
  if (! iclose)
  {
    Command::error("CommandMFI::runScript: invalid Close");
    return;
  }

  Data *ivol = svc.toCurve(script, _values.value(_ParmTypeVolume)->toString(), toffset);
  if (! ivol)
  {
    Command::error("CommandMFI::runScript: invalid Volume");
    return;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose << ivol;

  Data *line = getMFI(list, _values.value(_ParmTypePeriod)->toInteger());
  if (! line)
  {
    Command::error("CommandMFI::runScript: getMFI error");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandMFI::getMFI (QList<Data *> &list, int period)
{
  if (list.count() != 4)
    return 0;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
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

  TA_RetCode rc = TA_MFI(0,
                         size - 1,
                         &high[0],
                         &low[0],
                         &close[0],
                         &volume[0],
			 period,
                         &outBeg,
                         &outNb,
                         &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getMFI: TA-Lib error" << rc;
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
