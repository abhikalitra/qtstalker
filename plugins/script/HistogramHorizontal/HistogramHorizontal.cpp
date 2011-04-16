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

#include "HistogramHorizontal.h"
#include "Globals.h"
#include "Curve.h"

#include <QtDebug>

HistogramHorizontal::HistogramHorizontal ()
{
  _plugin = "HistogramHorizontal";
  _type = "CURVE";
}

HistogramHorizontal::~HistogramHorizontal ()
{
  qDeleteAll(_items);
}

int HistogramHorizontal::request (Setting *request, Setting *data)
{
  QStringList l;
  l << "CLEAR" << "HIGH_LOW";
  
  switch (l.indexOf(request->data("REQUEST")))
  {
    case 0:
      return clear();
      break;
    case 1:
      return highLowRange(request, data);
      break;
    default:
      return 1;
      break;
  }
}

void HistogramHorizontal::setParent (void *p)
{
  _plot = (QwtPlot *) p;
}

int HistogramHorizontal::clear ()
{
  qDeleteAll(_items);
  _items.clear();
  return 0;
}

int HistogramHorizontal::setCurve (Curve *curve)
{
  if (curve->type() != _plugin)
    return 1;
  
  HistogramHorizontalDraw *qcurve = new HistogramHorizontalDraw;
  qcurve->setData(curve);

  qcurve->setTitle(curve->label());
  qcurve->setPen(QPen(curve->color()));
  qcurve->setZ(curve->z());
  qcurve->setYAxis(QwtPlot::yRight);
  qcurve->attach(_plot);
    
  _items.insert(curve->label(), qcurve);

  return 0;
}

int HistogramHorizontal::highLowRange (Setting *, Setting *data)
{
  int rc = 1;
  int flag = 0;
  double h = 0;
  double l = 0;
  QHashIterator<QString, HistogramHorizontalDraw *> it(_items);
  while (it.hasNext())
  {
    it.next();

    double th = 0;
    double tl = 0;
    if (it.value()->highLow(th, tl))
      continue;
    
    rc = 0;
    
    if (! flag)
    {
      h = th;
      l = tl;
      flag++;
    }
    else
    {
      if (th > h)
        h = th;
      if (tl < l)
        l = tl;
    }
  }

  if (flag)
  {
    data->setData("HIGH", h);
    data->setData("LOW", l);
  }

  return rc;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  HistogramHorizontal *o = new HistogramHorizontal;
  return ((Plugin *) o);
}
