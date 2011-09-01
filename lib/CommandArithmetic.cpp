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
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandArithmetic::CommandArithmetic (QObject *p) : Command (p)
{
  _type = "ARITHMETIC";
  _method << "ADD" << "DIV" << "MULT" << "SUB";
}

int CommandArithmetic::runScript (Data *sg, Script *script)
{
  // verify NAME
  QString name = sg->get("OUTPUT");
  Data *line = script->data(name);
  if (line)
  {
    message(script->session(), _type, "::runScript: duplicate OUTPUT", name);
    return _ERROR;
  }

  // verify INPUT_1
  QString s = sg->get("INPUT_1");
  Data *in = script->data(s);
  if (! in)
  {
    message(script->session(), _type, "::runScript: invalid INPUT_1", s);
    return _ERROR;
  }

  // verify INPUT_2
  Data *in2 = 0;
  int delFlag = 0;
  s = sg->get("INPUT_2");
  bool ok;
  double value = s.toDouble(&ok);
  if (ok)
  {
    in2 = new CurveData;
    QList<int> keys = in->barKeys();
    int loop = 0;
    for (; loop < keys.count(); loop++)
    {
      Data *b = new CurveBar;
      b->set(CurveBar::_VALUE, value);
      in2->set(keys.at(loop), b);
    }
    delFlag++;
  }
  else
  {
    in2 = script->data(s);
    if (! in2)
    {
      message(script->session(), _type, "::runScript: invalid INPUT_2", s);
      return _ERROR;
    }
  }

  s = sg->get("METHOD");
  int method = _method.indexOf(s);
  if (method == -1)
  {
    message(script->session(), _type, "::runScript: invalid METHOD", s);
    return _ERROR;
  }

  line = getArithmetic(in, in2, method);
  if (! line)
  {
    message(script->session(), _type, "::runScript: getArithmetic error", name);
    if (delFlag)
      delete in2;
    return _ERROR;
  }

  if (delFlag)
    delete in2;

  script->setData(name, line);

  return _OK;
}

Data * CommandArithmetic::getArithmetic (Data *in, Data *in2, int method)
{
  QList<Data *> list;
  list << in << in2;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
  {
    message(QString(), _type, "::getArithmetic", "invalid keys");
    return 0;
  }

  Data *line = new CurveData;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *bar = in->getData(keys.at(loop));
    if (! bar)
      continue;

    Data *bar2 = in2->getData(keys.at(loop));
    if (! bar2)
      continue;

    switch (method)
    {
      case 0: // add
      {
        double v = bar->getDouble(CurveBar::_VALUE) + bar2->getDouble(CurveBar::_VALUE);
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, v);
        line->set(keys.at(loop), b);
	break;
      }
      case 1: // div
      {
        double v = bar->getDouble(CurveBar::_VALUE) / bar2->getDouble(CurveBar::_VALUE);
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, v);
        line->set(keys.at(loop), b);
	break;
      }
      case 2: // mult
      {
        double v = bar->getDouble(CurveBar::_VALUE) * bar2->getDouble(CurveBar::_VALUE);
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, v);
        line->set(keys.at(loop), b);
	break;
      }
      case 3: // sub
      {
        double v = bar->getDouble(CurveBar::_VALUE) - bar2->getDouble(CurveBar::_VALUE);
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, v);
        line->set(keys.at(loop), b);
	break;
      }
      default:
	break;
    }
  }

  return line;
}

Data * CommandArithmetic::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QString());
  sg->set("METHOD", QString());
  sg->set("INPUT_1", QString());
  sg->set("INPUT_2", QString());
  return sg;
}
