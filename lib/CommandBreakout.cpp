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

#include "CommandBreakout.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"

#include <QtDebug>

CommandBreakout::CommandBreakout (QObject *p) : Command (p)
{
  _name = "BREAKOUT";
  _method << "ABOVE" << "BELOW";
}

int CommandBreakout::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // INPUT_1
  QString s = sg->value("INPUT_1");
  Data *in = vdi.toCurve(script, s);
  if (! in)
  {
    qDebug() << "CommandBreakout::runScript: INPUT_1 missing" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INPUT_2
  s = sg->value("INPUT_2");
  Data *in2 = vdi.toCurve(script, s);
  if (! in2)
  {
    qDebug() << "CommandBreakout::runScript: INPUT_2 missing" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // METHOD
  s = sg->value("METHOD");
  int method = _method.indexOf(s);
  if (method == -1)
  {
    qDebug() << "CommandBreakout::runScript: invalid METHOD" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  int flag = 0;
  int rc = breakout(in, in2, method, flag);
  if (rc)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  _returnString = QString::number(flag);

  emit signalResume((void *) this);

  return _OK;
}

int CommandBreakout::breakout (Data *in, Data *in2, int method, int &flag)
{
  flag = 0;

  QList<Data *> list;
  list << in << in2;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 1;

  int end = keys.count() - 1;
  if (end < 0)
    return 1;

  int loop = 0;

  if (method == 0)
  {
    for (; loop < end - 1; loop++)
    {
      Data *bar = in->toData(keys.at(loop));
      if (! bar)
        continue;

      Data *bar2 = in2->toData(keys.at(loop));
      if (! bar2)
        continue;

      double v = bar->toData(CurveBar::_VALUE)->toDouble();
      double v2 = bar2->toData(CurveBar::_VALUE)->toDouble();
      if (v > v2)
        return 0;
    }
  }
  else
  {
    for (; loop < end - 1; loop++)
    {
      Data *bar = in->toData(keys.at(loop));
      if (! bar)
        continue;

      Data *bar2 = in2->toData(keys.at(loop));
      if (! bar2)
        continue;

      double v = bar->toData(CurveBar::_VALUE)->toDouble();
      double v2 = bar2->toData(CurveBar::_VALUE)->toDouble();
      if (v < v2)
        return 0;
    }
  }

  // compare last bar to confirm breakout
  Data *bar = in->toData(keys.at(end));
  if (! bar)
    return 0;

  Data *bar2 = in2->toData(keys.at(end));
  if (! bar2)
    return 0;

  double v = bar->toData(CurveBar::_VALUE)->toDouble();
  double v2 = bar2->toData(CurveBar::_VALUE)->toDouble();

  if (method == 0)
  {
    if (v <= v2)
      return 0;
  }
  else
  {
    if (v >= v2)
      return 0;
  }

  flag++;

  return 0;
}
