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

#include <QObject>

PlotLine::PlotLine ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  high = -99999999;
  low = 99999999;
  scaleFlag = FALSE;
  colorFlag = FALSE;
  plotFlag = FALSE;

  typeList << QObject::tr("Dot") << QObject::tr("Dash") << QObject::tr("Histogram");
  typeList << QObject::tr("Histogram Bar") << QObject::tr("Line") << QObject::tr("Horizontal");
  typeList << QObject::tr("Bar") << QObject::tr("Candle");
}

void PlotLine::setColor (QString &d)
{
  color.setNamedColor(d);
}

void PlotLine::setColor (QColor &d)
{
  color = d;
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
}

void PlotLine::getColorBar (int i, QColor &d)
{
  d = data[i].color;
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
  r.v = d;
  data.append(r);
  checkHighLow(d);
}

void PlotLine::prepend (double d)
{
  Val r;
  memset(&r, 0, sizeof(Val));
  r.color = color;
  r.v = d;
  data.prepend(r);
  checkHighLow(d);
}

double PlotLine::getData (int d)
{
  return data[d].v;
}

void PlotLine::getData (int i, QColor &c, double &d)
{
  Val r = data.at(i);
  c = r.color;
  d = r.v;
}

void PlotLine::setData (int i, double d)
{
  Val r = data[i];
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
  l.removeAll(QObject::tr("Horizontal"));
  l.removeAll(QObject::tr("Bar"));
  l.removeAll(QObject::tr("Candle"));
}

void PlotLine::append (QColor &c, double d)
{
  Val r;
  memset(&r, 0, sizeof(Val));
  r.color = c;
  r.v = d;
  data.append(r);
  checkHighLow(d);
}

void PlotLine::prepend (QColor &c, double d)
{
  Val r;
  memset(&r, 0, sizeof(Val));
  r.color = c;
  r.v = d;
  data.prepend(r);
  checkHighLow(d);
}

void PlotLine::getHighLowRange (int start, int end, double &h, double &l)
{
  int loop;
  h = -99999999;
  l = 99999999;
  for (loop = start; loop <= end; loop++)
  {
    Val r = data[loop];
    if (r.v > h)
      h = r.v;
    if (r.v < l)
      l = r.v;
  }
}

void PlotLine::getInfo (int i, Setting &r)
{
  QString s;
  strip(getData(i), 4, s);
  r.setData(label, s);
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

void PlotLine::setPlotFlag (bool d)
{
  plotFlag = d;
}

bool PlotLine::getPlotFlag ()
{
  return plotFlag;
}



