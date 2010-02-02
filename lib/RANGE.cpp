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

#include "RANGE.h"

#include <QtDebug>


RANGE::RANGE ()
{
  indicator = "RANGE";

  methodList << "MIN";
  methodList << "MININDEX";
  methodList << "MAX";
  methodList << "MAXINDEX";
  methodList << "MIDPOINT";
  methodList << "MIDPRICE";
  methodList << "TRANGE";
}

int RANGE::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,RANGE,<NAME>,<INPUT>,<PERIOD>,<METHOD>

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

    tlines.insert(set[2], in);
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period" << set[4];
    return 1;
  }

  int method = methodList.indexOf(set[5]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[5];
    return 1;
  }

  PlotLine *line = getRANGE(in, data, period, method);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * RANGE::getRANGE (PlotLine *in, BarData *data, int period, int method)
{
  int size = in->getSize();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_MIN(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 1:
    {
      TA_Integer iout[size];
      rc = TA_MININDEX(0, size - 1, &input[0], period, &outBeg, &outNb, &iout[0]);
      if (rc != TA_SUCCESS)
      {
        qDebug() << indicator << "::calculate: TA-Lib error" << rc;
        return 0;
      }
      PlotLine *line = new PlotLine;
      for (loop = 0; loop < outNb; loop++)
        line->append(iout[loop]);
      return line;
      break;
    }
    case 2:
      rc = TA_MAX(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 3:
    {
      TA_Integer iout[size];
      rc = TA_MAXINDEX(0, size - 1, &input[0], period, &outBeg, &outNb, &iout[0]);
      if (rc != TA_SUCCESS)
      {
        qDebug() << indicator << "::calculate: TA-Lib error" << rc;
        return 0;
      }
      PlotLine *line = new PlotLine;
      for (loop = 0; loop < outNb; loop++)
        line->append(iout[loop]);
      return line;
      break;
    }
    case 4:
      rc = TA_MIDPOINT(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 5:
    {
      size = data->count();
      TA_Real high[size];
      TA_Real low[size];
      for (loop = 0; loop < size; loop++)
      {
        high[loop] = (TA_Real) data->getHigh(loop);
        low[loop] = (TA_Real) data->getLow(loop);
      }
      rc = TA_MIDPRICE(0, size - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0]);
      break;
    }
    case 6:
    {
      size = data->count();
      TA_Real high[size];
      TA_Real low[size];
      TA_Real close[size];
      for (loop = 0; loop < size; loop++)
      {
        high[loop] = (TA_Real) data->getHigh(loop);
        low[loop] = (TA_Real) data->getLow(loop);
        close[loop] = (TA_Real) data->getClose(loop);
      }
      rc = TA_TRANGE(0, size - 1, &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    }
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

