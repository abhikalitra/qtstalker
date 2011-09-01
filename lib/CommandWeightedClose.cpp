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

#include "CommandWeightedClose.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandWeightedClose::CommandWeightedClose (QObject *p) : Command (p)
{
  _type = "AVERAGE_PRICE";
}

int CommandWeightedClose::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT");
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("HIGH");
  Data *ihigh = script->data(s);
  if (! ihigh)
  {
    qDebug() << _type << "::runScript: invalid HIGH" << s;
    return _ERROR;
  }

  s = sg->get("LOW");
  Data *ilow = script->data(s);
  if (! ilow)
  {
    qDebug() << _type << "::runScript: invalid LOW" << s;
    return _ERROR;
  }

  s = sg->get("CLOSE");
  Data *iclose = script->data(s);
  if (! iclose)
  {
    qDebug() << _type << "::runScript: invalid CLOSE" << s;
    return _ERROR;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose;
  line = getWC(list);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandWeightedClose::getWC (QList<Data *> &list)
{
  if (list.count() != 3)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  Data *line = new CurveData;
  int loop = 0;
  Data *ihigh = list.at(loop++);
  Data *ilow = list.at(loop++);
  Data *iclose = list.at(loop++);
  for (loop = 0; loop < keys.count(); loop++)
  {
    Data *hbar = ihigh->getData(keys.at(loop));
    if (! hbar)
      continue;

    Data *lbar = ilow->getData(keys.at(loop));
    if (! lbar)
      continue;

    Data *cbar = iclose->getData(keys.at(loop));
    if (! cbar)
      continue;

    double t = (hbar->getDouble(CurveBar::_VALUE) + lbar->getDouble(CurveBar::_VALUE) + (cbar->getDouble(CurveBar::_VALUE) * 2)) / 4.0;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, t);
    line->set(keys.at(loop), b);
  }

  return line;
}

Data * CommandWeightedClose::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QString());
  sg->set("HIGH", QString());
  sg->set("LOW", QString());
  sg->set("CLOSE", QString());
  return sg;
}
