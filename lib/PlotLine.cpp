/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#include "PlotLine.h"
#include <qpainter.h>
#include <qpointarray.h>

PlotLine::PlotLine ()
{
  data.setAutoDelete(TRUE);
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  high = -99999999;
  low = 99999999;
  scaleFlag = FALSE;
  colorFlag = FALSE;
}

PlotLine::~PlotLine ()
{
}

void PlotLine::copy (PlotLine *d)
{
  QColor c(d->getColor());
  setColor(c);
  
  setType(d->getType());
  bool flag = FALSE;
  if (lineType == Bar || lineType == Candle)
    flag = TRUE;
  
  QString s = d->getLabel();
  setLabel(s);
  
  setScaleFlag(d->getScaleFlag());

  setColorFlag(d->getColorFlag());

  int loop;
  QColor color;
  double o;
  double h;
  double l;
  double cl;
  bool ff;
  for (loop = 0; loop < (int) d->getSize(); loop++)
  {
    d->getBar(loop, color, o, h, l, cl, ff);
    appendBar(color, o, h, l , cl, ff);
  }
}

void PlotLine::setColor (QString &d)
{
  color.setNamedColor(d);
}

void PlotLine::setColor (QColor &d)
{
  color = d;
}

QColor PlotLine::getColor ()
{
  return color;
}

void PlotLine::setColorBar (int i, QColor &d)
{
  Val *r = data.at(i);
  if (r)
    r->color = d;
}

QColor PlotLine::getColorBar (int i)
{
  QColor color;
  Val *r = data.at(i);
  if (r)
    color = r->color;
  return color;
}

void PlotLine::setType (PlotLine::LineType d)
{
  lineType = d;
}

void PlotLine::setType (QString &d)
{
  if (! d.compare(tr("Dot")))
  {
    lineType = Dot;
    return;
  }

  if (! d.compare(tr("Dash")))
  {
    lineType = Dash;
    return;
  }
  
  if (! d.compare(tr("Histogram")))
  {
    lineType = Histogram;
    return;
  }
  
  if (! d.compare(tr("Dash")))
  {
    lineType = Dash;
    return;
  }

  if (! d.compare(tr("Histogram Bar")))
  {
    lineType = HistogramBar;
    return;
  }

  if (! d.compare(tr("Line")))
  {
    lineType = Line;
    return;
  }

  if (! d.compare(tr("Invisible")))
  {
    lineType = Invisible;
    return;
  }

  if (! d.compare(tr("Horizontal")))
  {
    lineType = Horizontal;
    return;
  }

  if (! d.compare(tr("Bar")))
  {
    lineType = Bar;
    return;
  }

  if (! d.compare(tr("Candle")))
  {
    lineType = Candle;
    return;
  }
}

PlotLine::LineType PlotLine::getType ()
{
  return lineType;
}

void PlotLine::setLabel (QString &d)
{
  label = d;
}

QString PlotLine::getLabel ()
{
  return label;
}

void PlotLine::append (double d)
{
  Val *r = new Val;
  memset(r, 0, sizeof(Val));
  r->v  = d;
  data.append(r);
  checkHighLow(d);
}

void PlotLine::prepend (double d)
{
  Val *r = new Val;
  memset(r, 0, sizeof(Val));
  r->v = d;
  data.prepend(r);
  checkHighLow(d);
}

double PlotLine::getData (int d)
{
  Val *r = data.at(d);
  return r->v;
}

void PlotLine::setData (int i, double d)
{
  Val *r = data.at(i);
  r->v = d;
  checkHighLow(d);
}

int PlotLine::getSize ()
{
  return (int) data.count();
}

double PlotLine::getHigh ()
{
  return high;
}

void PlotLine::setHigh (double d)
{
  high = d;
}

double PlotLine::getLow ()
{
  return low;
}

void PlotLine::setLow (double d)
{
  low = d;
}

void PlotLine::checkHighLow (double d)
{
  if (d > high)
    high = d;
  if (d < low)
    low = d;
}

void PlotLine::setScaleFlag (bool d)
{
  scaleFlag = d;
}

bool PlotLine::getScaleFlag ()
{
  return scaleFlag;
}

void PlotLine::setColorFlag (bool d)
{
  colorFlag = d;
}

bool PlotLine::getColorFlag ()
{
  return colorFlag;
}

void PlotLine::getLineTypes (QStringList &l)
{
  l.clear();
  l.append(QObject::tr("Dot"));
  l.append(QObject::tr("Dash"));
  l.append(QObject::tr("Histogram"));
  l.append(QObject::tr("Histogram Bar"));
  l.append(QObject::tr("Line"));
  l.append(QObject::tr("Invisible"));
  l.append(QObject::tr("Horizontal"));
}

void PlotLine::appendBar (QColor &c, double o, double h, double l, double cl)
{
  Val *r = new Val;
  memset(r, 0, sizeof(Val));
  r->color = c;
  r->open = o;
  r->high = h;
  r->low = l;
  r->v = cl;
  data.append(r);
  checkHighLow(o);
  checkHighLow(h);
  checkHighLow(l);
  checkHighLow(cl);
}

void PlotLine::appendBar (QColor &c, double o, double h, double l, double cl, bool cf)
{
  Val *r = new Val;
  memset(r, 0, sizeof(Val));
  r->color = c;
  r->open = o;
  r->high = h;
  r->low = l;
  r->v = cl;
  r->candleFill = cf;
  data.append(r);
  checkHighLow(o);
  checkHighLow(h);
  checkHighLow(l);
  checkHighLow(cl);
}

void PlotLine::getBar (int i, QColor &c, double &o, double &h, double &l, double &cl)
{
  Val *r = data.at(i);
  if (r)
  {
    c = r->color;
    o = r->open;
    h = r->high;
    l = r->low;
    cl = r->v;
  }
}

void PlotLine::getBar (int i, QColor &c, double &o, double &h, double &l, double &cl, bool &cf)
{
  Val *r = data.at(i);
  if (r)
  {
    c = r->color;
    o = r->open;
    h = r->high;
    l = r->low;
    cl = r->v;
    cf = r->candleFill;
  }
}

void PlotLine::getHighLowRange (int start, int end, double &h, double &l)
{
  int loop;
  h = -99999999;
  l = 99999999;

  bool flag = FALSE;
  if (lineType == Bar || lineType == Candle)
    flag = TRUE;

  for (loop = start; loop <= end; loop++)
  {
    Val *r = data.at(loop);
    if (r)
    {
      if (flag)
      {
        if (r->open > h)
          h = r->open;
        if (r->open < l)
          l = r->open;

        if (r->high > h)
          h = r->high;
        if (r->high < l)
          l = r->high;

        if (r->low > h)
          h = r->low;
        if (r->low < l)
          l = r->low;
      }
    }

    if (r->v > h)
      h = r->v;
    if (r->v < l)
      l = r->v;
  }
}

