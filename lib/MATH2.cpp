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

#include "MATH2.h"
#include "math.h" // for fabs

#include <QtDebug>


MATH2::MATH2 ()
{
  indicator = "MATH2";

  methodList << QObject::tr("ADD");
  methodList << QObject::tr("DIV");
  methodList << QObject::tr("MULT");
  methodList << QObject::tr("SUB");
}

int MATH2::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MATH2,<NAME>,<INPUT>,<INPUT2>,<METHOD>

  if (set.count() != 6)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
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

  int method = methodList.indexOf(set[5]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[5];
    return 1;
  }

  PlotLine *line = getMATH2(in, in2, method);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * MATH2::getMATH2 (PlotLine *in, PlotLine *in2, int method)
{
  int size = in->getSize();
  if (in2->getSize() < size)
    size = in2->getSize();

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = in->getSize() - 1;
  int loop2 = in2->getSize() - 1;
  int count = size - 1;
  for (; count > -1; loop--, loop2--, count--)
  {
    input[loop] = (TA_Real) in->getData(loop);
    input2[loop2] = (TA_Real) in2->getData(loop2);
  }

  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_ADD(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    case 1:
      rc = TA_DIV(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    case 2:
      rc = TA_MULT(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    case 3:
      rc = TA_SUB(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  return line;
}

