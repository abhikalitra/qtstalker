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

#ifndef DATE_PLOT_HPP
#define DATE_PLOT_HPP

#include <QWidget>
#include <QPixmap>
#include <QColor>
#include <QFont>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QList>
#include <QPainter>
#include <QDateTime>

#include "BarData.h"

class DatePlot : public QWidget
{
  Q_OBJECT

  public:
    DatePlot (QWidget *);
    void clear ();
    void setData (BarData *);
    void drawDailyDate (QPainter &);
    void drawWeeklyDate (QPainter &);
    void drawMonthlyDate (QPainter &);
    void drawMinuteDate (QPainter &);


  public slots:
    void draw();
    void drawRefresh();
    void setPixelspace (int);
    void setBackgroundColor (QColor);
    void setBorderColor (QColor);
    void setPlotFont (QFont);
    void setIndex (int);
    void setInterval(Bar::BarLength);

  protected:
    virtual void paintEvent (QPaintEvent *);
    virtual void resizeEvent (QResizeEvent *);

  private:
    QFont plotFont;
    QPixmap buffer;
    int pixelspace;
    int startX;
    int startIndex;
    Bar::BarLength interval;
    QColor backgroundColor;
    QColor borderColor;
    QList<QDateTime> dateList;
};

#endif


