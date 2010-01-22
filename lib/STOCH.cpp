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

#include "STOCH.h"
#include "ta_libc.h"

#include <QtDebug>


STOCH::STOCH ()
{
}

int STOCH::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, QStringList &maList, BarData *data)
{
  // INDICATOR,STOCH,<NAME_SLOWK>,<NAME_SLOWD>,<FASTK_PERIOD>,<SLOWK_PERIOD>,<SLOWK_MA_TYPE>,
  // <SLOWD_PERIOD>,<SLOWD_MA_TYPE>

  if (set.count() != 9)
  {
    qDebug() << "STOCH::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int fkp = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCH::calculate: invalid fastk period" << set[4];
    return 1;
  }

  int skp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCH::calculate: invalid slowk period" << set[5];
    return 1;
  }

  int kma = maList.indexOf(set[6]);
  if (kma == -1)
  {
    qDebug() << "STOCH::calculate: invalid slowk ma" << set[6];
    return 1;
  }

  int sdp = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCH::calculate: invalid slowd period" << set[7];
    return 1;
  }

  int dma = maList.indexOf(set[8]);
  if (dma == -1)
  {
    qDebug() << "STOCH::calculate: invalid slowd ma" << set[8];
    return 1;
  }

  int size = data->count();

  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_STOCH(0, size - 1, &high[0], &low[0], &close[0], fkp, skp, (TA_MAType) kma, sdp, (TA_MAType) dma, &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "STOCH::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *line2 = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    line->append(out[loop]);
    line2->append(out2[loop]);
  }

  tlines.insert(set[2], line);
  tlines.insert(set[3], line2);

  return 0;
}

int STOCH::calculate2 (QStringList &set, QHash<QString, PlotLine *> &tlines, QStringList &maList, BarData *data)
{
  // INDICATOR,STOCHF,<NAME_FASTK>,<NAME_FASTD>,<FASTK_PERIOD>,<FASTD_PERIOD>,<FASTD_MA_TYPE>

  if (set.count() != 7)
  {
    qDebug() << "STOCHF::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int fkp = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCHF::calculate: invalid fastk period" << set[4];
    return 1;
  }

  int fdp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCHF::calculate: invalid fastd period" << set[5];
    return 1;
  }

  int ma = maList.indexOf(set[6]);
  if (ma == -1)
  {
    qDebug() << "STOCH::calculate: invalid ma" << set[6];
    return 1;
  }

  int size = data->count();

  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_STOCHF(0, size - 1, &high[0], &low[0], &close[0], fkp, fdp, (TA_MAType) ma, &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "STOCHF::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *line2 = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    line->append(out[loop]);
    line2->append(out2[loop]);
  }

  tlines.insert(set[2], line);
  tlines.insert(set[3], line2);

  return 0;
}

int STOCH::calculate3 (QStringList &set, QHash<QString, PlotLine *> &tlines, QStringList &maList)
{
  // INDICATOR,STOCHRSI,<INPUT>,<NAME_FASTK>,<NAME_FASTD>,<PERIOD>,<FASTK_PERIOD>,<FASTD_PERIOD>,
  // <FASTD_MA_TYPE>

  if (set.count() != 9)
  {
    qDebug() << "STOCHRSI::calculate: invalid parm count" << set.count();
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
    qDebug() << "STOCHRSI::calculate: input error" << set[2];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCHRSI::calculate: invalid period" << set[5];
    return 1;
  }

  int fkp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCHRSI::calculate: invalid fastkk period" << set[6];
    return 1;
  }

  int fdp = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCHRSI::calculate: invalid fastd period" << set[7];
    return 1;
  }

  int ma = maList.indexOf(set[8]);
  if (ma == -1)
  {
    qDebug() << "STOCHRSI::calculate: invalid ma parm" << set[8];
    return 1;
  }

  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  TA_Real out2[in->getSize()];
  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_STOCHRSI(0, in->getSize() - 1, &input[0], period, fkp, fdp, (TA_MAType) ma, &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "STOCHRSI::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *line2 = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    line->append(out[loop]);
    line2->append(out2[loop]);
  }

  tlines.insert(set[3], line);
  tlines.insert(set[4], line2);

  return 0;
}

