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

#include "MAVP.h"
#include "ta_libc.h"

#include <QtDebug>


MAVP::MAVP ()
{
}

int MAVP::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, QStringList &maList, BarData *data)
{
  // INDICATOR,MAVP,<NAME>,<INPUT_1>,<INPUT_2>,<MIN_PERIOD>,<MAX_PERIOD>,<MA_TYPE>

  if (set.count() != 8)
  {
    qDebug() << "MAVP::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  PlotLine *in = tlines.value(set[3]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[3]));
    if (! in)
    {
      qDebug() << set[1] << "::calculate: input not found" << set[3];
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
      qDebug() << set[1] << "::calculate: input not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in2);
  }

  bool ok;
  int min = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MAVP::calculate: invalid min period parm" << set[5];
    return 1;
  }

  int max = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MAVP::calculate: invalid max period parm" << set[6];
    return 1;
  }

  int ma = maList.indexOf(set[7]);
  if (ma == -1)
  {
    qDebug() << "MAVP::calculate: invalid ma parm" << set[7];
    return 1;
  }

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

  TA_RetCode rc = TA_MAVP(0, size - 1, &input[0], &input2[0], min, max, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MAVP::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(set[2], line);

  return 0;
}

