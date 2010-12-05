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

#include "PP.h"
#include "Curve.h"

#include <QtDebug>

PP::PP ()
{
  _method << "R1" << "R2" << "R3" << "S1" << "S2" << "S3";
}

int PP::command (Command &command)
{
  // PP,<METHOD>,<NAME>
  //  0    1       2

  if (command.count() != 3)
  {
    qDebug() << "PP::command: invalid settings count" << command.count();
    return 1;
  }

  BarData *data = command.barData();
  if (! data)
  {
    qDebug() << "PP::command: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "PP::command: no indicator";
    return 1;
  }

  int pos = 1;
  int method = _method.indexOf(command.parm(pos));
  if (method == -1)
  {
    qDebug() << "PP::command: invalid method" << command.parm(pos);
    return 1;
  }

  pos++;
  QString name = command.parm(pos);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "PP::command: duplicate name" << name;
    return 1;
  }

  line = new Curve(Curve::Horizontal);

  Bar *bar = data->bar(data->count() - 1);
  double high = bar->high();
  double low = bar->low();
  double close = bar->close();
  double pp = 0;
  double t = 0;

  switch ((Method) method)
  {
    case _R1: // first resistance
    {
      pp = (high + low + close) / 3;
      t = (2 * pp) - low;
      line->setBar(0, new CurveBar(t));
      break;
    }
    case _R2: // second resistance
    {
      pp = (high + low + close) / 3;
      t = pp + (high - low);
      line->setBar(0, new CurveBar(t));
      break;
    }
    case _R3: // third resistance
    {
      pp = (high + low + close) / 3;
      t = (2 * pp) + (high - (2 * low));
      line->setBar(0, new CurveBar(t));
      break;
    }
    case _S1: // first support
    {
      pp = (high + low + close) / 3;
      t = (2 * pp) - high;
      line->setBar(0, new CurveBar(t));
      break;
    }
    case _S2: // second support
    {
      pp = (high + low + close) / 3;
      t = pp - (high - low);
      line->setBar(0, new CurveBar(t));
      break;
    }
    case _S3: // third support
    {
      pp = (high + low + close) / 3;
      t = (2 * pp) - ((2 * high) - low);
      line->setBar(0, new CurveBar(t));
      break;
    }
    default:
      break;
  }

  line->setLabel(name);
  i->setLine(name, line);

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  PP *o = new PP;
  return ((ScriptPlugin *) o);
}
