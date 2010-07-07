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

#include "MAFactory.h"
#include "ta_libc.h"
#include "Wilder.h"
#include "PlotFactory.h"

#include <QtDebug>

MAFactory::MAFactory ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("MAFactory::error on TA_Initialize");
  
  _maList << "EMA" << "DEMA" << "KAMA" << "SMA" << "TEMA" << "TRIMA" << "Wilder" << "WMA";
}

PlotLine * MAFactory::ma (PlotLine *in, int period, int method, int lineType, QColor &color)
{
  if (in->count() < period)
    return 0;
  
  QList<int> keys;
  in->keys(keys);
  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    PlotLineBar bar;
    in->data(keys.at(loop), bar);
    input[loop] = (TA_Real) bar.data();
  }

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) method)
  {
    case EMA:
      rc = TA_EMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case DEMA:
      rc = TA_DEMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case KAMA:
      rc = TA_KAMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case SMA:
      rc = TA_SMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case TEMA:
      rc = TA_TEMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case TRIMA:
      rc = TA_TRIMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _Wilder:
    {
      Wilder wilder;
      return wilder.wilder(in, period, lineType, color);
      break;
    }
    case WMA:
      rc = TA_WMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      qDebug() << "MAFactory::ma: invalid method";
      return 0;
      break;
  }
      
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MAFactory::ma: TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    PlotLineBar bar(color, out[outLoop]);
    line->setData(keys.at(keyLoop), bar);
    keyLoop--;
    outLoop--;
  }

  return line;
}

QStringList & MAFactory::list ()
{
  return _maList;
}

int MAFactory::typeFromString (QString &d)
{
  return _maList.indexOf(d);
}

