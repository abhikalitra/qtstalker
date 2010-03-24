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

#include <QPainter>
#include <QString>
#include <QDateTime>

#define SCALE_WIDTH 70
#define DATE_HEIGHT 30

DatePlot::DatePlot (QWidget *w) : QWidget(w)
{
  scaleWidth = SCALE_WIDTH;
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
    
  switch (interval)
  {
    case Bar::Minute1:
    case Bar::Minute5:
    case Bar::Minute10:
    case Bar::Minute15:
    case Bar::Minute30:
    case Bar::Minute60:
      getMinuteDate(data);
      break;
    case Bar::WeeklyBar:
      getWeeklyDate(data);
      break;
    case Bar::MonthlyBar:
      getMonthlyDate(data);
      break;
    default:
      getDailyDate(data);
      break;
  }
}

void DatePlot::draw ()
{
  if (buffer.isNull())
    return;
  
  buffer.fill(backgroundColor);

  if (dateList.count() && isVisible())
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
      TickItem item = dateList.at(loop);
      
      if (item.flag)
      {
        if (! item.tick)
	{
	  // draw the short tick
          painter.drawLine (x, 1, x, 4);
          painter.drawText (x - (fm.width(item.text, -1) / 2),
	                    fm.height() + 2,
			    item.text);
	}
	else
	{
	  // draw the long tick
          painter.drawLine (x, 1, x, buffer.height() - fm.height() - 2);
          painter.drawText (x - (fm.width(item.text, -1) / 2),
	                    buffer.height() - 2,
			    item.text);
	}
      }
              
      x = x + pixelspace;
      loop++;
    }
    
    painter.end();
  }

  update();
}

void DatePlot::drawRefresh ()
{
  update();
}

void DatePlot::paintEvent (QPaintEvent *)
{
  QPainter p(this);
  p.drawPixmap(0, 0, buffer);
}

void DatePlot::resizeEvent (QResizeEvent *event)
{
  buffer = QPixmap(event->size());
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

void DatePlot::setInterval (Bar::BarLength d)
{
  interval = d;
}

void DatePlot::getMinuteDate (BarData *data)
{
  xGrid.resize(0);
  int loop = 0;
  Bar *bar = data->getBar(loop);
  QDateTime nextHour = bar->getBarDate();
  QDateTime oldDay = nextHour;
  nextHour.setTime(QTime(nextHour.time().hour(), 0, 0, 0));
  
//  if ((nextHour.getHour() % 2) == 0)
  if (interval != Bar::Minute1)
    nextHour = nextHour.addSecs(7200);
  else
    nextHour = nextHour.addSecs(3600);

  while(loop < (int) data->count())
  {
    bar = data->getBar(loop);
    QDateTime date = bar->getBarDate();
    
    TickItem item;
    item.flag = 0;
    
    if (date.date().day() != oldDay.date().day())
    {
      item.flag = 1;
      item.tick = 1;
      item.text = date.date().toString("MMM d");
      oldDay = date;
      
      xGrid.resize(xGrid.size() + 1);
      xGrid[xGrid.size() - 1] = loop;
    }
    else
    {
      if (date >= nextHour)
      {
        if (interval < Bar::Minute30)
	{
          item.flag = 1;
          item.tick = 0;
          item.text = QString::number(date.time().hour()) + ":00";
	  
          xGrid.resize(xGrid.size() + 1);
          xGrid[xGrid.size() - 1] = loop;
	}
      }
    }
    
    if (date >= nextHour)
    {
      nextHour = date;
      nextHour.setTime(QTime(date.time().hour(), 0, 0, 0));
//      if ((date.getHour() % 2) == 0)
      if (interval != Bar::Minute1)
        nextHour = nextHour.addSecs(7200);
      else
        nextHour = nextHour.addSecs(3600);
    }

    dateList.append(item);
    loop++;
  }
}

void DatePlot::getDailyDate (BarData *data)
{
  int loop = 0;
  xGrid.resize(0);

  Bar *bar = data->getBar(loop);
  QDateTime dt = bar->getBarDate();
  QDate oldDate = dt.date();
  QDate oldWeek = oldDate;
  oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

  while(loop < (int) data->count())
  {
    TickItem item;
    item.flag = 0;
  
    bar = data->getBar(loop);
    dt = bar->getBarDate();
    QDate date = dt.date();

    if (date.month() != oldDate.month())
    {
      item.flag = 1;
      item.tick = 1;
      item.text = date.toString("MMM-yy");
      oldDate = date;
      oldWeek = date;
      oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
      
      xGrid.resize(xGrid.size() + 1);
      xGrid[xGrid.size() - 1] = loop;
    }
    else
    {
      // if start of new week make a tick
      if (date > oldWeek)
      {
        item.flag = 1;
        item.tick = 0;
        item.text = date.toString("d");
  	oldWeek = date;
        oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
      }
    }

    dateList.append(item);
    loop++;
  }
}

void DatePlot::getWeeklyDate (BarData *data)
{
  xGrid.resize(0);
  int loop = 0;

  Bar *bar = data->getBar(loop);
  QDateTime dt = bar->getBarDate();
  QDate oldMonth = dt.date();

  while(loop < (int) data->count())
  {
    TickItem item;
    item.flag = 0;
  
    bar = data->getBar(loop);
    dt = bar->getBarDate();
    QDate date = dt.date();

    if (date.year() != oldMonth.year())
    {
      xGrid.resize(xGrid.size() + 1);
      xGrid[xGrid.size() - 1] = loop;
    }
    
    if (date.month() != oldMonth.month())
    {
      oldMonth = date;
      item.flag = 1;
      item.tick = 0;
      item.text = date.toString("MMM");
      item.text.chop(2);
      
      if (date.month() == 1)
      {
        item.tick = 1;
        item.text = date.toString("yy");
      }
    }
    
    dateList.append(item);
    loop++;
  }
}

void DatePlot::getMonthlyDate (BarData *data)
{
  xGrid.resize(0);
  int loop = 0;

  Bar *bar = data->getBar(loop);
  QDateTime dt = bar->getBarDate();
  QDate oldYear = dt.date();

  while(loop < (int) data->count())
  {
    TickItem item;
    item.flag = 0;
  
    bar = data->getBar(loop);
    dt = bar->getBarDate();
    QDate date = dt.date();

    if (date.year() != oldYear.year())
    {
      oldYear = date;
      item.flag = 1;
      item.tick = 1;
      item.text = date.toString("yy");
      
      xGrid.resize(xGrid.size() + 1);
      xGrid[xGrid.size() - 1] = loop;
    }

    dateList.append(item);
    loop++;
  }
}

QVector<int> & DatePlot::getXGrid ()
{
  return xGrid;
}

