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

#include "DatePlot.h"
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qstring.h>

#define SCALE_WIDTH 60

DatePlot::DatePlot (QWidget *w) : QWidget(w)
{
  buffer = new QPixmap;
  setBackgroundMode(NoBackground);
  scaleWidth = SCALE_WIDTH;
  dateHeight = DATE_HEIGHT;
  startX = 2;
  backgroundColor.setNamedColor("black");
  borderColor.setNamedColor("white");
  pixelspace = 0;
  interval = BarData::DailyBar;
  startIndex = 0;
  data = 0;
  setMouseTracking(TRUE);
  setFocusPolicy(QWidget::ClickFocus);

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(12);
  plotFont.setWeight(50);
  
  setMinimumHeight(dateHeight);
  setMaximumHeight(dateHeight);
}

DatePlot::~DatePlot ()
{
  delete buffer;
}

void DatePlot::clear ()
{
  data = 0;
}

void DatePlot::setData (BarData *l)
{
  if (! l->count())
    return;

  data = l;
}

void DatePlot::draw ()
{
  buffer->fill(backgroundColor);

  if (data)
  {
    if (isShown())
      drawDate();
  }

  paintEvent(0);
}

void DatePlot::drawRefresh ()
{
  paintEvent(0);
}

void DatePlot::paintEvent (QPaintEvent *)
{
  bitBlt(this, 0, 0, buffer);
}

void DatePlot::resizeEvent (QResizeEvent *event)
{
  buffer->resize(event->size());
  draw();
}

void DatePlot::setBackgroundColor (QColor d)
{
  backgroundColor = d;
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
}

void DatePlot::setIndex (int d)
{
  startIndex = d;
}

void DatePlot::setInterval (BarData::BarCompression d)
{
  interval = d;
}

void DatePlot::drawDate ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  
  // clear date area
  painter.fillRect(0, buffer->height(), buffer->width() - scaleWidth, dateHeight, backgroundColor);

  // draw the seperator line
  painter.drawLine (0, 0, buffer->width() - scaleWidth, 0);

  painter.end();
  
  if (data->getBarType() == BarData::Tick)
  { 
    switch (interval)
    {
      case BarData::Minute5:
      case BarData::Minute15:
        draw15Date();
        break;
      case BarData::Minute30:
      case BarData::Minute60:
        drawHourlyDate();
        break;
      case BarData::WeeklyBar:
        drawWeeklyDate();
        break;
      case BarData::MonthlyBar:
        drawMonthlyDate();
        break;
      default:
        drawDailyDate();
        break;
    }
  }
  else
  {
    switch (interval)
    {
      case BarData::WeeklyBar:
        drawWeeklyDate();
        break;
      case BarData::MonthlyBar:
        drawMonthlyDate();
        break;
      default:
        drawDailyDate();
        break;
    }
  }
}

