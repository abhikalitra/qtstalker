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

#include "COMPARE.h"

#include <QtDebug>


COMPARE::COMPARE ()
{
  indicator = "COMPARE";
  deleteFlag = TRUE;
}

int COMPARE::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,COMPARE,<NAME>,<INPUT_1>,<INPUT_2>,<OPERATOR>

  if (set.count() != 7)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[3];
    return 1;
  }

  PlotLine *in = getInput(set[4], tlines, data);
  if (! in)
    return 1;

  PlotLine *in2 = getInput(set[5], tlines, data);
  if (! in2)
    return 1;

  int op = opList.indexOf(set[6]);
  if (op == -1)
  {
    qDebug() << indicator << "::calculate: invalid operator" << set[6];
    return 1;
  }

  PlotLine *line = 0;
  if (in->count() > 1 && in2->count() > 1) // A - A
    line = compareAA(in, in2, op);
  else
  {
    if (in->count() > 1 && in2->count() == 1) // A - V
      line = compareAV(in, in2, op);
    else
    {
      if (in->count() == 1 && in2->count() == 1) // V - V
        line = compareVV(in, in2, op);
      else
      {
        if (in->count() == 1 && in2->count() > 1) // V - A
          line = compareVA(in, in2, op);
	else
	  return 1;
      }
    }
  }

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * COMPARE::compareAA (PlotLine *in, PlotLine *in2, int op)
{
  int loop = in->count() - 1;
  int loop2 = in2->count() - 1;
  PlotLine *line = new PlotLine;

  while (loop > -1 && loop2 > -1)
  {
    double t = in->getData(loop);
    double t2 = in2->getData(loop2);

    switch (op)
    {
      case 0: // equal
        if (t == t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 1: // less then
        if (t < t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 2: // less than equal
        if (t <= t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 3: // greater than
        if (t > t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 4: // greater than equal
        if (t >= t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      default:
        break;
    }

    loop--;
    loop2--;
  }

  return line;
}

PlotLine * COMPARE::compareAV (PlotLine *in, PlotLine *in2, int op)
{
  int loop = in->count() - 1;
  PlotLine *line = new PlotLine;
  double t2 = in2->getData(0);

  while (loop > -1)
  {
    double t = in->getData(loop);

    switch (op)
    {
      case 0: // equal
        if (t == t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 1: // less then
        if (t < t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 2: // less than equal
        if (t <= t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 3: // greater than
        if (t > t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 4: // greater than equal
        if (t >= t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      default:
        break;
    }

    loop--;
  }

  return line;
}

PlotLine * COMPARE::compareVV (PlotLine *in, PlotLine *in2, int op)
{
  PlotLine *line = new PlotLine;

  double t = in->getData(0);
  double t2 = in2->getData(0);

  switch (op)
  {
    case 0: // equal
      if (t == t2)
        line->prepend(1);
      else
        line->prepend(0);
      break;
    case 1: // less then
      if (t < t2)
        line->prepend(1);
      else
        line->prepend(0);
      break;
    case 2: // less than equal
      if (t <= t2)
        line->prepend(1);
      else
        line->prepend(0);
      break;
    case 3: // greater than
      if (t > t2)
        line->prepend(1);
      else
        line->prepend(0);
      break;
    case 4: // greater than equal
      if (t >= t2)
        line->prepend(1);
      else
        line->prepend(0);
      break;
    default:
      break;
  }

  return line;
}

PlotLine * COMPARE::compareVA (PlotLine *in, PlotLine *in2, int op)
{
  int loop = in2->count() - 1;
  PlotLine *line = new PlotLine;
  double t = in->getData(0);

  while (loop > -1)
  {
    double t2 = in2->getData(loop);

    switch (op)
    {
      case 0: // equal
        if (t == t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 1: // less then
        if (t < t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 2: // less than equal
        if (t <= t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 3: // greater than
        if (t > t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 4: // greater than equal
        if (t >= t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      default:
        break;
    }

    loop--;
  }

  return line;
}

PlotLine * COMPARE::getInput (QString &name, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // check if an existing array
  PlotLine *in = tlines.value(name);
  if (! in)
  {
    // check if its a Open, High, Low, Close, Volume etc array
    in = data->getInput(data->getInputType(name));
    if (! in)
    {
      // check if its a value
      bool ok;
      double value = name.toDouble(&ok);
      if (! ok)
      {
        qDebug() << indicator << "::calculate: invalid input" << name;
        return 0;
      }

      // its a number so create a line for it
      in = new PlotLine;
      in->append(value);
    }
  }

  return in;
}

