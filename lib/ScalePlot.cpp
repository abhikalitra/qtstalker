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

#include "ScalePlot.h"
#include <qpainter.h>
#include <qpen.h>
#include <qpoint.h>
#include <qpointarray.h>
#include <math.h>
#include <qpaintdevicemetrics.h>
#include <qstring.h>
#include <qmemarray.h>

#define SCALE_WIDTH 60

ScalePlot::ScalePlot (QWidget *w) : QWidget(w)
{
  buffer = new QPixmap;
  setBackgroundMode(NoBackground);
  scaleWidth = SCALE_WIDTH;
  backgroundColor.setNamedColor("black");
  borderColor.setNamedColor("white");
  mainFlag = FALSE;
  scaleToScreen = FALSE;
  logScale = FALSE;
  mainHigh = -99999999;
  mainLow = 99999999;
  data = 0;
  scaler = 0;

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(12);
  plotFont.setWeight(50);
  
  setMinimumWidth(scaleWidth);
  setMaximumWidth(scaleWidth);
}

ScalePlot::~ScalePlot ()
{
  delete buffer;
}

void ScalePlot::clear ()
{
  mainHigh = -99999999;
  mainLow = 99999999;
  data = 0;
}

void ScalePlot::setData (BarData *l)
{
  if (! l->count())
    return;

  data = l;
  
  if (mainFlag)
  {
    mainHigh = data->getMax();
    mainLow = data->getMin();
  }
}

void ScalePlot::setMainFlag (bool d)
{
  mainFlag = d;
}

void ScalePlot::setScaleToScreen (bool d)
{
  scaleToScreen = d;
}

void ScalePlot::setLogScale (bool d)
{
  logScale = d;
}

void ScalePlot::draw ()
{
  buffer->fill(backgroundColor);

  if (data)
    drawScale();

  paintEvent(0);
}

void ScalePlot::drawRefresh ()
{
  paintEvent(0);
}

void ScalePlot::paintEvent (QPaintEvent *)
{
  bitBlt(this, 0, 0, buffer);
}

void ScalePlot::resizeEvent (QResizeEvent *event)
{
  buffer->resize(event->size());
  draw();
}

void ScalePlot::setScaleWidth (int d)
{
  if (d > 999 || d < SCALE_WIDTH)
    return;
  else
    scaleWidth = d;
}

void ScalePlot::setBackgroundColor (QColor &d)
{
  backgroundColor = d;
}

void ScalePlot::setBorderColor (QColor &d)
{
  borderColor = d;
}

void ScalePlot::setPlotFont (QFont &d)
{
  plotFont = d;
}

void ScalePlot::drawScale ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();

  painter.fillRect(0, 0, buffer->width(), buffer->height(), backgroundColor);
  
  QMemArray<double> scaleArray;
  scaler->getScaleArray(scaleArray);

  int x = 0;
  int loop;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = scaler->convertToY(scaleArray[loop]);
    painter.setPen(QPen(borderColor, 1, QPen::SolidLine));
    painter.drawLine (x, y, x + 4, y);

    // draw the text
    QString s;
    strip(scaleArray[loop], 4, s);
    
    // abbreviate too many (>=3) trailing zeroes in large numbers on y-axes
    if (! mainFlag)
    {
      bool flag = FALSE;
      
      if (s.toDouble() < 0)
      {
        flag = TRUE;
	s.remove(0, 1);  
      }
      
      if (s.toDouble() >= 1000000000)
      {
        strip(s.toDouble() / 1000000000, 4, s);
	s.append("b");
      }
      else
      {
        if (s.toDouble() >= 1000000)
        {
          strip(s.toDouble() / 1000000, 4, s);
	  s.append("m");
        }
        else
        {
          if (s.toDouble() >= 1000)
          {
            strip(s.toDouble() / 1000, 4, s);
	    s.append("k");
	  }
	}
      }
      
      if (flag)
        s.prepend("-");
    }
    
    painter.drawText(x + 7, y + (fm.height() / 2), s);
  }

  painter.setPen(QPen(borderColor, 1, QPen::SolidLine));
  painter.drawLine (x, 0, x, buffer->height());
  
  // draw the last value pointer on the scale of main plot
  if (mainFlag)
  {
    int y = scaler->convertToY(data->getClose(data->count() - 1));
    
    QPointArray array;
    array.setPoints(3, x + 2, y,
                    x + 8, y - 4,
	            x + 8, y + 4);
    painter.setBrush(borderColor);
    painter.drawPolygon(array, TRUE, 0, -1);
  }

  painter.end();
}

void ScalePlot::strip (double d, int p, QString &s)
{
  s = QString::number(d, 'f', p);

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
}

void ScalePlot::slotScaleToScreenChanged (bool d)
{
  setScaleToScreen(d);
  draw();
}

void ScalePlot::slotLogScaleChanged (bool d)
{
  setLogScale(d);
  draw();
}

void ScalePlot::setScaler (Scaler *d)
{
  scaler = d;
}

