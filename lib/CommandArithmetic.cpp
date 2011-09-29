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

#include "CommandArithmetic.h"
#include "VerifyDataInput.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "SettingDouble.h"

#include <QtDebug>

CommandArithmetic::CommandArithmetic (QObject *p) : Command (p)
{
  _name = "ARITHMETIC";
  _method << "ADD" << "DIV" << "MULT" << "SUB";
}

int CommandArithmetic::runScript (Message *sg, Script *script)
{
  // verify NAME
  QString name = sg->value("OUTPUT");
  if (name.isEmpty())
  {
    qDebug() << "CommandArithmetic::runScript: invalid OUTPUT " << name;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // verify INPUT_1
  VerifyDataInput vdi;
  QString s = sg->value("INPUT_1");
  Data *in = vdi.curveAll(script, s);
  if (! in)
  {
    qDebug() << "CommandArithmetic::runScript: invalid INPUT_1 " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset = in->offset();

  // verify INPUT_2
  s = sg->value("INPUT_2");
  Data *in2 = vdi.curveAll(script, s);
  if (! in2)
  {
    qDebug() << "CommandArithmetic::runScript: invalid INPUT_2 " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset2 = in2->offset();

  s = sg->value("METHOD");
  int method = _method.indexOf(s);
  if (method == -1)
  {
    qDebug() << "CommandArithmetic::runScript: invalid METHOD " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  Data *line = getArithmetic(in, offset, in2, offset2, method);
  if (! line)
  {
    qDebug() << "CommandArithmetic::runScript: getArithmetic error " << name;
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(name, line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

Data * CommandArithmetic::getArithmetic (Data *in, int off, Data *in2, int off2, int method)
{
  QList<Data *> list;
  list << in << in2;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
  {
    qDebug() << "CommandArithmetic::runScript: getArithmetic invalid keys";
    return 0;
  }

  Data *line = new CurveData;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    double v = 0;
    if (vdi.curveValue(in, keys, loop, off, v))
      continue;

    double v2 = 0;
    if (vdi.curveValue(in2, keys, loop, off2, v2))
      continue;

    switch (method)
    {
      case 0: // add
      {
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, new SettingDouble(v + v2));
        line->set(keys.at(loop), b);
	break;
      }
      case 1: // div
      {
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, new SettingDouble(v / v2));
        line->set(keys.at(loop), b);
	break;
      }
      case 2: // mult
      {
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, new SettingDouble(v * v2));
        line->set(keys.at(loop), b);
	break;
      }
      case 3: // sub
      {
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, new SettingDouble(v - v2));
        line->set(keys.at(loop), b);
	break;
      }
      default:
	break;
    }
  }

  return line;
}
