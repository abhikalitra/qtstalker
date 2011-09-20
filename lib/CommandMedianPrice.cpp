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
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "SettingFactory.h"
#include "SettingDouble.h"

#include <QtDebug>

CommandMedianPrice::CommandMedianPrice (QObject *p) : Command (p)
{
  _type = "MEDIAN_PRICE";
}

int CommandMedianPrice::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  QString s = sg->value("OUTPUT");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT";
    return _ERROR;
  }
  Setting *name = vdi.setting(SettingFactory::_STRING, script, s);
  if (! name)
  {
    _message << "invalid OUTPUT " + s;
    return _ERROR;
  }

  s = sg->value("INPUT_1");
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

  QList<Data *> list;
  list << in << in2;

  Data *line = getMP(list);
  if (! line)
    return _ERROR;

  script->setData(name->toString(), line);

  return _OK;
}

Data * CommandMedianPrice::getMP (QList<Data *> &list)
{
  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
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

    double t = (bar->get(CurveBar::_VALUE)->toDouble() + bar2->get(CurveBar::_VALUE)->toDouble()) / 2.0;
    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new SettingDouble(t));
    line->set(keys.at(loop), b);
  }

  return line;
}
