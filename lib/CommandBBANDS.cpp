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

#include "CommandBBANDS.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataMA.h"
#include "DataDouble.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandBBANDS::CommandBBANDS (QObject *p) : Command (p)
{
  _name = "BBANDS";

  _values.insert(_ParmTypeUpper, new DataString());
  _values.insert(_ParmTypeMiddle, new DataString());
  _values.insert(_ParmTypeLower, new DataString());
  _values.insert(_ParmTypeInput, new DataString());
  _values.insert(_ParmTypePeriod, new DataInteger(20));
  _values.insert(_ParmTypeMAType, new DataMA("EMA"));
  _values.insert(_ParmTypeDevUp, new DataDouble(2));
  _values.insert(_ParmTypeDevDown, new DataDouble(2));

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandBBANDS::CommandBBANDS: error on TA_Initialize");
}

void CommandBBANDS::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandBBANDS::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *in = svc.toCurve(script, _values.value(_ParmTypeInput)->toString(), toffset);
  if (! in)
  {
    Command::error("CommandBBANDS::runScript: invalid Input");
    return;
  }

  QList<Data *> list;
  list << in;

  QList<Data *> lines = getBBANDS(list,
				  _values.value(_ParmTypePeriod)->toInteger(),
				  _values.value(_ParmTypeDevUp)->toDouble(),
				  _values.value(_ParmTypeDevDown)->toDouble(),
				  _values.value(_ParmTypeMAType)->toInteger());
  if (lines.count() != 3)
  {
    qDeleteAll(lines);
    Command::error("CommandBBANDS::runScript: getBBANDS error");
    return;
  }

  script->setData(_values.value(_ParmTypeUpper)->toString(), lines.at(0));
  script->setData(_values.value(_ParmTypeMiddle)->toString(), lines.at(1));
  script->setData(_values.value(_ParmTypeLower)->toString(), lines.at(2));

  Command::done(QString());
}

QList<Data *> CommandBBANDS::getBBANDS (QList<Data *> &list, int period, double udev, double ldev, int type)
{
  QList<Data *> lines;
  if (! list.count())
    return lines;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real in[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill1(list, keys, &in[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_BBANDS(0,
                            size - 1,
                            &in[0],
                            period,
                            udev,
                            ldev,
                            (TA_MAType) type,
                            &outBeg,
                            &outNb,
                            &out[0],
                            &out2[0],
			    &out3[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getBBANDS: TA-Lib error" << rc;
    return lines;
  }

  lines.clear();

  Data *c = new CurveData;
  lines << c;
  c = new CurveData;
  lines << c;
  c = new CurveData;
  lines << c;

  TALibOutput tao;
  if (tao.fillDouble3(lines, keys, outNb, &out[0], &out2[0], &out3[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}
