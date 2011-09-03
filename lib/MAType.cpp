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
#include "CurveData.h"
#include "CurveBar.h"

#include <QDebug>

MAType::MAType ()
{
  _list << "SMA" << "EMA" << "WMA" << "DEMA" << "TEMA" << "TRIMA" << "KAMA" << "WILDER";

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

Data * MAType::getMA (Data *in, int period, int method)
{
  if (method == _WILDER)
    return getWilder(in, period);

  QList<int> keys = in->barKeys();

  int size = keys.count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Data *bar = in->getData(keys.at(loop));
    input[loop] = (TA_Real) bar->get(CurveBar::_VALUE).toDouble();
  }

  TA_RetCode rc = TA_MA(0, size - 1, &input[0], period, (TA_MAType) method, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MA::calculate: TA-Lib error" << rc;
    return 0;
  }

  Data *line = new CurveData;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CurveBar *b = new CurveBar;
    b->set(CurveBar::_VALUE, out[outLoop]);
    line->set(keys.at(keyLoop), b);

    keyLoop--;
    outLoop--;
  }

  return line;
}

Data * MAType::getWilder (Data *in, int period)
{
  Data *line = new CurveData;

  QList<int> keys = in->barKeys();

  double t = 0;
  int loop = 0;
  for (; loop < period; loop++)
  {
    Data *bar = in->getData(keys.at(loop));
    t += bar->get(CurveBar::_VALUE).toDouble();
  }
  double yesterday = t / (double) period;
  CurveBar *db = new CurveBar;
  db->set(CurveBar::_VALUE, QVariant(yesterday));
  line->set(keys.at(loop), db);

  for (; loop < keys.count(); loop++)
  {
    Data *bar = in->getData(keys.at(loop));
    double t  = (yesterday * (period - 1) + bar->get(CurveBar::_VALUE).toDouble()) / (double) period;
    yesterday = t;

    CurveBar *db = new CurveBar;
    db->set(CurveBar::_VALUE, QVariant(t));
    line->set(keys.at(loop), db);
  }

  return line;
}
