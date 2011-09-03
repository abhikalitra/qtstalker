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

#include "CommandCompare.h"
#include "Operator.h"
#include "Script.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "InputType.h"

#include <QtDebug>

CommandCompare::CommandCompare (QObject *p) : Command (p)
{
  _type = "COMPARE";
}

int CommandCompare::runScript (Data *sg, Script *script)
{
  // OUTPUT
  QString name = sg->get("OUTPUT").toString();
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  // INPUT_1
  QString s = sg->get("INPUT_1").toString();
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT_1 not found" << s;
    return _ERROR;
  }

  int offset = sg->get("INPUT_1_OFFSET").toInt();

  // verify OP
  Operator top;
  s = sg->get("OP").toString();
  Operator::Type op = top.stringToOperator(s);
  if (op == -1)
  {
    qDebug() << _type << "::runScript: invalid OP" << s;
    return _ERROR;
  }

  // INPUT_2
  int valueFlag2 = FALSE;
  Data *in2 = 0;
  s = sg->get("INPUT_2").toString();
  bool ok;
  double value2 = s.toDouble(&ok);
  if (ok)
    valueFlag2++;
  else
  {
    in2 = script->data(s);
    if (! in2)
    {
      qDebug() << _type << "::runScript: INPUT_2 not found" << s;
      return _ERROR;
    }
  }

  int offset2 = sg->get("INPUT_2_OFFSET").toInt();

  // RESULT_1
  int valueFlag3 = FALSE;
  Data *in3 = 0;
  s = sg->get("RESULT_1").toString();
  double value3 = s.toDouble(&ok);
  if (ok)
    valueFlag3++;
  else
  {
    in3 = script->data(s);
    if (! in3)
    {
      qDebug() << _type << "::runScript: RESULT_1 not found" << s;
      return _ERROR;
    }
  }

  int offset3 = sg->get("RESULT_1_OFFSET").toInt();

  // INPUT_4
  int valueFlag4 = FALSE;
  Data *in4 = 0;
  s = sg->get("RESULT_2").toString();
  double value4 = s.toDouble(&ok);
  if (ok)
    valueFlag4++;
  else
  {
    in4 = script->data(s);
    if (! in4)
    {
      qDebug() << _type << "::runScript: RESULT_2 not found" << s;
      return _ERROR;
    }
  }

  int offset4 = sg->get("RESULT_2_OFFSET").toInt();

  QList<Data *> list;
  list << in;
  if (! valueFlag2)
    list << in2;
  if (! valueFlag3)
    list << in3;
  if (! valueFlag4)
    list << in4;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
  {
    qDebug() << _type << "::runScript: invalid keys";
    return _ERROR;
  }

  line = new CurveData;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    int tloop = loop - offset;
    if (tloop < 0)
      continue;

    Data *bar = in->getData(keys.at(tloop));
    if (! bar)
      continue;

    double v2 = 0;
    if (valueFlag2)
      v2 = value2;
    else
    {
      tloop = loop - offset2;
      if (tloop < 0)
        continue;

      Data *bar2 = in2->getData(keys.at(tloop));
      if (! bar2)
        continue;

      v2 = bar2->get(CurveBar::_VALUE).toDouble();
    }

    double v3 = 0;
    if (valueFlag3)
      v3 = value3;
    else
    {
      tloop = loop - offset3;
      if (tloop < 0)
        continue;

      Data *bar3 = in3->getData(keys.at(tloop));
      if (! bar3)
        continue;

      v3 = bar3->get(CurveBar::_VALUE).toDouble();
    }

    double v4 = 0;
    if (valueFlag4)
      v4 = value4;
    else
    {
      tloop = loop - offset4;
      if (tloop < 0)
        continue;

      Data *bar4 = in4->getData(keys.at(tloop));
      if (! bar4)
        continue;

      v4 = bar4->get(CurveBar::_VALUE).toDouble();
    }

    if (top.test(bar->get(CurveBar::_VALUE).toDouble(), op, v2))
    {
      Data *b = new CurveBar;
      b->set(CurveBar::_VALUE, QVariant(v3));
      line->set(keys.at(loop), b);
    }
    else
    {
      Data *b = new CurveBar;
      b->set(CurveBar::_VALUE, QVariant(v4));
      line->set(keys.at(loop), b);
    }
  }

  script->setData(name, line);

  return _OK;
}

Data * CommandCompare::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QVariant(QString()));
  sg->set("INPUT_1", QVariant(QString()));
  sg->set("INPUT_1_OFFSET", QVariant(0));
  sg->set("INPUT_2", QVariant(QString()));
  sg->set("INPUT_2_OFFSET", QVariant(0));
  sg->set("RESULT_1", QVariant(QString()));
  sg->set("RESULT_1_OFFSET", QVariant(0));
  sg->set("RESULT_2", QVariant(QString()));
  sg->set("RESULT_2_OFFSET", QVariant(0));
  sg->set("OP", QVariant(QString("EQ")));
  return sg;
}
