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
  setBackgroundMode(NoBackground);
  scaleWidth = SCALE_WIDTH;
  startX = 2;
  backgroundColor.setNamedColor("black");
  borderColor.setNamedColor("white");
  pixelspace = 0;
  interval = BarData::DailyBar;
  startIndex = 0;
  data = 0;
//  setMouseTracking(TRUE);
  setFocusPolicy(QWidget::ClickFocus);

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(12);
  plotFont.setWeight(50);
  
  setMinimumHeight(DATE_HEIGHT);
  setMaximumHeight(DATE_HEIGHT);
  
  dateList.setAutoDelete(TRUE);
}

DatePlot::~DatePlot ()
{
}

void DatePlot::clear ()
{
  data = 0;
  dateList.clear();
}

void DatePlot::setData (BarData *l)
{
  if (! l->count())
    return;
    
  data = l;
  dateList.clear();
    
  if (l->getBarType() == BarData::Tick)
  { 
    switch (interval)
    {
      case BarData::Minute1:
      case BarData::Minute5:
      case BarData::Minute10:
      case BarData::Minute15:
      case BarData::Minute30:
      case BarData::Minute60:
        getMinuteDate();
        break;
      default:
        getDailyDate();
        break;
    }
  }
  else
  {
    switch (interval)
    {
      case BarData::WeeklyBar:
        getWeeklyDate();
        break;
      case BarData::MonthlyBar:
        getMonthlyDate();
        break;
      default:
        getDailyDate();
        break;
    }
  }
}

void DatePlot::draw ()
{
  buffer.fill(backgroundColor);

  if (dateList.count() && isShown())
  {
    QPainter painter;
    painter.begin(&buffer);
    painter.setPen(borderColor);
    painter.setFont(plotFont);

    QFontMetrics fm(plotFont);
    int x = startX;
    int loop = startIndex;
  
    // clear date area
    painter.fillRect(0, buffer.height(), buffer.width() - scaleWidth, buffer.height(), backgroundColor);

    // draw the seperator line
    painter.drawLine (0, 0, buffer.width() - scaleWidth, 0);

    while(x <= (buffer.width() - scaleWidth) && loop < (int) dateList.count())
    {
      TickItem *item = dateList.at(loop);
      
      if (item->flag)
      {
        if (! item->tick)
	{
	  // draw the short tick
          painter.drawLine (x, 1, x, 4);
	  
          painter.drawText (x - (fm.width(item->text, -1) / 2),
	                    fm.height() + 2,
			    item->text,
			    -1);
	}
	else
	{
	  // draw the long tick
          painter.drawLine (x, 1, x, buffer.height() - fm.height() - 2);
	  
          painter.drawText (x - (fm.width(item->text, -1) / 2),
	                    buffer.height() - 2,
			    item->text,
			    -1);
	}
      }
              
      x = x + pixelspace;
      loop++;
    }
    
    painter.end();
  }

  paintEvent(0);
}

void DatePlot::drawRefresh ()
{
  paintEvent(0);
}

void DatePlot::paintEvent (QPaintEvent *)
{
  bitBlt(this, 0, 0, &buffer);
}

void DatePlot::resizeEvent (QResizeEvent *event)
{
  buffer.resize(event->size());
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

void DatePlot::getMinuteDate ()
{
  int loop = 0;
  BarDate nextHour = data->getDate(loop);
  BarDate oldDay = data->getDate(loop);
  nextHour.setTime(nextHour.getHour(), 0, 0);
  if ((nextHour.getHour() % 2) == 0)
    nextHour.addSecs(7200);
  else
    nextHour.addSecs(3600);

  while(loop < (int) data->count())
  {
    BarDate date = data->getDate(loop);
    
    TickItem *item = new TickItem;
    item->flag = 0;
    
    if (date.getDate().day() != oldDay.getDate().day())
    {
      item->flag = 1;
      item->tick = 1;
      item->text = date.getDate().toString("MMM d");
      oldDay = date;
    }
    else
    {
      if (date.getDateValue() >= nextHour.getDateValue())
      {
        if (interval < BarData::Minute30)
	{
          item->flag = 1;
          item->tick = 0;
          item->text = QString::number(date.getHour()) + ":00";
	}
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

    dateList.append(item);
    loop++;
  }
}

void DatePlot::getDailyDate ()
{
  int loop = 0;

  QDate oldDate = data->getDate(loop).getDate();
  QDate oldWeek = oldDate;
  oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

  while(loop < (int) data->count())
  {
    TickItem *item = new TickItem;
    item->flag = 0;
  
    QDate date = data->getDate(loop).getDate();

    if (date.month() != oldDate.month())
    {
      item->flag = 1;
      item->tick = 1;
      item->text = date.toString("MMM'yy");
      oldDate = date;
      oldWeek = date;
      oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
    }
    else
    {
      // if start of new week make a tick
      if (date > oldWeek)
      {
        item->flag = 1;
        item->tick = 0;
        item->text = date.toString("d");
  	oldWeek = date;
        oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
      }
    }

    dateList.append(item);
    loop++;
  }
}

void DatePlot::getWeeklyDate ()
{
  QFontMetrics fm(plotFont);
  int loop = 0;

  QDate oldMonth = data->getDate(loop).getDate();

  while(loop < (int) data->count())
  {
    TickItem *item = new TickItem;
    item->flag = 0;
  
    QDate date = data->getDate(loop).getDate();

    if (date.month() != oldMonth.month())
    {
      oldMonth = date;
      item->flag = 1;
      item->tick = 0;
      item->text = date.toString("MMM");
      
      if (date.month() == 1)
      {
        item->tick = 1;
        item->text = date.toString("MMM'yy");
      }
    }

    dateList.append(item);
    loop++;
  }
}

void DatePlot::getMonthlyDate ()
{
  int loop = 0;
  QDate oldYear = data->getDate(loop).getDate();

  while(loop < (int) data->count())
  {
    TickItem *item = new TickItem;
    item->flag = 0;
  
    QDate date = data->getDate(loop).getDate();

    if (date.year() != oldYear.year())
    {
      oldYear = date;
      item->flag = 1;
      item->tick = 1;
      item->text = date.toString("yyyy");
    }

    dateList.append(item);
    loop++;
  }
}

