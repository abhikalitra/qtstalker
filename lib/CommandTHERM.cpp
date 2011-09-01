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

/* The "Market Thermometer" _indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

/* Alert Notes
1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days
*/


#include "CommandTHERM.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>
#include <cmath>

CommandTHERM::CommandTHERM (QObject *p) : Command (p)
{
  _type = "THERM";
}

int CommandTHERM::runScript (Data *sg, Script *script)
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

  QList<Data *> list;
  list << ihigh << ilow;
  line = getTHERM(list);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandTHERM::getTHERM (QList<Data *> &list)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  Data *line = new CurveData;
  Data *ihigh = list.at(0);
  Data *ilow = list.at(1);
  double thermometer = 0;
  int loop = 1;
  for (; loop < keys.count(); loop++)
  {
    Data *hbar = ihigh->getData(keys.at(loop));
    if (! hbar)
      continue;

    Data *phbar = ihigh->getData(keys.at(loop - 1));
    if (! phbar)
      continue;

    Data *lbar = ilow->getData(keys.at(loop));
    if (! lbar)
      continue;

    Data *plbar = ilow->getData(keys.at(loop - 1));
    if (! plbar)
      continue;

    double high = fabs(hbar->getDouble(CurveBar::_VALUE) - phbar->getDouble(CurveBar::_VALUE));
    double lo = fabs(plbar->getDouble(CurveBar::_VALUE) - lbar->getDouble(CurveBar::_VALUE));

    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, thermometer);
    line->set(keys.at(loop), b);
  }

  return line;
}

Data * CommandTHERM::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QString());
  sg->set("HIGH", QString());
  sg->set("LOW", QString());
  return sg;
}
