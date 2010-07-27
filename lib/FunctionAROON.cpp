/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "FunctionAROON.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionAROON::FunctionAROON ()
{
  _methodList << "AROON" << "OSC";
}

int FunctionAROON::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,AROON,<METHOD>,*
  //     0       1      2      3

  int method = _methodList.indexOf(set[3]);

  int rc = 1;
  switch ((Method) method)
  {
    case _AROON:
      rc = scriptAROON(set, ind, data);
      break;
    case _AROONOSC:
      rc = scriptAROONOSC(set, ind, data);
      break;
    default:
      break;
  }

  return rc;
}

int FunctionAROON::scriptAROON (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,AROON,AROON,<UPPER NAME>,<LOWER NAME>,<PERIOD>
  //     0       1      2     3         4           5          6

  if (set.count() != 7)
  {
    qDebug() << "FunctionAROON::scriptAROON: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "FunctionAROON::scriptAROON: duplicate upper name" << set[4];
    return 1;
  }

  tl = ind.line(set[5]);
  if (tl)
  {
    qDebug() << "FunctionAROON::scriptAROON: duplicate lower name" << set[5];
    return 1;
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionAROON::scriptAROON: invalid period" << set[6];
    return 1;
  }

  QList<PlotLine *> pl;
  if (getAROON(data, period, pl))
    return 1;

  pl.at(0)->setLabel(set[4]);
  pl.at(1)->setLabel(set[5]);
  
  ind.setLine(set[4], pl.at(0));
  ind.setLine(set[5], pl.at(1));

  return 0;
}

int FunctionAROON::scriptAROONOSC (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,AROON,AROONOSC,<NAME>,<PERIOD>
  //     0       1      2       3       4      5

  if (set.count() != 6)
  {
    qDebug() << "FunctionAROONOSC::scriptAROONOSC: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "FunctionAROONOSC::scriptAROONOSC: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionAROONOSC::scriptAROONOSC: invalid period" << set[5];
    return 1;
  }

  PlotLine *line = getAROONOSC(data, period);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}


int FunctionAROON::getAROON (BarData &data, int period, QList<PlotLine *> &pl)
{
  int size = data.count();
  if (size < period)
    return 1;
  
  TA_Real high[size];
  TA_Real low[size];
  TA_Real dout[size];
  TA_Real uout[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar bar = data.getBar(loop);
    high[loop] = (TA_Real) bar.getHigh();
    low[loop] = (TA_Real) bar.getLow();
  }

  TA_RetCode rc = TA_AROON(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           period,
                           &outBeg,
                           &outNb,
                           &dout[0],
                           &uout[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionAROON::getAROON: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *upper = new PlotLine;
  PlotLine *lower = new PlotLine;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    upper->setData(dataLoop, uout[outLoop]);
    lower->setData(dataLoop, dout[outLoop]);

    dataLoop--;
    outLoop--;
  }

  pl.append(upper);
  pl.append(lower);

  return 0;
}

PlotLine * FunctionAROON::getAROONOSC (BarData &data, int period)
{
  if (data.count() < period)
    return 0;

  int size = data.count();

  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar bar = data.getBar(loop);
    high[loop] = (TA_Real) bar.getHigh();
    low[loop] = (TA_Real) bar.getLow();
  }

  TA_RetCode rc = TA_AROONOSC(0,
                              size - 1,
                              &high[0],
                              &low[0],
                              period,
                              &outBeg,
                              &outNb,
                              &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionAROON::getAROONOSC: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setData(dataLoop, out[outLoop]);
    dataLoop--;
    outLoop--;
  }

  return line;
}

QStringList & FunctionAROON::list ()
{
  return _methodList;
}

