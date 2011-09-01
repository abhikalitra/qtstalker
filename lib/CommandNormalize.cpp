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

#include "CommandNormalize.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>
#include <cmath>

CommandNormalize::CommandNormalize (QObject *p) : Command (p)
{
  _type = "NORMALIZE";
}

int CommandNormalize::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT");
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("INPUT");
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: invalid INPUT" << s;
    return _ERROR;
  }

  QList<Data *> list;
  list << in;
  line = getNORM(list);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandNormalize::getNORM (QList<Data *> &list)
{
  if (! list.count())
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  Data *in = list.at(0);
  double max = 0;
  double min = 0;
  if (in->highLow(max, min))
    return 0;

  double range = fabs(max) + fabs(min);

  Data *line = new CurveData;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *bar = in->getData(keys.at(loop));
    double t = ((bar->getDouble(CurveBar::_VALUE) - min) / range) * 100;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, t);
    line->set(keys.at(loop), t);
  }

  return line;
}

Data * CommandNormalize::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QString());
  sg->set("INPUT", QString());
  return sg;
}
