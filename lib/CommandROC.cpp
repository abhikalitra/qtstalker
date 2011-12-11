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

#include "CommandROC.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataList.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandROC::CommandROC (QObject *p) : Command (p)
{
  _name = "ROC";
  _method << "ROC" << "ROCP" << "ROCR" << "ROCR100";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeInput, new DataString());
  _values.insert(_ParmTypePeriod, new DataInteger(10));

  DataList *dl = new DataList(_method.at(0));
  dl->set(_method);
  _values.insert(_ParmTypeMethod, dl);

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandROC::CommandROC: error on TA_Initialize");
}

void CommandROC::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandROC::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *in = svc.toCurve(script, _values.value(_ParmTypeInput)->toString(), toffset);
  if (! in)
  {
    Command::error("CommandROC::runScript: invalid Input");
    return;
  }

  QList<Data *> list;
  list << in;

  Data *line = getROC(list,
		      _values.value(_ParmTypePeriod)->toInteger(),
		      _values.value(_ParmTypeMethod)->toInteger());
  if (! line)
  {
    Command::error("CommandROC::runScript: getROC error");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandROC::getROC (QList<Data *> &list, int period, int method)
{
  if (! list.count())
    return 0;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill1(list, keys, &input[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_ROC(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 1:
      rc = TA_ROCP(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 2:
      rc = TA_ROCR(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 3:
      rc = TA_ROCR100(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getROC: TA-Lib error" << rc;
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
