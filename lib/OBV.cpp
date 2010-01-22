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

#include "OBV.h"
#include "ta_libc.h"

#include <QtDebug>


OBV::OBV ()
{
}

int OBV::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,OBV,<NAME>,<INPUT>

  if (set.count() != 4)
  {
    qDebug() << "OBV::calculate: invalid parm count" << set.count();
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
    qDebug() << "OBV::calculate: no input" << set[3];
    return 1;
  }

  int size = in->getSize();
  if (data->count() < size)
    size = data->count();

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;
  int loop = in->getSize() - 1;
  int loop2 = data->count() - 1;
  int count = size - 1;
  for (; count > -1; loop--, loop2--, count--)
  {
    input[loop] = (TA_Real) in->getData(loop);
    input2[loop2] = (TA_Real) data->getVolume(loop2);
  }

  TA_RetCode rc = TA_OBV(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "OBV::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(set[2], line);

  return 0;
}

