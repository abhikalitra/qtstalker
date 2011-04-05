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

#include "Dot.h"
#include "Globals.h"
#include "Curve.h"

#include <QtDebug>

Dot::Dot ()
{
  _plugin = "Dot";
  _type = "CURVE";
}

Dot::~Dot ()
{
  qDeleteAll(_items);
}

int Dot::request (Setting *request, Setting *)
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

void Dot::setParent (void *p)
{
  _plot = (QwtPlot *) p;
}

int Dot::clear ()
{
  qDeleteAll(_items);
  _items.clear();
  return 0;
}

int Dot::setCurve (Curve *curve)
{
  if (curve->type() != _plugin)
    return 1;
  
  QwtPlotCurve *qcurve = new QwtPlotCurve;
  qcurve->setStyle(QwtPlotCurve::Dots);

  QList<int> keys;
  curve->keys(keys);

  QwtArray<double> x;
  QwtArray<double> y;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    x.append(keys.at(loop));
    CurveBar *bar = curve->bar(keys.at(loop));
    y.append(bar->data());
  }
  qcurve->setData(x, y);
  
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
  Dot *o = new Dot;
  return ((Plugin *) o);
}
