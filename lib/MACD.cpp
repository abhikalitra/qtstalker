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

#include "MACD.h"
#include "ta_libc.h"

#include <QtDebug>


MACD::MACD ()
{
}

int MACD::calculate1 (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MACD,<INPUT>,<NAME_MACD>,<NAME_SIGNAL>,<NAME_HIST>,<FAST_PERIOD>,<SLOW_PERIOD>,<SIGNAL_PERIOD>

  if (set.count() != 9)
  {
    qDebug() << "MACD::calculate: invalid parm count" << set.count();
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
  int fast = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACD::calculate: invalid fast period" << set[6];
    return 1;
  }

  int slow = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACD::calculate: invalid slow period" << set[7];
    return 1;
  }

  int signal = set[8].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACD::calculate: invalid signal period" << set[8];
    return 1;
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  TA_Real out2[in->getSize()];
  TA_Real out3[in->getSize()];
  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_MACD(0, in->getSize() - 1, &input[0], fast, slow, signal, &outBeg, &outNb, &out[0], &out2[0], &out3[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MACD::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *sig = new PlotLine;
  PlotLine *hist = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    line->append(out[loop]);
    sig->append(out2[loop]);
    hist->append(out3[loop]);
  }

  tlines.insert(set[3], line);
  tlines.insert(set[4], sig);
  tlines.insert(set[5], hist);

  return 0;
}

int MACD::calculate2 (QStringList &set, QHash<QString, PlotLine *> &tlines, QStringList &maList, BarData *data)
{
  // INDICATOR,MACDEXT,<INPUT>,<NAME_MACD>,<NAME_SIGNAL>,<NAME_HIST>,<FAST_PERIOD>,<FAST_MA_TYPE>,
  // <SLOW_PERIOD>,<SLOW_MA_TYPE>,<SIGNAL_PERIOD>,<SIGNAL_MA_TYPE>

  if (set.count() != 12)
  {
    qDebug() << "MACDEXT::calculate: invalid parm count" << set.count();
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
  int fast = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACDEXT::calculate: invalid fast period" << set[6];
    return 1;
  }

  int fastma = maList.indexOf(set[7]);
  if (fastma == -1)
  {
    qDebug() << "MACDEXT::calculate: invalid fast ma" << set[7];
    return 1;
  }

  int slow = set[8].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACDEXT::calculate: invalid slow period" << set[8];
    return 1;
  }

  int slowma = maList.indexOf(set[9]);
  if (slowma == -1)
  {
    qDebug() << "MACDEXT::calculate: invalid slow ma" << set[9];
    return 1;
  }

  int signal = set[10].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACDEXT::calculate: invalid signal period" << set[9];
    return 1;
  }

  int signalma = maList.indexOf(set[11]);
  if (signalma == -1)
  {
    qDebug() << "MACDEXT::calculate: invalid fast ma" << set[11];
    return 1;
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  TA_Real out2[in->getSize()];
  TA_Real out3[in->getSize()];
  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_MACDEXT(0, in->getSize() - 1, &input[0], fast, (TA_MAType) fastma,
			     slow, (TA_MAType) slowma, signal, (TA_MAType) signalma,
			     &outBeg, &outNb, &out[0], &out2[0], &out3[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MACDEXT::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *sig = new PlotLine;
  PlotLine *hist = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    line->append(out[loop]);
    sig->append(out2[loop]);
    hist->append(out3[loop]);
  }

  tlines.insert(set[3], line);
  tlines.insert(set[4], sig);
  tlines.insert(set[5], hist);

  return 0;
}

int MACD::calculate3 (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MACDFIX,<INPUT>,<NAME_MACD>,<NAME_SIGNAL>,<NAME_HIST>,<SIGNAL_PERIOD>

  if (set.count() != 7)
  {
    qDebug() << "MACDFIX::calculate: invalid parm count" << set.count();
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
    qDebug() << "MACDFIX::calculate: invalid period" << set[6];
    return 1;
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  TA_Real out2[in->getSize()];
  TA_Real out3[in->getSize()];
  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_MACDFIX(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0], &out2[0], &out3[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MACDFIX::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *sig = new PlotLine;
  PlotLine *hist = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    line->append(out[loop]);
    sig->append(out2[loop]);
    hist->append(out3[loop]);
  }

  tlines.insert(set[3], line);
  tlines.insert(set[4], sig);
  tlines.insert(set[5], hist);

  return 0;
}

