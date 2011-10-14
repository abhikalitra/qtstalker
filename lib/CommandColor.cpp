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

#include "CommandColor.h"
#include "Operator.h"
#include "VerifyDataInput.h"
#include "DataFactory.h"
#include "CurveBar.h"
#include "CurveData.h"
#include "DataColor.h"

#include <QtDebug>

CommandColor::CommandColor (QObject *p) : Command (p)
{
  _name = "COLOR";
}

int CommandColor::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // COLOR
  QColor color;
  QString s = sg->value("COLOR");
  if (vdi.toColor(script, s, color))
  {
    qDebug() << "CommandColor::runScript: invalid COLOR" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INPUT_1
  s = sg->value("INPUT_1");
  Data *line = vdi.toCurve(script, s);
  if (! line)
  {
    qDebug() << "CommandColor::runScript: INPUT_1 not found" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset = 0;
  Data *set = line->toData(CurveData::_OFFSET);
  if (set)
    offset = set->toInteger();

  // OP
  int op;
  s = sg->value("OP");
  if (vdi.toOp(script, s, op))
  {
    qDebug() << "CommandColor::runScript: invalid OP" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INPUT_2
  s = sg->value("INPUT_2");
  Data *line2 = vdi.toCurve(script, s);
  if (! line2)
  {
    qDebug() << "CommandColor::runScript: invalid INPUT_2" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset2 = 0;
  set = line2->toData(CurveData::_OFFSET);
  if (set)
    offset2 = set->toInteger();

  // INPUT_3
  s = sg->value("INPUT_3");
  Data *line3 = vdi.toCurve(script, s);
  if (! line3)
  {
    qDebug() << "CommandColor::runScript: invalid INPUT_3" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset3 = 0;
  set = line3->toData(CurveData::_OFFSET);
  if (set)
    offset3 = set->toInteger();

  QList<Data *> list;
  list << line << line2 << line3;

  QList<int> keys;
  if (vdi.curveKeys(list, keys))
  {
    qDebug() << "CommandColor::runScript: invalid keys";
    emit signalResume((void *) this);
    return _ERROR;
  }

  Operator top;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    double v = 0;
    if (vdi.curveValue(line, keys, loop, offset, v))
      continue;

    double v2 = 0;
    if (vdi.curveValue(line2, keys, loop, offset2, v2))
      continue;

    int tloop = loop;
    tloop -= offset3;
    if (tloop < 0)
      continue;
    Data *bar3 = line3->toData(keys.at(tloop));
    if (! bar3)
      continue;

    if (top.test(v, (Operator::Type) op, v2))
      bar3->set(CurveBar::_COLOR, new DataColor(color));
  }

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
