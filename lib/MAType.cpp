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

#include "MAType.h"
#include "ta_libc.h"

#include <QDebug>

MAType::MAType ()
{
  _list << "SMA" << "EMA" << "WMA" << "DEMA" << "TEMA" << "TRIMA" << "KAMA";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("MAType::MAType: error on TA_Initialize");
}

QStringList & MAType::list ()
{
  return _list;
}

MAType::Type MAType::fromString (QString d)
{
  return (MAType::Type) _list.indexOf(d);
}

Curve * MAType::getMA (Curve *in, int period, int method)
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
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_MA(0, size - 1, &input[0], period, (TA_MAType) method, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MA::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  return line;
}

Curve * MAType::getWilder (Curve *in, int period)
{
  if (in->count() < period)
    return 0;

  Curve *line = new Curve;

  QList<int> keys;
  in->keys(keys);

  double t = 0;
  int loop = 0;
  for (; loop < period; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    t += bar->data();
  }
  double yesterday = t / (double) period;
  line->setBar(keys.at(loop), new CurveBar(yesterday));

  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    double t  = (yesterday * (period - 1) + bar->data()) / (double) period;
    yesterday = t;

    line->setBar(keys.at(loop), new CurveBar(t));
  }

  return line;
}
