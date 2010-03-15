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

#include "ScalePlot.h"
#include "Utils.h"

#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QPolygon>
#include <QString>
#include <QVector>

#define SCALE_WIDTH 70

ScalePlot::ScalePlot (QWidget *w) : QWidget(w)
{
  scaleWidth = SCALE_WIDTH;
  backgroundColor.setNamedColor("black");
  borderColor.setNamedColor("white");
  mainFlag = FALSE;
  scaleToScreen = FALSE;
  logScale = FALSE;
  close = 0;
  activeFlag = FALSE;

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(12);
  plotFont.setWeight(50);
  
  setMinimumWidth(scaleWidth);
  setMaximumWidth(scaleWidth);
}

void ScalePlot::clear ()
{
  points.clear();
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
  if (buffer.isNull())
    return;
  
  buffer.fill(backgroundColor);

  if (activeFlag)
    drawScale();

  update();
}

void ScalePlot::drawRefresh ()
{
  update();
}

void ScalePlot::paintEvent (QPaintEvent *)
{
  QPainter p(this);
  p.drawPixmap(0, 0, buffer);
}

void ScalePlot::resizeEvent (QResizeEvent *event)
{
  buffer = QPixmap(event->size());
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
  painter.begin(&buffer);
  painter.setFont(plotFont);
  painter.setPen(QPen(borderColor, 1, Qt::SolidLine));

  painter.fillRect(0, 0, buffer.width(), buffer.height(), backgroundColor);
  
  QVector<double> scaleArray;
  scaler.getScaleArray(scaleArray);
  
  QFontMetrics fm(plotFont);

  int x = 0;
  int loop;
  Utils util;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = scaler.convertToY(scaleArray[loop]);
    painter.drawLine (x, y, x + 4, y);

    // draw the text
    QString s;
    util.strip(scaleArray[loop], 4, s);
    
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
        util.strip(s.toDouble() / 1000000000, 4, s);
	s.append("b");
      }
      else
      {
        if (s.toDouble() >= 1000000)
        {
          util.strip(s.toDouble() / 1000000, 4, s);
	  s.append("m");
        }
      }
      
      if (flag)
        s.prepend("-");
    }
    
    painter.drawText(x + 7, y + (fm.height() / 2), s);
  }

  painter.drawLine (x, 0, x, buffer.height());

  drawPoints(painter);

  painter.end();
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

void ScalePlot::setScaler (Scaler &d)
{
  scaler = d;
}

void ScalePlot::setScalePoints (QList<Setting> &d)
{
  points = d;
  activeFlag = TRUE;
}

void ScalePlot::drawPoints (QPainter &painter)
{
  QFontMetrics fm(plotFont);

  int loop;
  for (loop = 0; loop < points.count(); loop++)
  {
    Setting set = points.at(loop);
    QColor color;
    QString d;
    set.getData(0, d);
    color.setNamedColor(d);
    set.getData(1, d);
    double v = set.getDouble(1);
    
    int y = scaler.convertToY(v);
    QRect rc = painter.boundingRect(1, y - (fm.height() / 2), 1, 1, 0, d);
    painter.fillRect(rc, color);
    
    color.setRed(color.red() || borderColor.red());
    color.setGreen(color.green() || borderColor.green());
    color.setBlue(color.blue() || borderColor.blue());
    painter.setPen(color);
    painter.drawText(rc, d);
  }
}

