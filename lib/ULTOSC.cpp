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

#include "ULTOSC.h"
#include "ta_libc.h"

#include <QtDebug>


ULTOSC::ULTOSC ()
{
}

int ULTOSC::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,ULTOSC,<NAME>,<PERIOD_1>,<PERIOD_2>,<PERIOD_3>

  if (set.count() != 6)
  {
    qDebug() << "ULTOSC::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  bool ok;
  int fp = set[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ULTOSC::calculate: invalid fast period" << set[3];
    return 1;
  }

  int mp = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ULTOSC::calculate: invalid med period" << set[4];
    return 1;
  }

  int sp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ULTOSC::calculate: invalid slow period" << set[5];
    return 1;
  }

  int size = data->count();

  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_ULTOSC(0, size - 1, &high[0], &low[0], &close[0], fp, mp, sp, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ULTOSC::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(set[2], line);

  return 0;
}

