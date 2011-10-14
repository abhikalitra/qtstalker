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

#include "TALibInput.h"
#include "CurveBar.h"

TALibInput::TALibInput ()
{
}

int TALibInput::fill (QList<Data *> &list, QList<int> &keys, TA_Real out[], TA_Real out2[], TA_Real out3[], TA_Real out4[])
{
  if (! list.count())
    return 0;

  switch (list.count())
  {
    case 2:
      return fill2(list, keys, &out[0], &out2[0]);
    case 3:
      return fill3(list, keys, &out[0], &out2[0], &out3[0]);
    case 4:
      return fill4(list, keys, &out[0], &out2[0], &out3[0], &out4[0]);
    default:
      return fill1(list, keys, &out[0]);
  }

  return 0;
}

int TALibInput::fill1 (QList<Data *> &list, QList<int> &keys, TA_Real out[])
{
  int ipos = 0;
  int opos = 0;
  Data *in = list.at(0);
  for (; ipos < keys.count(); ipos++)
  {
    Data *bar = in->toData(keys.at(ipos));
    if (! bar)
      continue;

    out[opos] = (TA_Real) bar->toData(CurveBar::_VALUE)->toDouble();
    opos++;
  }

  return opos;
}

int TALibInput::fill2 (QList<Data *> &list, QList<int> &keys, TA_Real out[], TA_Real out2[])
{
  int ipos = 0;
  int opos = 0;
  Data *in = list.at(0);
  Data *in2 = list.at(1);
  for (; ipos < keys.count(); ipos++)
  {
    Data *bar = in->toData(keys.at(ipos));
    if (! bar)
      continue;

    Data *bar2 = in2->toData(keys.at(ipos));
    if (! bar2)
      continue;

    out[opos] = (TA_Real) bar->toData(CurveBar::_VALUE)->toDouble();
    out2[opos] = (TA_Real) bar2->toData(CurveBar::_VALUE)->toDouble();
    opos++;
  }

  return opos;
}

int TALibInput::fill3 (QList<Data *> &list, QList<int> &keys, TA_Real out[], TA_Real out2[], TA_Real out3[])
{
  int ipos = 0;
  int opos = 0;
  Data *in = list.at(0);
  Data *in2 = list.at(1);
  Data *in3 = list.at(2);
  for (; ipos < keys.count(); ipos++)
  {
    Data *bar = in->toData(keys.at(ipos));
    if (! bar)
      continue;

    Data *bar2 = in2->toData(keys.at(ipos));
    if (! bar2)
      continue;

    Data *bar3 = in3->toData(keys.at(ipos));
    if (! bar3)
      continue;

    out[opos] = (TA_Real) bar->toData(CurveBar::_VALUE)->toDouble();
    out2[opos] = (TA_Real) bar2->toData(CurveBar::_VALUE)->toDouble();
    out3[opos] = (TA_Real) bar3->toData(CurveBar::_VALUE)->toDouble();
    opos++;
  }

  return opos;
}

int TALibInput::fill4 (QList<Data *> &list, QList<int> &keys, TA_Real out[], TA_Real out2[], TA_Real out3[], TA_Real out4[])
{
  int ipos = 0;
  int opos = 0;
  Data *in = list.at(0);
  Data *in2 = list.at(1);
  Data *in3 = list.at(2);
  Data *in4 = list.at(3);
  for (; ipos < keys.count(); ipos++)
  {
    Data *bar = in->toData(keys.at(ipos));
    if (! bar)
      continue;

    Data *bar2 = in2->toData(keys.at(ipos));
    if (! bar2)
      continue;

    Data *bar3 = in3->toData(keys.at(ipos));
    if (! bar3)
      continue;

    Data *bar4 = in4->toData(keys.at(ipos));
    if (! bar4)
      continue;

    out[opos] = (TA_Real) bar->toData(CurveBar::_VALUE)->toDouble();
    out2[opos] = (TA_Real) bar2->toData(CurveBar::_VALUE)->toDouble();
    out3[opos] = (TA_Real) bar3->toData(CurveBar::_VALUE)->toDouble();
    out4[opos] = (TA_Real) bar4->toData(CurveBar::_VALUE)->toDouble();
    opos++;
  }

  return opos;
}
