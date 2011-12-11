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

#include "CommandMACD.h"
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

CommandMACD::CommandMACD (QObject *p) : Command (p)
{
  _name = "MACD";

  _values.insert(_ParmTypeMACD, new DataString());
  _values.insert(_ParmTypeSignal, new DataString());
  _values.insert(_ParmTypeHist, new DataString());
  _values.insert(_ParmTypeInput, new DataString());
  _values.insert(_ParmTypePeriodFast, new DataInteger(12));
  _values.insert(_ParmTypeMAFast, new DataMA("EMA"));
  _values.insert(_ParmTypePeriodSlow, new DataInteger(24));
  _values.insert(_ParmTypeMASlow, new DataMA("EMA"));
  _values.insert(_ParmTypePeriodSignal, new DataInteger(9));
  _values.insert(_ParmTypeMASignal, new DataMA("EMA"));

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandMACD::CommandMACD: error on TA_Initialize");
}

void CommandMACD::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandMACD::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *in = svc.toCurve(script, _values.value(_ParmTypeInput)->toString(), toffset);
  if (! in)
  {
    Command::error("CommandMACD::runScript: invalid Input");
    return;
  }

  QList<Data *> list;
  list << in;

  QList<Data *> lines = getMACD(list,
				_values.value(_ParmTypePeriodFast)->toInteger(),
				_values.value(_ParmTypePeriodSlow)->toInteger(),
				_values.value(_ParmTypePeriodSignal)->toInteger(),
				_values.value(_ParmTypeMAFast)->toInteger(),
				_values.value(_ParmTypeMASlow)->toInteger(),
				_values.value(_ParmTypeMASignal)->toInteger());
  if (lines.count() != 3)
  {
    qDeleteAll(lines);
    Command::error("CommandMACD::runScript: getMACD error");
    return;
  }

  script->setData(_values.value(_ParmTypeMACD)->toString(), lines.at(0));
  script->setData(_values.value(_ParmTypeSignal)->toString(), lines.at(1));
  script->setData(_values.value(_ParmTypeHist)->toString(), lines.at(2));

  Command::done(QString());
}

QList<Data *> CommandMACD::getMACD (QList<Data *> &list, int fp, int sp, int sigp, int fma, int sma, int sigma)
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

  TA_RetCode rc = TA_MACDEXT(0,
                             size - 1,
                             &in[0],
                             fp,
                             (TA_MAType) fma,
                             sp,
                             (TA_MAType) sma,
                             sigp,
                             (TA_MAType) sigma,
                             &outBeg,
                             &outNb,
                             &out[0],
                             &out2[0],
                             &out3[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getMACD: TA-Lib error" << rc;
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
