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

ScalePlot::ScalePlot ()
{
}

void ScalePlot::draw (PlotData &pd)
{
  QPainter painter;
  painter.begin(&pd.buffer);
  painter.setFont(pd.plotFont);
  painter.setPen(QPen(pd.borderColor, 1, Qt::SolidLine));

  QList<double> scaleArray;
  pd.scaler.scaleArray(scaleArray);
  
  QFontMetrics fm(pd.plotFont);

  int x = pd.buffer.width() - pd.scaleWidth;
  int loop;
  Utils util;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = pd.scaler.convertToY(scaleArray.at(loop));
    painter.drawLine (x, y, x + 4, y);

    // draw the text
    QString s;
    util.strip(scaleArray.at(loop), 4, s);
    
    // abbreviate too many (>=3) trailing zeroes in large numbers on y-axes
    bool flag = FALSE;
      
    if (s.toDouble() < 0)
    {
      flag = TRUE;
      s.remove(0, 1);
    }
      
    if (s.toDouble() >= 1000000000)
    {
      util.strip(s.toDouble() / 1000000000, 4, s);
      s.append("B");
    }
    else
    {
      if (s.toDouble() >= 1000000)
      {
        util.strip(s.toDouble() / 1000000, 4, s);
	s.append("M");
      }
    }
      
    if (flag)
      s.prepend("-");
    
    painter.drawText(x + 7, y + (fm.height() / 2), s);
  }

  // draw the vertical separator line
  painter.drawLine (x, 0, x, pd.buffer.height() - pd.dateHeight);

  painter.end();
}

void ScalePlot::drawPoints (PlotData &pd, QList<Setting> &points)
{
  QPainter painter;
  painter.begin(&pd.buffer);
  painter.setFont(pd.plotFont);

  QFontMetrics fm(pd.plotFont);

  int offset = 8;
  int x = pd.buffer.width() - pd.scaleWidth + 1;

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
    int y = pd.scaler.convertToY(v);

    QRect rc = painter.boundingRect(x + offset,
                                    y - (fm.height() / 2),
                                    1,
                                    1,
                                    0,
                                    d);

    // draw the left arrow portion of the box shape
    QPolygon arrow(4);
    arrow.setPoint(0, x, y);
    arrow.setPoint(1, x + offset, rc.top() - 1);
    arrow.setPoint(2, x + offset, rc.bottom() + 1);
    arrow.setPoint(3, x, y);
    painter.setBrush(color);
    painter.drawPolygon(arrow, Qt::OddEvenFill);
    
    painter.fillRect(rc, color);
    
    color.setRed(color.red() || pd.borderColor.red());
    color.setGreen(color.green() || pd.borderColor.green());
    color.setBlue(color.blue() || pd.borderColor.blue());
    painter.setPen(color);
    painter.drawText(rc, d);
  }
}

