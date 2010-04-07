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

#include "DatePlot.h"

#include <QString>
#include <QPalette>
#include <QDebug>

#define DATE_HEIGHT 30

DatePlot::DatePlot (QWidget *w) : QWidget(w)
{
  startX = 2;
  backgroundColor.setNamedColor("black");
  borderColor.setNamedColor("white");
  pixelspace = 0;
  interval = Bar::DailyBar;
  startIndex = 0;
  setFocusPolicy(Qt::ClickFocus);

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(12);
  plotFont.setWeight(50);
  
  setMinimumHeight(DATE_HEIGHT);
  setMaximumHeight(DATE_HEIGHT);
  
  setAutoFillBackground(TRUE);
  setBackgroundColor(backgroundColor);
}

void DatePlot::clear ()
{
  dateList.clear();
}

void DatePlot::setData (BarData *data)
{
  if (! data->count())
    return;
    
  dateList.clear();
  
  int loop;
  for (loop = 0; loop < data->count(); loop++)
  {
    Bar *bar = data->getBar(loop);
    dateList.append(bar->getDate());
  }
   
  createPixmap();
}

void DatePlot::createPixmap ()
{
  if (! dateList.count())
    return;
  
  buffer = QPixmap(this->width() + startX + (dateList.count() * pixelspace), DATE_HEIGHT);

  // clear date area
  buffer.fill(backgroundColor);

  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  // draw the seperator line
  painter.drawLine (0, 0, buffer.width(), 0);

  switch (interval)
  {
    case Bar::Minute1:
    case Bar::Minute5:
    case Bar::Minute10:
    case Bar::Minute15:
    case Bar::Minute30:
    case Bar::Minute60:
      drawMinuteDate(painter);      
      break;
    case Bar::WeeklyBar:
      drawWeeklyDate(painter);      
      break;
    case Bar::MonthlyBar:
      drawMonthlyDate(painter);      
      break;
    default: // daily
      drawDailyDate(painter);      
      break;
  }
}

void DatePlot::draw ()
{
  update();
}

void DatePlot::drawRefresh ()
{
  update();
}

void DatePlot::paintEvent (QPaintEvent *)
{
  if (! dateList.count())
    return;
  
  QPainter p(this);
  int x = startX + (startIndex * pixelspace);
  p.drawPixmap(0, 0, buffer, x, 0, this->width(), buffer.height());
}

void DatePlot::resizeEvent (QResizeEvent *)
{
  createPixmap();
  paintEvent(0);
}

void DatePlot::setBackgroundColor (QColor d)
{
  backgroundColor = d;
  
  QPalette palet;
  palet.setColor(backgroundRole(), backgroundColor);
  setPalette(palet);   
}

void DatePlot::setBorderColor (QColor d)
{
  borderColor = d;
}

void DatePlot::setPlotFont (QFont d)
{
  plotFont = d;
}

void DatePlot::setPixelspace (int d)
{
  pixelspace = d;
  createPixmap();
}

void DatePlot::setIndex (int d)
{
  startIndex = d;
}

void DatePlot::setInterval (Bar::BarLength d)
{
  interval = d;
}

