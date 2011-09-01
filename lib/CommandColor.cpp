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
#include "Script.h"
#include "Operator.h"
#include "InputType.h"
#include "CurveBar.h"

#include <QtDebug>

CommandColor::CommandColor (QObject *p) : Command (p)
{
  _type = "COLOR";
}

int CommandColor::runScript (Data *sg, Script *script)
{
  // verify INPUT_1
  QString s = sg->get("INPUT_1");
  Data *line = script->data(s);
  if (! line)
  {
    qDebug() << _type << "::runScript: INPUT_1 not found" << s;
    return _ERROR;
  }

  // get NAME_OFFSET
  int offset = sg->getInteger("INPUT_1_OFFSET");

  // verify OP
  Operator top;
  s = sg->get("OP");
  Operator::Type op = top.stringToOperator(s);
  if (op == -1)
  {
    qDebug() << _type << "::runScript: invalid OP" << s;
    return _ERROR;
  }

  // verify INPUT_2
  int valueFlag2 = FALSE;
  double value2 = 0;
  Data *line2 = 0;
  s = sg->get("INPUT_2");
  bool ok;
  value2 = s.toDouble(&ok);
  if (ok)
    valueFlag2++;
  else
  {
    line2 = script->data(s);
    if (! line2)
    {
      qDebug() << _type << "::runScript: invalid INPUT_2" << s;
      return _ERROR;
    }
  }

  // get NAME_2_OFFSET
  int offset2 = sg->getInteger("INPUT_2_OFFSET");

  // verify INPUT_3
  s = sg->get("INPUT_3");
  Data *line3 = script->data(s);
  if (! line3)
  {
    qDebug() << _type << "::runScript: INPUT_3 not found" << s;
    return _ERROR;
  }

  // get NAME_3_OFFSET
  int offset3 = sg->getInteger("INPUT_3_OFFSET");

  QColor color = sg->getColor("COLOR");

  QList<Data *> list;
  list << line << line3;
  if (! valueFlag2)
    list << line2;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
  {
    qDebug() << _type << "::runScript: invalid keys";
    return _ERROR;
  }

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    int tloop = loop - offset;
    if (tloop < 0)
      continue;

    Data *bar = line->getData(keys.at(tloop));
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

      Data *bar2 = line2->getData(keys.at(tloop));
      if (! bar2)
        continue;

      v2 = bar2->getDouble(CurveBar::_VALUE);
    }

    tloop = loop - offset3;
    if (tloop < 0)
      continue;

    Data *bar3 = line3->getData(keys.at(tloop));
    if (! bar3)
      continue;

    if (top.test(bar->getDouble(CurveBar::_VALUE), op, v2))
      bar3->set(CurveBar::_COLOR, color);
  }

  return _OK;
}

Data * CommandColor::settings ()
{
  Data *sg = new Data;
  sg->set("INPUT_1", QString());
  sg->set("INPUT_1_OFFSET", 0);
  sg->set("INPUT_2", QString());
  sg->set("INPUT_2_OFFSET", 0);
  sg->set("INPUT_3", QString());
  sg->set("INPUT_3_OFFSET", 0);
  sg->set("OP", QString("EQ"));
  sg->set("COLOR", QColor(Qt::red));
  return sg;
}
