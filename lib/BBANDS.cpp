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

#include "BBANDS.h"
#include "ta_libc.h"

#include <QtDebug>


BBANDS::BBANDS ()
{
}

int BBANDS::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, QStringList &maList, BarData *data)
{
  // INDICATOR,BBANDS,<INPUT>,<NAME_UPPER>,<NAME_MIDDLE>,<NAME_LOWER>,<PERIOD>,<UPPER_DEVIATION>,<LOWER_DEVIATION>,<MA_TYPE>

  if (set.count() != 10)
  {
    qDebug() << "BBANDS::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[3];
    return 1;
  }

  tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[4];
    return 1;
  }

  tl = tlines.value(set[5]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[5];
    return 1;
  }

  PlotLine *in = tlines.value(set[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[2]));
    if (! in)
    {
      qDebug() << set[1] << "::calculate: input not found" << set[2];
      return 1;
    }

    tlines.insert(set[2], in);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "BBANDS::calculate: invalid period parm" << set[6];
    return 1;
  }

  double upDev = set[7].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "BBANDS::calculate: invalid up deviation parm" << set[7];
    return 1;
  }

  double lowDev = set[8].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "BBANDS::calculate: invalid lower deviation parm" << set[8];
    return 1;
  }

  int ma = maList.indexOf(set[9]);
  if (ma == -1)
  {
    qDebug() << "BBANDS::calculate: invalid ma parm" << set[9];
    return 1;
  }

  TA_Real input[in->getSize()];
  TA_Real upper[in->getSize()];
  TA_Real middle[in->getSize()];
  TA_Real lower[in->getSize()];
  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_BBANDS(0, in->getSize() - 1, &input[0], period, upDev, lowDev, (TA_MAType) ma, &outBeg, &outNb, &upper[0], &middle[0], &lower[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "BBANDS::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *u = new PlotLine;
  PlotLine *m = new PlotLine;
  PlotLine *l = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    u->append(upper[loop]);
    m->append(middle[loop]);
    l->append(lower[loop]);
  }

  tlines.insert(set[3], u);
  tlines.insert(set[4], m);
  tlines.insert(set[5], l);

  return 0;
}