void DatePlot::drawMinuteDate (QPainter &painter)
{
  int loop = 0;
  QFontMetrics fm(plotFont);
  int x = startX;

  QDateTime nextHour = dateList.at(loop);
  QDateTime oldDay = nextHour;
  nextHour.setTime(QTime(nextHour.time().hour(), 0, 0, 0));
  
  if (interval != Bar::Minute1)
    nextHour = nextHour.addSecs(7200);
  else
    nextHour = nextHour.addSecs(3600);

  for (; loop < dateList.count(); loop++, x = x + pixelspace)
  {
    QDateTime date = dateList.at(loop);
    
    if (date.date().day() != oldDay.date().day())
    {
      oldDay = date;

      // big tick
      painter.drawLine (x, 1, x, buffer.height() - fm.height() - 2);
      QString text = date.date().toString("MMM d");
      painter.drawText (x - (fm.width(text, -1) / 2),
	                buffer.height() - 2,
			text);
    }
    else
    {
      if (date >= nextHour)
      {
        if (interval < Bar::Minute30)
	{
	  // draw the short tick
	  painter.drawLine (x, 1, x, 4);
          QString text = QString::number(date.time().hour()) + ":00";
          painter.drawText (x - (fm.width(text, -1) / 2),
	                    fm.height() + 2,
			    text);
	}
      }
    }
    
    if (date >= nextHour)
    {
      nextHour = date;
      nextHour.setTime(QTime(date.time().hour(), 0, 0, 0));
      if (interval != Bar::Minute1)
        nextHour = nextHour.addSecs(7200);
      else
        nextHour = nextHour.addSecs(3600);
    }
  }
}

void DatePlot::drawDailyDate (QPainter &painter)
{
  int loop = 0;
  QFontMetrics fm(plotFont);
  int x = startX;

  QDateTime dt = dateList.at(loop);
  QDate oldDate = dt.date();
  QDate oldWeek = oldDate;
  oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

  for (; loop < dateList.count(); loop++, x = x + pixelspace)
  {
    dt = dateList.at(loop);
    QDate date = dt.date();

    if (date.month() != oldDate.month())
    {
      oldDate = date;
      oldWeek = date;
      oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

      // draw the long tick
      painter.drawLine (x, 1, x, buffer.height() - fm.height() - 2);
      QString text = date.toString("MMM-yy");
      painter.drawText (x - (fm.width(text, -1) / 2),
	                buffer.height() - 2,
			text);
    }
    else
    {
      // if start of new week make a tick
      if (date > oldWeek)
      {
  	oldWeek = date;
        oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
	
	// draw the short tick
        painter.drawLine (x, 1, x, 4);
        QString text = date.toString("d");
        painter.drawText (x - (fm.width(text, -1) / 2),
	                  fm.height() + 2,
			  text);
      }
    }
  }
}

void DatePlot::drawWeeklyDate (QPainter &painter)
{
  int loop = 0;
  QFontMetrics fm(plotFont);
  int x = startX;

  QDateTime dt = dateList.at(loop);
  QDate oldMonth = dt.date();

  for (; loop < dateList.count(); loop++, x = x + pixelspace)
  {
    dt = dateList.at(loop);
    QDate date = dt.date();

    if (date.month() != oldMonth.month())
    {
      oldMonth = date;

      if (date.month() == 1)
      {
        // draw the long tick
        painter.drawLine (x, 1, x, buffer.height() - fm.height() - 2);
        QString text = date.toString("yyyy");
        painter.drawText (x - (fm.width(text, -1) / 2),
	                  buffer.height() - 2,
			  text);
      }
      else
      {
        // draw the short tick
        painter.drawLine (x, 1, x, 4);
        QString text = date.toString("MMM");
        text.chop(2);
        painter.drawText (x - (fm.width(text, -1) / 2),
	                  fm.height() + 2,
			  text);
      }
    }
  }
}

void DatePlot::drawMonthlyDate (QPainter &painter)
{
  int loop = 0;
  QFontMetrics fm(plotFont);
  int x = startX;

  QDateTime dt = dateList.at(loop);
  QDate oldYear = dt.date();

  for (; loop < dateList.count(); loop++, x = x + pixelspace)
  {
    dt = dateList.at(loop);
    QDate date = dt.date();

    if (date.year() != oldYear.year())
    {
      oldYear = date;
      
      // draw the long tick
      painter.drawLine (x, 1, x, buffer.height() - fm.height() - 2);
      QString text = date.toString("yyyy");
      painter.drawText (x - (fm.width(text, -1) / 2),
	                buffer.height() - 2,
			text);
    }
  }
}

