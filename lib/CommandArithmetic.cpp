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
  QString name = sg->get("OUTPUT").toString();
  Data *line = script->data(name);
  if (line)
  {
    _message << "duplicate OUTPUT " + name;
    return _ERROR;
  }

  // verify INPUT_1
  QString s = sg->get("INPUT_1").toString();
  Data *in = script->data(s);
  if (! in)
  {
    _message << "invalid INPUT_1 " + s;
    return _ERROR;
  }

  // verify INPUT_2
  Data *in2 = 0;
  int delFlag = 0;
  s = sg->get("INPUT_2").toString();
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
      b->set(CurveBar::_VALUE, QVariant(value));
      in2->set(keys.at(loop), b);
    }
    delFlag++;
  }
  else
  {
    in2 = script->data(s);
    if (! in2)
    {
      _message << "invalid INPUT_2 " + s;
      return _ERROR;
    }
  }

  s = sg->get("METHOD").toString();
  int method = _method.indexOf(s);
  if (method == -1)
  {
    _message << "invalid METHOD " + s;
    return _ERROR;
  }

  line = getArithmetic(in, in2, method);
  if (! line)
  {
    _message << "getArithmetic error " + name;
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
    _message << "getArithmetic invalid keys";
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
        double v = bar->get(CurveBar::_VALUE).toDouble() + bar2->get(CurveBar::_VALUE).toDouble();
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(v));
        line->set(keys.at(loop), b);
	break;
      }
      case 1: // div
      {
        double v = bar->get(CurveBar::_VALUE).toDouble() / bar2->get(CurveBar::_VALUE).toDouble();
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(v));
        line->set(keys.at(loop), b);
	break;
      }
      case 2: // mult
      {
        double v = bar->get(CurveBar::_VALUE).toDouble() * bar2->get(CurveBar::_VALUE).toDouble();
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(v));
        line->set(keys.at(loop), b);
	break;
      }
      case 3: // sub
      {
        double v = bar->get(CurveBar::_VALUE).toDouble() - bar2->get(CurveBar::_VALUE).toDouble();
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(v));
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
  sg->set("OUTPUT", QVariant(QString()));
  sg->set("METHOD", QVariant(QString()));
  sg->set("INPUT_1", QVariant(QString()));
  sg->set("INPUT_2", QVariant(QString()));
  return sg;
}
