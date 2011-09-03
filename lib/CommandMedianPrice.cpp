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

#include "CommandMedianPrice.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandMedianPrice::CommandMedianPrice (QObject *p) : Command (p)
{
  _type = "MEDIAN_PRICE";
}

int CommandMedianPrice::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT").toString();
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("INPUT_1").toString();
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT_1 missing" << s;
    return _ERROR;
  }

  s = sg->get("INPUT_2").toString();
  Data *in2 = script->data(s);
  if (! in2)
  {
    qDebug() << _type << "::runScript: INPUT_2 missing" << s;
    return _ERROR;
  }

  QList<Data *> list;
  list << in << in2;
  line = getMP(list);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandMedianPrice::getMP (QList<Data *> &list)
{
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  Data *line = new CurveData;
  Data *in = list.at(0);
  Data *in2 = list.at(1);
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *bar = in->getData(keys.at(loop));
    if (! bar)
      continue;

    Data *bar2 = in2->getData(keys.at(loop));
    if (! bar2)
      continue;

    double t = (bar->get(CurveBar::_VALUE).toDouble() + bar2->get(CurveBar::_VALUE).toDouble()) / 2.0;
    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, QVariant(t));
    line->set(keys.at(loop), b);
  }

  return line;
}

Data * CommandMedianPrice::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QVariant(QString()));
  sg->set("INPUT_1", QVariant(QString()));
  sg->set("INPUT_2", QVariant(QString()));
  return sg;
}
