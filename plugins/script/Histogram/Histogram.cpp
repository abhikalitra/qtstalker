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

#include "Histogram.h"
#include "Globals.h"
#include "Curve.h"

#include <QtDebug>
#include <QSettings>

Histogram::Histogram ()
{
  _plugin = "Histogram";
  _type = "CURVE";
}

Histogram::~Histogram ()
{
  qDeleteAll(_items);
}

int Histogram::request (Setting *request, Setting *data)
{
  QStringList l;
  l << "CLEAR" << "HIGH_LOW" << "INFO";

  switch (l.indexOf(request->data("REQUEST")))
  {
    case 0:
      return clear();
      break;
    case 1:
      return highLowRange(request, data);
      break;
    case 2:
      return info(request, data);
      break;
    default:
      return 1;
      break;
  }
}

void Histogram::setParent (void *p)
{
  _plot = (QwtPlot *) p;
}

int Histogram::clear ()
{
  qDeleteAll(_items);
  _items.clear();
  return 0;
}

int Histogram::setCurve (Curve *curve)
{
  if (curve->type() != _plugin)
    return 1;

  QSettings set(g_localSettings);
  HistogramDraw *qcurve = new HistogramDraw;
  qcurve->setRenderHint(QwtPlotItem::RenderAntialiased, set.value("antialias", TRUE).toBool());
  qcurve->setCurveData(curve);
      
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

  QColor c = curve->color();
  c.setAlpha(150);
  qcurve->setPen(c);
  qcurve->setBrush(c);
  qcurve->setTitle(curve->label());
  qcurve->setPen(QPen(curve->color()));
  qcurve->setZ(curve->z());
  qcurve->setYAxis(QwtPlot::yRight);
  qcurve->attach(_plot);
  _items.insert(curve->label(), qcurve);

  return 0;
}

int Histogram::info (Setting *request, Setting *data)
{
  int index = request->getInt("INDEX");
  QHashIterator<QString, HistogramDraw *> it(_items);
  while (it.hasNext())
  {
    it.next();
    it.value()->info(index, data);
  }

  return 0;
}
int Histogram::highLowRange (Setting *request, Setting *data)
{
  int rc = 1;
  int flag = 0;
  double h = 0;
  double l = 0;
  int start = request->getInt("START");
  int end = request->getInt("END");
  QHashIterator<QString, HistogramDraw *> it(_items);
  while (it.hasNext())
  {
    it.next();

    double th = 0;
    double tl = 0;
    if (it.value()->highLowRange(start, end, th, tl))
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
  Histogram *o = new Histogram;
  return ((Plugin *) o);
}
