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

#include "SAR.h"
#include "ta_libc.h"

#include <QtDebug>


SAR::SAR ()
{
}

int SAR::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,SAR,<NAME>,<ACCELERATION>,<MAXIMUM>

  if (set.count() != 5)
  {
    qDebug() << "SAR::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  bool ok;
  double accel = set[3].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAR::calculate: invalid acceleration" << set[3];
    return 1;
  }

  double max = set[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAR::calculate: invalid maximum" << set[4];
    return 1;
  }

  int size = data->count();

  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_SAR(0, size - 1, &high[0], &low[0], accel, max, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "SAR::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(set[2], line);

  return 0;
}

int SAR::calculate2 (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,SAREXT,<NAME>,<START>,<OFFSET_REVERSE>,<ACCEL_INIT_LONG>,<ACCEL_LONG>,<ACCEL_MAX_LONG>,
  // <ACCEL_INIT_SHORT>,<ACCEL_SHORT>,<ACCEL_MAX_SHORT>

  if (set.count() != 11)
  {
    qDebug() << "SAREXT::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  bool ok;
  double start = set[3].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAREXT::calculate: invalid start" << set[3];
    return 1;
  }

  double offrev = set[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAREXT::calculate: invalid offset reverse" << set[4];
    return 1;
  }

  double ail = set[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAREXT::calculate: invalid accel init long" << set[5];
    return 1;
  }

  double al = set[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAREXT::calculate: invalid accel long" << set[6];
    return 1;
  }

  double aml = set[7].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAREXT::calculate: invalid accel max long" << set[7];
    return 1;
  }

  double ais = set[8].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAREXT::calculate: invalid accel init short" << set[8];
    return 1;
  }

  double as = set[9].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAREXT::calculate: invalid accel short" << set[9];
    return 1;
  }

  double ams = set[10].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAREXT::calculate: invalid accel max short" << set[10];
    return 1;
  }

  int size = data->count();

  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_SAREXT(0, size - 1, &high[0], &low[0], start, offrev, ail, al, aml,
			    ais, as, ams, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "SAREXT::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(set[2], line);

  return 0;
}

