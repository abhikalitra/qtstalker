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
#include "Operator.h"
#include "VerifyDataInput.h"
#include "DataFactory.h"
#include "CurveBar.h"
#include "DataSetting.h"
#include "SettingColor.h"
#include "SettingFactory.h"

#include <QtDebug>

CommandColor::CommandColor (QObject *p) : Command (p)
{
  _type = "COLOR";
}

int CommandColor::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // color
  QString s = sg->value("COLOR");
  Setting *color = vdi.setting(SettingFactory::_COLOR, script, s);
  if (! color)
  {
    _message << "invalid COLOR " + s;
    return _ERROR;
  }

  // verify INPUT_1
  s = sg->value("INPUT_1");
  Data *line = vdi.curveAll(script, s);
  if (! line)
  {
    _message << "INPUT_1 not found " + s;
    return _ERROR;
  }
  int offset = line->offset();

  // verify OP
  Operator top;
  s = sg->value("OP");
  Operator::Type op = top.stringToOperator(s);
  if (op == -1)
  {
    _message << "invalid OP " + s;
    return _ERROR;
  }

  // verify INPUT_2
  s = sg->value("INPUT_2");
  Data *line2 = vdi.curveAll(script, s);
  if (! line2)
  {
    _message << "invalid INPUT_2 " + s;
    return _ERROR;
  }
  int offset2 = line2->offset();

  // verify INPUT_3
  s = sg->value("INPUT_3");
  Data *line3 = vdi.curve(script, s);
  if (! line3)
  {
    _message << "INPUT_3 not found " + s;
    return _ERROR;
  }
  int offset3 = line3->offset();

  QList<Data *> list;
  list << line << line2 << line3;

  QList<int> keys;
  if (vdi.curveKeys(list, keys))
  {
    _message << "invalid keys";
    return _ERROR;
  }

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    double v = 0;
    if (vdi.curveValue(line, keys, loop, offset, v))
      continue;

    double v2 = 0;
    if (vdi.curveValue(line2, keys, loop, offset2, v2))
      continue;

    int tloop = loop - offset3;
    if (tloop < 0)
      continue;

    Data *bar3 = line3->getData(keys.at(tloop));
    if (! bar3)
      continue;

    if (top.test(v, op, v2))
      bar3->set(CurveBar::_COLOR, new SettingColor(color->toColor()));
  }

  return _OK;
}