void DatePlot::draw15Date ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();
  
  int x = startX;
  int loop = startIndex;

  BarDate nextHour = data->getDate(loop);
  BarDate oldDay = data->getDate(loop);
  nextHour.setTime(nextHour.getHour(), 0, 0);
  if ((nextHour.getHour() % 2) == 0)
    nextHour.addSecs(7200);
  else
    nextHour.addSecs(3600);

  while(x <= (buffer->width() - scaleWidth) && loop < (int) data->count())
  {
    BarDate date = data->getDate(loop);
    
    if (date.getDate().day() != oldDay.getDate().day())
    {
      painter.drawLine (x, 1, x, dateHeight - fm.height() - 1);
      QString s = date.getDate().toString("d");
      painter.drawText (x - (fm.width(s, -1) / 2), buffer->height() - 2, s, -1);
      oldDay = date;
    }
    else
    {
      // every 2 hours make a small tick
      if (date.getDateValue() >= nextHour.getDateValue())
      {
        painter.drawLine (x, 1, x, 4);

        QString s;
        if (date.getHour() >= 12)
	{
	  if (date.getHour() == 12)
            s = QString::number(date.getHour());
	  else
            s = QString::number(date.getHour() - 12);	
	  s.append("p");
	}
	else
	{
          s = QString::number(date.getHour());	
	  s.append("a");
	}
        painter.drawText (x - (fm.width(s, -1) / 2),
		          buffer->height() - dateHeight + fm.height() + 1,
			  s,
			  -1);
      }
    }
    
    if (date.getDateValue() >= nextHour.getDateValue())
    {
      nextHour = date;
      nextHour.setTime(date.getHour(), 0, 0);
      if ((date.getHour() % 2) == 0)
        nextHour.addSecs(7200);
      else
        nextHour.addSecs(3600);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void DatePlot::drawHourlyDate ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();
  
  int x = startX;
  int loop = startIndex;

  QDate oldDay = data->getDate(loop).getDate();

  while(x <= (buffer->width() - scaleWidth) && loop < (int) data->count())
  {
    QDate date = data->getDate(loop).getDate();

    if (date.day() != oldDay.day())
    {
      oldDay = date;
      painter.drawLine (x, 1, x, dateHeight / 3);
      QString s = date.toString("d");
      painter.drawText (x - (fm.width(s, -1) / 2),
	  		(dateHeight / 3) + fm.height() + 2,
			s,
			-1);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void DatePlot::drawDailyDate ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();
  
  int x = startX;
  int loop = startIndex;

  QDate oldDate = data->getDate(loop).getDate();
  QDate oldWeek = oldDate;
  oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

  while(x <= (buffer->width() - scaleWidth) && loop < (int) data->count())
  {
    QDate date = data->getDate(loop).getDate();

    if (date.month() != oldDate.month())
    {
      oldDate = date;

      painter.drawLine (x, 1, x, dateHeight - fm.height() - 1);

      QString s = date.toString("MMM'yy");
      painter.drawText (x - (fm.width(s, -1) / 2), buffer->height() - 2, s, -1);

      oldWeek = date;
      oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
    }
    else
    {
      // if start of new week make a tick
      if (date > oldWeek)
      {
        painter.drawLine (x, 1, x, 4);

        QString s = date.toString("d");
        painter.drawText (x - (fm.width(s, -1) / 2),
			  buffer->height() - dateHeight + fm.height() + 1,
			  s,
			  -1);

  	oldWeek = date;
        oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
      }
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void DatePlot::drawWeeklyDate ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();
  
  int x = startX;
  int loop = startIndex;

  QDate oldMonth = data->getDate(loop).getDate();

  while(x <= (buffer->width() - scaleWidth) && loop < (int) data->count())
  {
    QDate date = data->getDate(loop).getDate();

    if (date.month() != oldMonth.month())
    {
      oldMonth = date;

      painter.drawLine (x, 1, x, dateHeight / 3);

      QString s;
      s = date.toString("MMM'yy");

      if (fm.width(s, -1) > 4 * pixelspace)
      {
        if (date.month() == 1)
          s = date.toString("yy");
	else
	{
          s = date.toString("MMM");
          s.truncate(1);
	}
      }

      painter.drawText (x - (fm.width(s, -1) / 2),
  		        (dateHeight / 3) + fm.height() + 2,
			s,
			-1);
    }

    x = x + pixelspace;
    loop++;
  }
  
  painter.end();
}

void DatePlot::drawMonthlyDate ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();
  
  int x = startX;
  int loop = startIndex;

  QDate oldYear = data->getDate(loop).getDate();

  while(x <= (buffer->width() - scaleWidth) && loop < (int) data->count())
  {
    QDate date = data->getDate(loop).getDate();

    if (date.year() != oldYear.year())
    {
      oldYear = date;
      painter.drawLine (x, 1, x, dateHeight / 3);
      QString s = date.toString("yyyy");
      painter.drawText (x - (fm.width(s, -1) / 2),
	  		(dateHeight / 3) + fm.height() + 2,
			s,
			-1);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

