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

InputType::InputType ()
{
  _list << "Date" << "Open" << "High" << "Low" << "Close" << "Volume" << "OI";
}

QStringList & InputType::list ()
{
  return _list;
}

InputType::Type InputType::fromString (QString d)
{
  return (InputType::Type) _list.indexOf(d);
}

Curve * InputType::input (BarData *bd, QString t)
{
  int type = _list.indexOf(t);
  if (type == -1)
    return 0;

  Curve *line = new Curve;
  int loop = 0;
  for (; loop < bd->count(); loop++)
  {
    Bar *bar = bd->bar(loop);
    if (! bar)
      continue;
    
    switch ((Type) type)
    {
      case _DATE:
      {
	CurveBar *cb = new CurveBar;
	cb->setDateTime(bar->date());
        line->setBar(loop, cb);
	break;
      }
      case _OPEN:
        line->setBar(loop, new CurveBar(bar->open()));
	break;
      case _HIGH:
        line->setBar(loop, new CurveBar(bar->high()));
	break;
      case _LOW:
        line->setBar(loop, new CurveBar(bar->low()));
	break;
      case _CLOSE:
        line->setBar(loop, new CurveBar(bar->close()));
	break;
      case _VOLUME:
        line->setBar(loop, new CurveBar(bar->volume()));
	break;
      case _OI:
        line->setBar(loop, new CurveBar(bar->oi()));
	break;
      default:
	break;
    }
  }
  
  return line;
}

Curve * InputType::ohlc (BarData *bd)
{
  Curve *line = new Curve;
  int loop = 0;
  for (; loop < bd->count(); loop++)
  {
    Bar *b = bd->bar(loop);
    if (! b)
      continue;

    CurveBar *cb = new CurveBar;
    cb->setData(0, b->open());
    cb->setData(1, b->high());
    cb->setData(2, b->low());
    cb->setData(3, b->close());
    line->setBar(loop, cb);
  }

  return line;
}

int InputType::inputs (QList<Curve *> &list, QStringList &order, Indicator *i, BarData *bd)
{
  int loop = 0;
  for (; loop < order.count(); loop++)
  {
    Curve *c = i->line(order.at(loop));
    if (! c)
    {
      c = input(bd, order.at(loop));
      if (! c)
        return 1;

      c->setLabel(order.at(loop));
      i->setLine(order.at(loop), c);
      list.append(c);
    }
    else
      list.append(c);
}

  return 0;
}

int InputType::keys (QList<Curve *> &list, QList<int> &keys)
{
  keys.clear();
  
  int loop = 0;
  for (; loop < list.count(); loop++)
  {
    if (! keys.count())
    {
      list.at(loop)->keys(keys);
      continue;
    }
    
    if (list.at(loop)->count() < keys.count())
      list.at(loop)->keys(keys);
  }

  if (! keys.count())
    return 1;
  
  return 0;
}

int InputType::fill (QList<Curve *> &list, QList<int> &keys, TA_Real out[], TA_Real out2[], TA_Real out3[], TA_Real out4[])
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
        CurveBar *bar = list.at(0)->bar(keys.at(ipos));
        if (! bar)
          continue;
        CurveBar *bar2 = list.at(1)->bar(keys.at(ipos));
        if (! bar2)
          continue;
        out[opos] = (TA_Real) bar->data();
        out2[opos] = (TA_Real) bar2->data();
        opos++;
	break;
      }
      case 3:
      {
        CurveBar *bar = list.at(0)->bar(keys.at(ipos));
        if (! bar)
          continue;
        CurveBar *bar2 = list.at(1)->bar(keys.at(ipos));
        if (! bar2)
          continue;
        CurveBar *bar3 = list.at(2)->bar(keys.at(ipos));
        if (! bar3)
          continue;
        out[opos] = (TA_Real) bar->data();
        out2[opos] = (TA_Real) bar2->data();
        out3[opos] = (TA_Real) bar3->data();
        opos++;
	break;
      }
      case 4:
      {
        CurveBar *bar = list.at(0)->bar(keys.at(ipos));
        if (! bar)
          continue;
        CurveBar *bar2 = list.at(1)->bar(keys.at(ipos));
        if (! bar2)
          continue;
        CurveBar *bar3 = list.at(2)->bar(keys.at(ipos));
        if (! bar3)
          continue;
        CurveBar *bar4 = list.at(3)->bar(keys.at(ipos));
        if (! bar4)
          continue;
        out[opos] = (TA_Real) bar->data();
        out2[opos] = (TA_Real) bar2->data();
        out3[opos] = (TA_Real) bar3->data();
        out4[opos] = (TA_Real) bar4->data();
        opos++;
	break;
      }
      default:
        CurveBar *bar = list.at(0)->bar(keys.at(ipos));
        if (! bar)
          continue;
        out[opos] = (TA_Real) bar->data();
        opos++;
	break;
    }
  }

  return opos;
}

int InputType::outputs (QList<Curve *> &list, QList<int> &keys, int outNb, TA_Real out[], TA_Real out2[], TA_Real out3[])
{
  if (! list.count())
    return 1;

  switch (list.count())
  {
    case 2:
    {
      Curve *c = list.at(0);
      Curve *c2 = list.at(1);
      int keyLoop = keys.count() - 1;
      int outLoop = outNb - 1;
      while (keyLoop > -1 && outLoop > -1)
      {
        c->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
        c2->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));
        keyLoop--;
        outLoop--;
      }
      break;
    }
    case 3:
    {
      Curve *c = list.at(0);
      Curve *c2 = list.at(1);
      Curve *c3 = list.at(2);
      int keyLoop = keys.count() - 1;
      int outLoop = outNb - 1;
      while (keyLoop > -1 && outLoop > -1)
      {
        c->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
        c2->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));
        c3->setBar(keys.at(keyLoop), new CurveBar(out3[outLoop]));
        keyLoop--;
        outLoop--;
      }
      break;
    }
    default:
    {
      Curve *c = list.at(0);
      int keyLoop = keys.count() - 1;
      int outLoop = outNb - 1;
      while (keyLoop > -1 && outLoop > -1)
      {
        c->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
        keyLoop--;
        outLoop--;
      }
      break;
    }
  }

  return 0;
}
