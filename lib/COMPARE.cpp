/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

  methodList << "ARRAY";
  methodList << "VALUE";
}

int COMPARE::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  if (set.count() != 7)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[7]);

  int rc = 0;
  switch (method)
  {
    case 0:
      rc = getCOMPARE(set, tlines, data);
      break;
    case 1:
      rc = getCOMPARE2(set, tlines, data);
      break;
    default:
      break;
  }

  return rc;
}

int COMPARE::getCOMPARE (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,COMPARE,<NAME>,<INPUT_1>,<INPUT_2>,<OPERATOR>,<METHOD>

  if (set.count() != 7)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[2];
    return 1;
  }

  PlotLine *in = tlines.value(set[3]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[3]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[3];
      return 1;
    }

    tlines.insert(set[3], in);
  }

  PlotLine *in2 = tlines.value(set[4]);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(set[4]));
    if (! in2)
    {
      qDebug() << indicator << "::calculate: input2 not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in2);
  }

  int op = opList.indexOf(set[5]);
  if (op == -1)
  {
    qDebug() << indicator << "::calculate: invalid operator" << set[5];
    return 1;
  }

  int loop = in->getSize() - 1;
  int loop2 = in2->getSize() - 1;
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
      case 5: // AND
        if (t && t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 6: // OR
        if (t || t2)
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

  tlines.insert(set[2], line);

  return 0;
}

int COMPARE::getCOMPARE2 (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,COMPARE2,<NAME>,<INPUT>,<VALUE>,<OPERATOR>,<METHOD>

  if (set.count() != 7)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[2];
    return 1;
  }

  PlotLine *in = tlines.value(set[3]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[3]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[3];
      return 1;
    }

    tlines.insert(set[3], in);
  }

  bool ok;
  double value = set[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid input2" << set[4];
    return 1;
  }

  int op = opList.indexOf(set[5]);
  if (op == -1)
  {
    qDebug() << indicator << "::calculate: invalid operator" << set[5];
    return 1;
  }

  int loop = in->getSize() - 1;
  PlotLine *line = new PlotLine;

  while (loop > -1)
  {
    double t = in->getData(loop);

    switch (op)
    {
      case 0: // equal
        if (t == value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 1: // less then
        if (t < value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 2: // less than equal
        if (t <= value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 3: // greater than
        if (t > value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 4: // greater than equal
        if (t >= value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 5: // AND
        if (t && value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 6: // OR
        if (t || value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      default:
        break;
    }

    loop--;
  }

  tlines.insert(set[2], line);

  return 0;
}

