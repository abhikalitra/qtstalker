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

#include "InputType.h"
#include "CurveBar.h"

InputType::InputType ()
{
}

int InputType::keys (QList<Data *> &list, QList<int> &keys)
{
  keys.clear();

  Data *keyData = 0;
  int minKey = 99999999;
  int loop = 0;
  for (; loop < list.count(); loop++)
  {
    Data *curve = list.at(loop);
    if (curve->barKeyCount() < minKey)
    {
      minKey = curve->barKeyCount();
      keyData = curve;
    }
  }

  if (keyData)
  {
    keys = keyData->barKeys();
    return 0;
  }

  return 1;
}

int InputType::fill (QList<Data *> &list, QList<int> &keys, TA_Real out[], TA_Real out2[], TA_Real out3[], TA_Real out4[])
{
  if (! list.count())
    return 0;

  int ipos = 0;
  int opos = 0;
  for (; ipos < keys.count(); ipos++)
  {
    switch (list.count())
    {
      case 2:
      {
        Data *bar = list.at(0)->getData(keys.at(ipos));
        if (! bar)
          continue;

        Data *bar2 = list.at(1)->getData(keys.at(ipos));
        if (! bar2)
          continue;

        out[opos] = (TA_Real) bar->get(CurveBar::_VALUE).toDouble();
        out2[opos] = (TA_Real) bar2->get(CurveBar::_VALUE).toDouble();
        opos++;

	break;
      }
      case 3:
      {
        Data *bar = list.at(0)->getData(keys.at(ipos));
        if (! bar)
          continue;

        Data *bar2 = list.at(1)->getData(keys.at(ipos));
        if (! bar2)
          continue;

        Data *bar3 = list.at(2)->getData(keys.at(ipos));
        if (! bar3)
          continue;

        out[opos] = (TA_Real) bar->get(CurveBar::_VALUE).toDouble();
        out2[opos] = (TA_Real) bar2->get(CurveBar::_VALUE).toDouble();
        out3[opos] = (TA_Real) bar3->get(CurveBar::_VALUE).toDouble();
        opos++;

	break;
      }
      case 4:
      {
        Data *bar = list.at(0)->getData(keys.at(ipos));
        if (! bar)
          continue;

        Data *bar2 = list.at(1)->getData(keys.at(ipos));
        if (! bar2)
          continue;

        Data *bar3 = list.at(2)->getData(keys.at(ipos));
        if (! bar3)
          continue;

        Data *bar4 = list.at(3)->getData(keys.at(ipos));
        if (! bar4)
          continue;

        out[opos] = (TA_Real) bar->get(CurveBar::_VALUE).toDouble();
        out2[opos] = (TA_Real) bar2->get(CurveBar::_VALUE).toDouble();
        out3[opos] = (TA_Real) bar3->get(CurveBar::_VALUE).toDouble();
        out4[opos] = (TA_Real) bar4->get(CurveBar::_VALUE).toDouble();
        opos++;

	break;
      }
      default:
        Data *bar = list.at(0)->getData(keys.at(ipos));
        if (! bar)
          continue;

        out[opos] = (TA_Real) bar->get(CurveBar::_VALUE).toDouble();
        opos++;

	break;
    }
  }

  return opos;
}

int InputType::outputs (QList<Data *> &list, QList<int> &keys, int outNb, TA_Real out[], TA_Real out2[], TA_Real out3[])
{
  if (! list.count())
    return 1;

  switch (list.count())
  {
    case 2:
    {
      Data *c = list.at(0);
      Data *c2 = list.at(1);
      int keyLoop = keys.count() - 1;
      int outLoop = outNb - 1;
      while (keyLoop > -1 && outLoop > -1)
      {
        CurveBar *b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(out[outLoop]));
        c->set(keys.at(keyLoop), b);

        b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(out2[outLoop]));
        c2->set(keys.at(keyLoop), b);

        keyLoop--;
        outLoop--;
      }
      break;
    }
    case 3:
    {
      Data *c = list.at(0);
      Data *c2 = list.at(1);
      Data *c3 = list.at(2);
      int keyLoop = keys.count() - 1;
      int outLoop = outNb - 1;
      while (keyLoop > -1 && outLoop > -1)
      {
        CurveBar *b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(out[outLoop]));
        c->set(keys.at(keyLoop), b);

        b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(out2[outLoop]));
        c2->set(keys.at(keyLoop), b);

        b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(out3[outLoop]));
        c3->set(keys.at(keyLoop), b);

        keyLoop--;
        outLoop--;
      }
      break;
    }
    default:
    {
      Data *c = list.at(0);
      int keyLoop = keys.count() - 1;
      int outLoop = outNb - 1;
      while (keyLoop > -1 && outLoop > -1)
      {
        CurveBar *b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(out[outLoop]));
        c->set(keys.at(keyLoop), b);

        keyLoop--;
        outLoop--;
      }
      break;
    }
  }

  return 0;
}

int InputType::outputs (QList<Data *> &list, QList<int> &keys, int outNb, TA_Integer out[], TA_Integer out2[], TA_Integer out3[])
{
  if (! list.count())
    return 1;

  switch (list.count())
  {
    case 2:
    {
      Data *c = list.at(0);
      Data *c2 = list.at(1);
      int keyLoop = keys.count() - 1;
      int outLoop = outNb - 1;
      while (keyLoop > -1 && outLoop > -1)
      {
        CurveBar *b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(out[outLoop]));
        c->set(keys.at(keyLoop), b);

        b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(out2[outLoop]));
        c2->set(keys.at(keyLoop), b);

        keyLoop--;
        outLoop--;
      }
      break;
    }
    case 3:
    {
      Data *c = list.at(0);
      Data *c2 = list.at(1);
      Data *c3 = list.at(2);
      int keyLoop = keys.count() - 1;
      int outLoop = outNb - 1;
      while (keyLoop > -1 && outLoop > -1)
      {
        CurveBar *b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(out[outLoop]));
        c->set(keys.at(keyLoop), b);

        b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(out2[outLoop]));
        c2->set(keys.at(keyLoop), b);

        b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(out3[outLoop]));
        c3->set(keys.at(keyLoop), b);

        keyLoop--;
        outLoop--;
      }
      break;
    }
    default:
    {
      Data *c = list.at(0);
      int keyLoop = keys.count() - 1;
      int outLoop = outNb - 1;
      while (keyLoop > -1 && outLoop > -1)
      {
        CurveBar *b = new CurveBar;
        b->set(CurveBar::_VALUE, QVariant(out[outLoop]));
        c->set(keys.at(keyLoop), b);

        keyLoop--;
        outLoop--;
      }
      break;
    }
  }

  return 0;
}
