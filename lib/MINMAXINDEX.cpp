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

#include "MINMAXINDEX.h"
#include "ta_libc.h"

#include <QtDebug>


MINMAXINDEX::MINMAXINDEX ()
{
}

int MINMAXINDEX::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MINMAXINDEX,<INPUT>,<NAME_MIN>,<NAME_MAX>,<PERIOD>

  if (set.count() != 6)
  {
    qDebug() << "MINMAXINDEX::calculate: invalid parm count" << set.count();
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
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MINMAXINDEX::calculate: invalid period parm" << set[5];
    return 1;
  }

  TA_Real input[in->getSize()];
  TA_Integer out[in->getSize()];
  TA_Integer out2[in->getSize()];
  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_MINMAXINDEX(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MINMAXINDEX::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *min = new PlotLine;
  PlotLine *max = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    min->append(out[loop]);
    max->append(out2[loop]);
  }

  tlines.insert(set[3], min);
  tlines.insert(set[4], max);

  return 0;
}

