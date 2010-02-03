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
  methodList << QObject::tr("ACOS");
  methodList << QObject::tr("ASIN");
  methodList << QObject::tr("ATAN");
  methodList << QObject::tr("COS");
  methodList << QObject::tr("COSH");
  methodList << QObject::tr("SIN");
  methodList << QObject::tr("SINH");
  methodList << QObject::tr("TAN");
  methodList << QObject::tr("TANH");
  methodList << QObject::tr("ADD");
  methodList << QObject::tr("DIV");
  methodList << QObject::tr("MULT");
  methodList << QObject::tr("SUB");
  methodList << QObject::tr("STDDEV");
}

int MATH1::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  int rc = 1;
  if (set.count() == 5)
    rc = getCUS2(set, tlines, data);

  if (set.count() == 6)
    rc = getCUS3(set, tlines, data);

  // only for STDDEV
  if (set.count() == 7)
    rc = getCUS4(set, tlines, data);

  return rc;
}

int MATH1::getCUS2 (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MATH1,METHOD,<NAME>,<INPUT>

  if (set.count() != 5)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[2]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[2];
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[3];
    return 1;
  }

  PlotLine *in = tlines.value(set[4]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[4]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in);
  }

  PlotLine *line = getMATH1(in, method);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

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
    case 7:
      rc = TA_ACOS(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 8:
      rc = TA_ASIN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 9:
      rc = TA_ATAN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 10:
      rc = TA_COS(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 11:
      rc = TA_COSH(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 12:
      rc = TA_SIN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 13:
      rc = TA_SINH(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 14:
      rc = TA_TAN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 15:
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

int MATH1::getCUS3 (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MATH1,METHOD,<NAME>,<INPUT>,<INPUT2>

  if (set.count() != 6)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[2]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[2];
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[3];
    return 1;
  }

  PlotLine *in = tlines.value(set[4]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[4]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in);
  }

  PlotLine *in2 = tlines.value(set[5]);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(set[5]));
    if (! in2)
    {
      qDebug() << indicator << "::calculate: input2 not found" << set[5];
      return 1;
    }

    tlines.insert(set[5], in2);
  }

  PlotLine *line = getMATH2(in, in2, method);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * MATH1::getMATH2 (PlotLine *in, PlotLine *in2, int method)
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

int MATH1::getCUS4 (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MATH1,METHOD,<NAME>,<INPUT>,<PERIOD>,<DEVIATION>

  if (set.count() != 7)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[2]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[2];
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[3];
    return 1;
  }

  PlotLine *in = tlines.value(set[4]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[4]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period" << set[5];
    return 1;
  }

  double dev = set[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid deviation" << set[6];
    return 1;
  }

  PlotLine *line = getSTDDEV(in, period, dev);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * MATH1::getSTDDEV (PlotLine *in, int period, double dev)
{
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_STDDEV(0, in->getSize() - 1, &input[0], period, dev, &outBeg, &outNb, &out[0]);
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

