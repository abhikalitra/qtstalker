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
//  _list << "Date" << "Open" << "High" << "Low" << "Close" << "Volume" << "OI";
}

/*
QStringList & InputType::list ()
{
  return _list;
}
*/
/*
InputType::Type InputType::fromString (QString d)
{
  return (InputType::Type) _list.indexOf(d);
}
*/
/*
Data * InputType::input (BarData *bd, QString t)
{
  int type = _list.indexOf(t);
  if (type == -1)
    return 0;

  Data *line = new Data;
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
	DataBar *cb = new DataBar;
	cb->setDateTime(bar->date());
        line->setBar(loop, cb);
	break;
      }
      case _OPEN:
        line->setBar(loop, new DataBar(bar->open()));
	break;
      case _HIGH:
        line->setBar(loop, new DataBar(bar->high()));
	break;
      case _LOW:
        line->setBar(loop, new DataBar(bar->low()));
	break;
      case _CLOSE:
        line->setBar(loop, new DataBar(bar->close()));
	break;
      case _VOLUME:
        line->setBar(loop, new DataBar(bar->volume()));
	break;
      case _OI:
        line->setBar(loop, new DataBar(bar->oi()));
	break;
      default:
	break;
    }
  }

  return line;
}
*/
/*
Data * InputType::ohlc (BarData *bd)
{
  Data *line = new Data;
  int loop = 0;
  for (; loop < bd->count(); loop++)
  {
    Bar *b = bd->bar(loop);
    if (! b)
      continue;

    DataBar *cb = new DataBar;
    cb->setData(0, b->open());
    cb->setData(1, b->high());
    cb->setData(2, b->low());
    cb->setData(3, b->close());
    line->setBar(loop, cb);
  }

  return line;
}
*/
/*
int InputType::inputs (QList<Data *> &list, QStringList &order, Script *i, BarData *bd)
{
  int loop = 0;
  for (; loop < order.count(); loop++)
  {
    Data *c = i->curve(order.at(loop));
    if (! c)
    {
      c = input(bd, order.at(loop));
      if (! c)
        return 1;

      c->setLabel(order.at(loop));
      i->setData(order.at(loop), c);
      list.append(c);
    }
    else
      list.append(c);
}

  return 0;
}
*/

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

        out[opos] = (TA_Real) bar->getDouble(CurveBar::_VALUE);
        out2[opos] = (TA_Real) bar2->getDouble(CurveBar::_VALUE);
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

        out[opos] = (TA_Real) bar->getDouble(CurveBar::_VALUE);
        out2[opos] = (TA_Real) bar2->getDouble(CurveBar::_VALUE);
        out3[opos] = (TA_Real) bar3->getDouble(CurveBar::_VALUE);
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

        out[opos] = (TA_Real) bar->getDouble(CurveBar::_VALUE);
        out2[opos] = (TA_Real) bar2->getDouble(CurveBar::_VALUE);
        out3[opos] = (TA_Real) bar3->getDouble(CurveBar::_VALUE);
        out4[opos] = (TA_Real) bar4->getDouble(CurveBar::_VALUE);
        opos++;

	break;
      }
      default:
        Data *bar = list.at(0)->getData(keys.at(ipos));
        if (! bar)
          continue;

        out[opos] = (TA_Real) bar->getDouble(CurveBar::_VALUE);
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
        b->set(CurveBar::_VALUE, out[outLoop]);
        c->set(keys.at(keyLoop), b);

        b = new CurveBar;
        b->set(CurveBar::_VALUE, out2[outLoop]);
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
        b->set(CurveBar::_VALUE, out[outLoop]);
        c->set(keys.at(keyLoop), b);

        b = new CurveBar;
        b->set(CurveBar::_VALUE, out2[outLoop]);
        c2->set(keys.at(keyLoop), b);

        b = new CurveBar;
        b->set(CurveBar::_VALUE, out3[outLoop]);
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
        b->set(CurveBar::_VALUE, out[outLoop]);
        c->set(keys.at(keyLoop), b);

        keyLoop--;
        outLoop--;
      }
      break;
    }
  }

  return 0;
}
