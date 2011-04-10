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

#include "Candle.h"
#include "Globals.h"
#include "Curve.h"

#include <QtDebug>

Candle::Candle ()
{
  _plugin = "Candle";
  _type = "CURVE";
}

Candle::~Candle ()
{
  qDeleteAll(_items);
}

int Candle::request (Setting *request, Setting *)
{
  QStringList l;
  l << "CLEAR";
  
  switch (l.indexOf(request->data("REQUEST")))
  {
    case 0:
      return clear();
      break;
    default:
      return 1;
      break;
  }
}

void Candle::setParent (void *p)
{
  _plot = (QwtPlot *) p;
}

int Candle::clear ()
{
  qDeleteAll(_items);
  _items.clear();
  return 0;
}

int Candle::setCurve (Curve *curve)
{
  if (curve->type() != _plugin)
    return 1;
  
  CandleDraw *qcurve = new CandleDraw;
  qcurve->setData(curve);

  qcurve->setTitle(curve->label());
  qcurve->setPen(QPen(curve->color()));
  qcurve->setZ(curve->z());
  qcurve->setYAxis(QwtPlot::yRight);
  qcurve->attach(_plot);
    
  _items.insert(curve->label(), qcurve);

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  Candle *o = new Candle;
  return ((Plugin *) o);
}