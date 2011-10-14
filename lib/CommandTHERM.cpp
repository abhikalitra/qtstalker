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
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "DataDouble.h"

#include <QtDebug>
#include <cmath>

CommandTHERM::CommandTHERM (QObject *p) : Command (p)
{
  _name = "THERM";
}

int CommandTHERM::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandTHERM::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // HIGH
  s = sg->value("HIGH");
  Data *ihigh = vdi.toCurve(script, s);
  if (! ihigh)
  {
    qDebug() << "CommandTHERM::runScript: invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // LOW
  s = sg->value("LOW");
  Data *ilow = vdi.toCurve(script, s);
  if (! ilow)
  {
    qDebug() << "CommandTHERM::runScript: invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QList<Data *> list;
  list << ihigh << ilow;

  Data *line = getTHERM(list);
  if (! line)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(name, line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

Data * CommandTHERM::getTHERM (QList<Data *> &list)
{
  if (list.count() != 2)
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;

  Data *line = new CurveData;
  Data *ihigh = list.at(0);
  Data *ilow = list.at(1);
  double thermometer = 0;
  int loop = 1;
  for (; loop < keys.count(); loop++)
  {
    Data *hbar = ihigh->toData(keys.at(loop));
    if (! hbar)
      continue;

    Data *phbar = ihigh->toData(keys.at(loop - 1));
    if (! phbar)
      continue;

    Data *lbar = ilow->toData(keys.at(loop));
    if (! lbar)
      continue;

    Data *plbar = ilow->toData(keys.at(loop - 1));
    if (! plbar)
      continue;

    double high = fabs(hbar->toData(CurveBar::_VALUE)->toDouble() - phbar->toData(CurveBar::_VALUE)->toDouble());
    double lo = fabs(plbar->toData(CurveBar::_VALUE)->toDouble() - lbar->toData(CurveBar::_VALUE)->toDouble());

    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(thermometer));
    line->set(keys.at(loop), b);
  }

  return line;
}
