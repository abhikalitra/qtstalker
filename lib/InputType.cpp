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
  _list << "Open" << "High" << "Low" << "Close" << "Volume" << "OI";
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

Curve * InputType::ohlc (BarData *bd, QColor uc, QColor dc, QColor nc)
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
    cb->setColor(nc);

    Bar *yb = bd->bar(loop - 1);
    if (yb)
    {
      if (b->close() < yb->close())
	cb->setColor(dc);
      else
      {
	if (b->close() > yb->close())
	  cb->setColor(uc);
      }
    }
    
    line->setBar(loop, cb);
  }

  return line;
}
