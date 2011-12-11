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
#include "TALibInput.h"
#include "TALibOutput.h"
#include "DataString.h"
#include "DataDouble.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandMAMA::CommandMAMA (QObject *p) : Command (p)
{
  _name = "MAMA";

  _values.insert(_ParmTypeMAMA, new DataString());
  _values.insert(_ParmTypeFAMA, new DataString());
  _values.insert(_ParmTypeInput, new DataString());
  _values.insert(_ParmTypeLimitFast, new DataDouble(0.5));
  _values.insert(_ParmTypeLimitSlow, new DataDouble(0.05));

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandMAMA::CommandMAMA: error on TA_Initialize");
}

void CommandMAMA::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandMAMA::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *in = svc.toCurve(script, _values.value(_ParmTypeInput)->toString(), toffset);
  if (! in)
  {
    Command::error("CommandMAMA::runScript: invalid Input");
    return;
  }

  QList<Data *> list;
  list << in;

  QList<Data *> lines = getMAMA(list,
				_values.value(_ParmTypeLimitFast)->toDouble(),
				_values.value(_ParmTypeLimitSlow)->toDouble());
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    Command::error("CommandMAMA::runScript: getMAMA error");
    return;
  }

  script->setData(_values.value(_ParmTypeMAMA)->toString(), lines.at(0));
  script->setData(_values.value(_ParmTypeFAMA)->toString(), lines.at(1));

  Command::done(QString());
}

QList<Data *> CommandMAMA::getMAMA (QList<Data *> &list, double flimit, double slimit)
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
