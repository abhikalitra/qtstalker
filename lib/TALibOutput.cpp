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

#include "TALibOutput.h"
#include "CurveBar.h"
#include "DataDouble.h"

TALibOutput::TALibOutput ()
{
}

int TALibOutput::fillDouble1 (QList<Data *> &list, QList<int> &keys, int outNb, TA_Real out[])
{
  if (! list.count())
    return 1;

  Data *c = list.at(0);
  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CurveBar *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(out[outLoop]));
    c->set(keys.at(keyLoop), b);

    keyLoop--;
    outLoop--;
  }

  return 0;
}

int TALibOutput::fillDouble2 (QList<Data *> &list, QList<int> &keys, int outNb, TA_Real out[], TA_Real out2[])
{
  if (list.count() != 2)
    return 1;

  Data *c = list.at(0);
  Data *c2 = list.at(1);
  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CurveBar *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(out[outLoop]));
    c->set(keys.at(keyLoop), b);

    b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(out2[outLoop]));
    c2->set(keys.at(keyLoop), b);

    keyLoop--;
    outLoop--;
  }

  return 0;
}

int TALibOutput::fillDouble3 (QList<Data *> &list, QList<int> &keys, int outNb, TA_Real out[], TA_Real out2[], TA_Real out3[])
{
  if (list.count() != 3)
    return 1;

  Data *c = list.at(0);
  Data *c2 = list.at(1);
  Data *c3 = list.at(2);
  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CurveBar *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(out[outLoop]));
    c->set(keys.at(keyLoop), b);

    b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(out2[outLoop]));
    c2->set(keys.at(keyLoop), b);

    b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(out3[outLoop]));
    c3->set(keys.at(keyLoop), b);

    keyLoop--;
    outLoop--;
  }

  return 0;
}

int TALibOutput::fillInteger1 (QList<Data *> &list, QList<int> &keys, int outNb, TA_Integer out[])
{
  if (! list.count())
    return 1;

  Data *c = list.at(0);
  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CurveBar *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(out[outLoop]));
    c->set(keys.at(keyLoop), b);

    keyLoop--;
    outLoop--;
  }

  return 0;
}
