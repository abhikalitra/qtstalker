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
  _type = "BREAKOUT";

  _method << "ABOVE" << "BELOW";
}

int CommandBreakout::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  QString s = sg->value("INPUT_1");
  Data *in = vdi.curve(script, s);
  if (! in)
  {
    _message << "INPUT_1 missing " + s;
    return _ERROR;
  }

  s = sg->value("INPUT_2");
  Data *in2 = vdi.curve(script, s);
  if (! in2)
  {
    _message << "INPUT_2 missing " + s;
    return _ERROR;
  }

  s = sg->value("METHOD");
  int method = _method.indexOf(s);
  if (method == -1)
  {
    _message << "invalid METHOD " + s;
    return _ERROR;
  }

  int flag = 0;
  int rc = breakout(in, in2, method, flag);
  if (rc)
    return _ERROR;

  _returnString = QString::number(flag);

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
    for (; loop < end; loop++)
    {
      Data *bar = in->getData(keys.at(loop));
      if (! bar)
        continue;

      Data *bar2 = in2->getData(keys.at(loop));
      if (! bar2)
        continue;

      double v = bar->get(CurveBar::_VALUE)->toDouble();
      double v2 = bar2->get(CurveBar::_VALUE)->toDouble();
      if (v > v2)
        return 0;
    }
  }
  else
  {
    for (; loop < end; loop++)
    {
      Data *bar = in->getData(keys.at(loop));
      if (! bar)
        continue;

      Data *bar2 = in2->getData(keys.at(loop));
      if (! bar2)
        continue;

      double v = bar->get(CurveBar::_VALUE)->toDouble();
      double v2 = bar2->get(CurveBar::_VALUE)->toDouble();
      if (v < v2)
        return 0;
    }
  }

  // compare last bar to confirm breakout
  Data *bar = in->getData(keys.at(end));
  if (! bar)
    return 0;

  Data *bar2 = in2->getData(keys.at(end));
  if (! bar2)
    return 0;

  double v = bar->get(CurveBar::_VALUE)->toDouble();
  double v2 = bar2->get(CurveBar::_VALUE)->toDouble();

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
