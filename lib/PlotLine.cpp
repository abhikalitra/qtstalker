/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
  scaler = 0;
  buffer = 0;
  data.setAutoDelete(TRUE);
  colorBars.setAutoDelete(TRUE);
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  high = -99999999;
  low = 99999999;
  colorBarFlag = FALSE;
  scaleFlag = FALSE;
}

PlotLine::~PlotLine ()
{
}

void PlotLine::copy (PlotLine *d)
{
  QColor c(d->getColor());
  setColor(c);
  setType(d->getType());
  QString s = d->getLabel();
  setLabel(s);
  setColorFlag(d->getColorFlag());
  setScaleFlag(d->getScaleFlag());

  int loop;
  for (loop = 0; loop < (int) d->getSize(); loop++)
  {
    append(d->getData(loop));

    if (d->getColorFlag() == TRUE)
    {
      c = d->getColorBar(loop);
      appendColorBar(c);
    }
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
  r->v  = d;
  data.append(r);
  checkHighLow(d);
}

void PlotLine::prepend (double d)
{
  Val *r = new Val;
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
  colorBarFlag = d;
}

bool PlotLine::getColorFlag ()
{
  return colorBarFlag;
}

void PlotLine::appendColorBar (QColor &d)
{
  colorBars.append(new QColor(d));
}

void PlotLine::prependColorBar (QColor &d)
{
  colorBars.prepend(new QColor(d));
}

QColor PlotLine::getColorBar (int d)
{
  if (d >= (int) colorBars.count())
    return QString::null;

  QColor *color = colorBars.at(d);
  return QColor(color->red(), color->green(), color->blue());
}

void PlotLine::draw (int dataSize, int startX, int startIndex, int pixelspace)
{ 
  switch (lineType)
  {
    case PlotLine::Histogram:
      drawHistogram(dataSize, startX, startIndex, pixelspace);
      break;
    case PlotLine::HistogramBar:
      drawHistogramBar(dataSize, startX, startIndex, pixelspace);
      break;
    case PlotLine::Dot:
      drawDot(dataSize, startX, startIndex, pixelspace);
      break;
    case PlotLine::Line:
    case PlotLine::Dash:
      drawLine(dataSize, startX, startIndex, pixelspace);
      break;
    case PlotLine::Horizontal:
      drawHorizontalLine(startX);
      break;
    default:
      break;
  }
}

void PlotLine::drawLine (int dataSize, int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);

  QPen pen;
  pen.setColor(getColor());

  if (getType() == PlotLine::Dash)
    pen.setStyle(Qt::DotLine);
  else
    pen.setStyle(Qt::SolidLine);
  painter.setPen(pen);

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;
  int loop = getSize() - dataSize + startIndex;
  
  Scaler *scale = 0;
  if (getScaleFlag())
  {  
    scale = new Scaler;
    scale->set(scaler->getHeight(),
  	       getHigh(),
	       getLow(),
	       scaler->getLogScaleHigh(),
	       scaler->getLogRange(),
	       scaler->getLogFlag());
  }

  while ((x2 < buffer->width()) && (loop < (int) getSize()))
  {
    if (loop > -1)
    {
      if (getScaleFlag())
        y2 = scale->convertToY(getData(loop));
      else
        y2 = scaler->convertToY(getData(loop));

      if (y != -1)
        painter.drawLine (x, y, x2, y2);
      x = x2;
      y = y2;
    }

    x2 = x2 + pixelspace;
    loop++;
  }

  painter.end();

  if (scale)
    delete scale;
}

void PlotLine::drawHorizontalLine (int startX)
{
  QPainter painter;
  painter.begin(buffer);

  QPen pen;
  pen.setColor(getColor());
  painter.setPen(pen);

  int y = scaler->convertToY(getData(getSize() - 1));

  painter.drawLine (0, y, buffer->width(), y);

  painter.drawText(startX, y - 1, strip(getData(getSize() - 1)));

  painter.end();
}

void PlotLine::drawDot (int dataSize, int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);

  QPen pen;
  pen.setColor(getColor());
  painter.setPen(pen);

  int x = startX;
  int loop = getSize() - dataSize + startIndex;
  
  Scaler *scale = 0;
  if (getScaleFlag())
  {
    scale = new Scaler;
    scale->set(scaler->getHeight(),
  	       getHigh(),
	       getLow(),
	       scaler->getLogScaleHigh(),
	       scaler->getLogRange(),
	       scaler->getLogFlag());
  }

  while ((x < buffer->width()) && (loop < (int) getSize()))
  {
    if (loop > -1)
    {
      int y;
      if (getScaleFlag())
        y = scale->convertToY(getData(loop));
      else
        y = scaler->convertToY(getData(loop));

      painter.drawPoint(x, y);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();

  if (scale)
    delete scale;
}

void PlotLine::drawHistogram (int dataSize, int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(getColor());
  painter.setBrush(getColor());

  int loop = getSize() - dataSize + startIndex;

  QPointArray pa(4);

  int zero = 0;
  Scaler *scale = 0;
  if (getScaleFlag())
  {
    scale = new Scaler;
    scale->set(scaler->getHeight(),
  	       getHigh(),
	       getLow(),
	       scaler->getLogScaleHigh(),
	       scaler->getLogRange(),
	       scaler->getLogFlag());
    zero = scale->convertToY(0);
  }
  else
    zero = scaler->convertToY(0);

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;

  while ((x < buffer->width()) && (loop < (int) getSize()))
  {
    if (loop > -1)
    {
      if (getScaleFlag())
        y2 = scale->convertToY(getData(loop));
      else
        y2 = scaler->convertToY(getData(loop));
      pa.setPoint(0, x, zero);
      pa.setPoint(1, x, y);
      pa.setPoint(2, x2, y2);
      pa.setPoint(3, x2, zero);

      if (y != -1)
        painter.drawPolygon(pa, TRUE, 0, -1);

      x = x2;
      y = y2;
    }

    x2 = x2 + pixelspace;
    loop++;
  }

  painter.end();

  if (scale)
    delete scale;
}

void PlotLine::drawHistogramBar (int dataSize, int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);

  QColor color(getColor());

  int x = startX;
  int zero = 0;
  Scaler *scale = 0;
  if (getScaleFlag())
  {
    scale = new Scaler;
    scale->set(scaler->getHeight(),
  	       getHigh(),
	       getLow(),
	       scaler->getLogScaleHigh(),
	       scaler->getLogRange(),
	       scaler->getLogFlag());
    zero = scale->convertToY(0);
  }
  else
    zero = scaler->convertToY(0);

  int loop = getSize() - dataSize + startIndex;

  while ((x < buffer->width()) && (loop < (int) getSize()))
  {
    if (loop > -1)
    {
      int y;
      if (getScaleFlag())
        y = scale->convertToY(getData(loop));
      else
        y = scaler->convertToY(getData(loop));

      if (getColorFlag() == TRUE)
	color = getColorBar(loop);

      painter.fillRect(x, y, pixelspace - 1, zero - y, color);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();

  if (scale)
    delete scale;
}

QString PlotLine::strip (double d)
{
  QString s = QString::number(d, 'f', 4);

  while (1)
  {
    if (s.find('.', -1, TRUE) != -1)
    {
      s.truncate(s.length() - 1);
      break;
    }
    else
    {
      if (s.find('0', -1, TRUE) != -1)
        s.truncate(s.length() - 1);
      else
        break;
    }
  }

  return s;
}

void PlotLine::setPointers (Scaler *s, QPixmap *p)
{
  scaler = s;
  buffer = p;
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

