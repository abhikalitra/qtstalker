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

#include "PlotLine.h"
#include <QPainter>
#include <QPolygon>
#include <QObject>
#include <QList>

PlotLine::PlotLine ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  high = -99999999;
  low = 99999999;
  scaleFlag = FALSE;
  colorFlag = FALSE;
  plotFlag = FALSE;
  
  typeList << QObject::tr("Dot") << QObject::tr("Dash") << QObject::tr("Histogram") << QObject::tr("Histogram Bar");
  typeList << QObject::tr("Line") << QObject::tr("Invisible") << QObject::tr("Horizontal");
  typeList << QObject::tr("Bar") << QObject::tr("Candle");
}

void PlotLine::setColor (QString &d)
{
  color.setNamedColor(d);
//  resetColor();
}

void PlotLine::setColor (QColor &d)
{
  color = d;
//  resetColor();
}
  
void PlotLine::getColor (QColor &d)
{
  d = color;
}

void PlotLine::setColorBar (int i, QColor &d)
{
  Val r = data[i];
  r.color = d;
  data.replace(i, r);
  
//  data[i].color = d;
}

void PlotLine::getColorBar (int i, QColor &d)
{
  d = data[i].color;
}

void PlotLine::resetColor ()
{
  if (! data.count())
    return;
  
  int loop;
  for (loop = 0; loop < data.count(); loop++)
  {
    Val r = data[loop];
    r.color = color;
    data.replace(loop, r);
  }
}

void PlotLine::setType (PlotLine::LineType d)
{
  lineType = d;
}

void PlotLine::setType (QString &d)
{
  int lt = typeList.indexOf(d);
  if (lt != -1)
    lineType = (LineType) lt;
}

PlotLine::LineType PlotLine::getType ()
{
  return lineType;
}

void PlotLine::setLabel (QString &d)
{
  label = d;
}

void PlotLine::getLabel (QString &d)
{
  d = label;
}

void PlotLine::append (double d)
{
  Val r;
  memset(&r, 0, sizeof(Val));
  r.color = color;
  r.open = d;
  r.high = d;
  r.low = d;
  r.v = d;
  data.append(r);
  checkHighLow(d);
}

void PlotLine::prepend (double d)
{
  Val r;
  memset(&r, 0, sizeof(Val));
  r.color = color;
  r.open = d;
  r.high = d;
  r.low = d;
  r.v = d;
  data.prepend(r);
  checkHighLow(d);
}

double PlotLine::getData (int d)
{
  return data[d].v;
}

void PlotLine::setData (int i, double d)
{
  Val r = data[i];
  r.open = d;
  r.high = d;
  r.low = d;
  r.v = d;
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
  l = typeList;
}

void PlotLine::append (QColor &c, double o, double h, double l, double cl, bool cf)
{
  Val r;
  memset(&r, 0, sizeof(Val));
  r.color = c;
  r.open = o;
  r.high = h;
  r.low = l;
  r.v = cl;
  r.candleFill = cf;
  data.append(r);
  checkHighLow(o);
  checkHighLow(h);
  checkHighLow(l);
  checkHighLow(cl);
}

void PlotLine::append2 (QColor &c, double o, double h, double l, double cl, bool cf)
{
  Val r;
  memset(&r, 0, sizeof(Val));
  r.color = c;
  r.open = o;
  r.high = h;
  r.low = l;
  r.v = cl;
  r.candleFill = cf;
  data.append(r);
  checkHighLow(h);
  checkHighLow(l);
}

void PlotLine::prepend (QColor &c, double o, double h, double l, double cl, bool cf)
{
  Val r;
  memset(&r, 0, sizeof(Val));
  r.color = c;
  r.open = o;
  r.high = h;
  r.low = l;
  r.v = cl;
  r.candleFill = cf;
  data.prepend(r);
  checkHighLow(o);
  checkHighLow(h);
  checkHighLow(l);
  checkHighLow(cl);
}

void PlotLine::getData (int i, QColor &c, double &o, double &h, double &l, double &cl, bool &cf)
{
  Val r = data[i];
  c = r.color;
  o = r.open;
  h = r.high;
  l = r.low;
  cl = r.v;
  cf = r.candleFill;
}

void PlotLine::getHighLowRange (int start, int end, double &h, double &l)
{
  int loop;
  h = -99999999;
  l = 99999999;
  for (loop = start; loop <= end; loop++)
  {
    Val r = data[loop];

    switch (lineType)
    {
      case Bar:
      case Candle:
        if (r.open > h)
          h = r.open;
        if (r.open < l)
          l = r.open;

        if (r.high > h)
          h = r.high;
        if (r.high < l)
          l = r.high;

        if (r.low > h)
          h = r.low;
        if (r.low < l)
          l = r.low;

        if (r.v > h)
          h = r.v;
        if (r.v < l)
          l = r.v;
        break;
      default:
        if (r.v > h)
          h = r.v;
        if (r.v < l)
          l = r.v;
      break;
    }
  }
}

void PlotLine::getInfo (int i, Setting &r)
{
  QString s, k;
  double open, high, low, close;
  QColor color;
  bool ff;

  switch (lineType)
  {
    case Bar:
    case Candle:
      getData(i, color, open, high, low, close, ff);

      strip(open, 4, s);
      k = "O";
      r.setData(k, s);

      strip(high, 4, s);
      k = "H";
      r.setData(k, s);

      strip(low, 4, s);
      k = "L";
      r.setData(k, s);

      strip(close, 4, s);
      k = "C";
      r.setData(k, s);
      break;
    default:
      strip(getData(i), 4, s);
      r.setData(label, s);
      break;
  }
}

void PlotLine::strip (double d, int p, QString &s)
{
  s = QString::number(d, 'f', p);

  while (1)
  {
    if (s.indexOf('.', -1, Qt::CaseSensitive) != -1)
    {
      s.truncate(s.length() - 1);
      break;
    }
    else
    {
      if (s.indexOf('0', -1, Qt::CaseSensitive) != -1)
        s.truncate(s.length() - 1);
      else
        break;
    }
  }
}

void PlotLine::getData (int i, QDateTime &dt)
{
  if (i >= (int) dateList.count())
    return;

  dt = dateList[i];
}

void PlotLine::append (QDateTime &dt)
{
  dateList.append(dt);
}

void PlotLine::prepend (QDateTime &dt)
{
  dateList.prepend(dt);
}

void PlotLine::getDateList (QList<QDateTime> &dl)
{
  dl = dateList;
}

void PlotLine::setDateList (QList<QDateTime> &dl)
{
  dateList = dl;
}

void PlotLine::getData (QList<Val> &d)
{
  d = data;
}

void PlotLine::setPlotFlag (bool d)
{
  plotFlag = d;
}

bool PlotLine::getPlotFlag ()
{
  return plotFlag;
}


