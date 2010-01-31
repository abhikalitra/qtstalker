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

#include "TRIG.h"

#include <QtDebug>


TRIG::TRIG ()
{
  indicator = "TRIG";

  methodList << QObject::tr("ACOS");
  methodList << QObject::tr("ASIN");
  methodList << QObject::tr("ATAN");
  methodList << QObject::tr("COS");
  methodList << QObject::tr("COSH");
  methodList << QObject::tr("SIN");
  methodList << QObject::tr("SINH");
  methodList << QObject::tr("TAN");
  methodList << QObject::tr("TANH");
}

int TRIG::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,TRIG,<NAME>,<INPUT>,<METHOD>

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

  PlotLine *line = getTRIG(in, method);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * TRIG::getTRIG (PlotLine *in, int method)
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
      rc = TA_ACOS(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 1:
      rc = TA_ASIN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 2:
      rc = TA_ATAN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 3:
      rc = TA_COS(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 4:
      rc = TA_COSH(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 5:
      rc = TA_SIN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 6:
      rc = TA_SINH(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 7:
      rc = TA_TAN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 8:
      rc = TA_TANH(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
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

