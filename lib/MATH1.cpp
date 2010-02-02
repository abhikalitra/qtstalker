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

#include "MATH1.h"
#include "math.h" // for fabs

#include <QtDebug>


MATH1::MATH1 ()
{
  indicator = "MATH1";

  methodList << QObject::tr("CEIL");
  methodList << QObject::tr("EXP");
  methodList << QObject::tr("FLOOR");
  methodList << QObject::tr("LN");
  methodList << QObject::tr("LOG10");
  methodList << QObject::tr("NORMALIZE");
  methodList << QObject::tr("SQRT");
}

int MATH1::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MATH1,<NAME>,<INPUT>,<METHOD>

  if (set.count() != 5)
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

  int method = methodList.indexOf(set[4]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[4];
    return 1;
  }

  PlotLine *line = getMATH1(in, method);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * MATH1::getMATH1 (PlotLine *in, int method)
{
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_CEIL(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 1:
      rc = TA_EXP(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 2:
      rc = TA_FLOOR(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 3:
      rc = TA_LN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 4:
      rc = TA_LOG10(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 5:
    {
      int loop = 0;
      double range = 0;
      double max = -99999999.0;
      double min = 99999999.0;
      for (loop = 0; loop < in->getSize(); loop++)
      {
        if (in->getData(loop) > max)
          max = in->getData(loop);
        if (in->getData(loop) < min)
          min = in->getData(loop);
      }
      range = fabs(max) + fabs(min);
      for (loop = 0; loop < in->getSize(); loop++)
        out[loop] = (TA_Real) ((in->getData(loop) - min) / range) * 100;
      break;
    }
    case 6:
      rc = TA_SQRT(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  return line;
}

