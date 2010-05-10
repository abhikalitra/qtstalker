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
#include <QDebug>
#include <QPainter>

DatePlot::DatePlot ()
{
}

void DatePlot::draw (PlotData &pd)
{
  QPainter painter(&pd.buffer);
  painter.setPen(pd.borderColor);
  
  // draw the seperator line
  painter.drawLine (0,
                    pd.buffer.height() - pd.dateHeight,
                    pd.buffer.width() - pd.scaleWidth,
                    pd.buffer.height() - pd.dateHeight);

  switch (pd.interval)
  {
    case Bar::Minute1:
    case Bar::Minute5:
    case Bar::Minute10:
    case Bar::Minute15:
    case Bar::Minute30:
    case Bar::Minute60:
      drawMinuteDate(pd, painter);      
      break;
    case Bar::WeeklyBar:
      drawWeeklyDate(pd, painter);      
      break;
    case Bar::MonthlyBar:
      drawMonthlyDate(pd, painter);      
      break;
    default: // daily
      drawDailyDate(pd, painter);      
      break;
  }

  painter.end();
}

void DatePlot::drawMinuteDate (PlotData &pd, QPainter &painter)
{
  int loop = pd.startIndex;
  QFontMetrics fm(pd.plotFont);
  int x = pd.startX;

  QDateTime nextHour;
  pd.dateBars.getDate(loop, nextHour);
  
  QDateTime oldDay = nextHour;
  nextHour.setTime(QTime(nextHour.time().hour(), 0, 0, 0));
  
  if (pd.interval != Bar::Minute1)
    nextHour = nextHour.addSecs(7200);
  else
    nextHour = nextHour.addSecs(3600);

  while((x < pd.buffer.width() - pd.scaleWidth) && (loop < pd.dateBars.count()))
  {
    QDateTime date;
    pd.dateBars.getDate(loop, date);
    
    if (date.date().day() != oldDay.date().day())
    {
      oldDay = date;

      // big tick
      painter.drawLine (x,
                        pd.buffer.height() - pd.dateHeight,
                        x,
                        pd.buffer.height() - fm.height() - 2);
      QString text = date.date().toString("MMM d");
      painter.drawText (x - (fm.width(text, -1) / 2),
	                pd.buffer.height() - 2,
			text);
    }
    else
    {
      if (date >= nextHour)
      {
        if (pd.interval < Bar::Minute30)
	{
	  // draw the short tick
	  painter.drawLine (x,
                            pd.buffer.height() - pd.dateHeight,
                            x,
                            pd.buffer.height() - pd.dateHeight + 4);
          QString text = QString::number(date.time().hour()) + ":00";
          painter.drawText (x - (fm.width(text, -1) / 2),
	                    pd.buffer.height() - pd.dateHeight + fm.height() + 2,
			    text);
	}
      }
    }
    
    if (date >= nextHour)
    {
      nextHour = date;
      nextHour.setTime(QTime(date.time().hour(), 0, 0, 0));
      if (pd.interval != Bar::Minute1)
        nextHour = nextHour.addSecs(7200);
      else
        nextHour = nextHour.addSecs(3600);
    }
    
    loop++;
    x += pd.pixelspace;
  }
}

void DatePlot::drawDailyDate (PlotData &pd, QPainter &painter)
{
  int loop = pd.startIndex;
  QFontMetrics fm(pd.plotFont);
  int x = pd.startX;

  QDateTime dt;
  pd.dateBars.getDate(loop, dt);
  
  QDate oldDate = dt.date();
  QDate oldWeek = oldDate;
  oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

  while((x < pd.buffer.width() - pd.scaleWidth) && (loop < pd.dateBars.count()))
  {
    pd.dateBars.getDate(loop, dt);
    QDate date = dt.date();

    if (date.month() != oldDate.month())
    {
      oldDate = date;
      oldWeek = date;
      oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

      // draw the long tick
      painter.drawLine (x,
                        pd.buffer.height() - pd.dateHeight,
                        x,
                        pd.buffer.height() - fm.height() - 2);
      QString text = date.toString("MMM-yy");
      painter.drawText (x - (fm.width(text, -1) / 2),
	                pd.buffer.height() - 2,
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
        painter.drawLine (x,
                          pd.buffer.height() - pd.dateHeight,
                          x,
                          pd.buffer.height() - pd.dateHeight + 4);
        QString text = date.toString("d");
        painter.drawText (x - (fm.width(text, -1) / 2),
	                  pd.buffer.height() - pd.dateHeight + fm.height() + 2,
			  text);
      }
    }
    
    loop++;
    x += pd.pixelspace;
  }
}

void DatePlot::drawWeeklyDate (PlotData &pd, QPainter &painter)
{
  int loop = pd.startIndex;
  QFontMetrics fm(pd.plotFont);
  int x = pd.startX;

  QDateTime dt;
  pd.dateBars.getDate(loop, dt);
  
  QDate oldMonth = dt.date();

  while((x < pd.buffer.width() - pd.scaleWidth) && (loop < pd.dateBars.count()))
  {
    pd.dateBars.getDate(loop, dt);
    QDate date = dt.date();

    if (date.month() != oldMonth.month())
    {
      oldMonth = date;

      if (date.month() == 1)
      {
        // draw the long tick
        painter.drawLine (x,
                          pd.buffer.height() - pd.dateHeight,
                          x,
                          pd.buffer.height() - fm.height() - 2);
        QString text = date.toString("yyyy");
        painter.drawText (x - (fm.width(text, -1) / 2),
	                  pd.buffer.height() - 2,
			  text);
      }
      else
      {
        // draw the short tick
        painter.drawLine (x,
                          pd.buffer.height() - pd.dateHeight,
                          x,
                          pd.buffer.height() - pd.dateHeight + 4);
        QString text = date.toString("MMM");
        text.chop(2);
        painter.drawText (x - (fm.width(text, -1) / 2),
	                  pd.buffer.height() - pd.dateHeight + fm.height() + 2,
			  text);
      }
    }
    
    loop++;
    x += pd.pixelspace;
  }
}

void DatePlot::drawMonthlyDate (PlotData &pd, QPainter &painter)
{
  int loop = pd.startIndex;
  QFontMetrics fm(pd.plotFont);
  int x = pd.startX;

  QDateTime dt;
  pd.dateBars.getDate(loop, dt);
  
  QDate oldYear = dt.date();

  while((x < pd.buffer.width() - pd.scaleWidth) && (loop < pd.dateBars.count()))
  {
    pd.dateBars.getDate(loop, dt);
    QDate date = dt.date();

    if (date.year() != oldYear.year())
    {
      oldYear = date;
      
      // draw the long tick
      painter.drawLine (x,
                        pd.buffer.height() - pd.dateHeight,
                        x,
                        pd.buffer.height() - fm.height() - 2);
      QString text = date.toString("yyyy");
      painter.drawText (x - (fm.width(text, -1) / 2),
	                pd.buffer.height() - 2,
			text);
    }
    
    loop++;
    x += pd.pixelspace;
  }
}

