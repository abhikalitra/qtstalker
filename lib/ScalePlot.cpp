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
#include "Strip.h"

#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QPolygon>
#include <QString>

ScalePlot::ScalePlot ()
{
}

void ScalePlot::draw (PlotData &pd, Indicator &ind)
{
  QPainter painter;
  painter.begin(&pd.buffer);
  painter.setFont(pd.plotFont);
  painter.setPen(QPen(pd.borderColor, 1, Qt::SolidLine));

  painter.fillRect(pd.buffer.width() - pd.scaleWidth,
                   0,
                   pd.scaleWidth,
                   pd.buffer.height(),
                   pd.backgroundColor);
  

  QList<double> scaleArray;
  Scaler scaler = ind.scaler();
  scaler.scaleArray(scaleArray);
  
  QFontMetrics fm(pd.plotFont);

  int x = pd.buffer.width() - pd.scaleWidth;
  int loop;
  Strip strip;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = scaler.convertToY(scaleArray.at(loop));
    painter.drawLine (x, y, x + 4, y);

    QString s;
    double v = scaleArray.at(loop);
    
    if (v < 1000000)
      strip.strip(v, 4, s);
    else
    {
      bool flag = FALSE;
      if (v < 0)
      {
        flag = TRUE;
        v = v * 1;
      }
      
      if (v >= 1000000 && v < 1000000000)
      {
        s = QString::number(v / 1000000, 'f', 2);
        while (s.endsWith("0"))
          s.chop(1);
        while (s.endsWith("."))
          s.chop(1);
        s.append("M");
        if (flag)
          s.prepend("-");
      }
      else if (v >= 1000000000)
      {
        s = QString::number(v / 1000000000, 'f', 2);
        while (s.endsWith("0"))
          s.chop(1);
        while (s.endsWith("."))
          s.chop(1);
        s.append("B");
        if (flag)
          s.prepend("-");
      }
    }

    painter.drawText(x + 7, y + (fm.height() / 2), s);
  }

  // draw the vertical separator line
  int height = pd.buffer.height();
  if (ind.date())
    height -= pd.dateHeight;
  
  painter.drawLine (x, 0, x, height);

  painter.end();
}

void ScalePlot::drawPoints (PlotData &pd, Indicator &indicator)
{
  QList<Setting> pl;
  points(pd, pl, indicator);
  
  QPainter painter;
  painter.begin(&pd.buffer);
  painter.setFont(pd.plotFont);

  QFontMetrics fm(pd.plotFont);

  int offset = 8;
  int x = pd.buffer.width() - pd.scaleWidth + 1;

  Scaler scaler = indicator.scaler();

  int loop;
  for (loop = 0; loop < pl.count(); loop++)
  {
    Setting set = pl.at(loop);
    QColor color;
    QString d;
    set.getData(0, d);
    color.setNamedColor(d);
    set.getData(1, d);
    double v = set.getDouble(1);
    int y = scaler.convertToY(v);

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

void ScalePlot::points (PlotData &pd, QList<Setting> &l, Indicator &indicator)
{
  QStringList plotList = indicator.plotOrder();

  Strip strip;
  int loop = 0;
  for (; loop < plotList.count(); loop++)
  {
    QString s = plotList.at(loop);
    PlotLine *line = indicator.line(s);
    
    if (line->type() == "Horizontal")
      continue;

    PlotLineBar *bar = line->data(pd.infoIndex);
    if (! bar)
      continue;

    QColor color = bar->color();
    Setting set;
    s = color.name();
    set.setData(0, s);

    strip.strip(bar->data(), 4, s);
    set.setData(1, s);

    l.append(set);
  }
}

