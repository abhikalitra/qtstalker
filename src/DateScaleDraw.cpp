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

#include "DateScaleDraw.h"
#include "BarLength.h"
#include "Globals.h"

#include <QString>
#include <QDebug>

DateScaleDraw::DateScaleDraw ()
{
  _barLength = -1;
}

void DateScaleDraw::setDates ()
{
  _dateList.clear();
  _data.clear();

  _barLength = g_barData->barLength();

  int loop = 0;
  for (; loop < g_barData->count(); loop++)
  {
    Bar *bar = g_barData->bar(loop);
    if (! bar)
      continue;

    QString s;
    bar->rangeKey(s);

    _data.insert(s, loop);

    _dateList.append(bar->date());
  }

//  QwtValueList vlist[3];
//  vlist[0] << 6 << 18 << 30;
//  vlist[1] << 12 << 36;
//  vlist[2] << 0 << 24;

//  QwtScaleDiv sd(0, 300, vlist);
//  setScaleDiv(sd);
  
//  getTicks();
}

int DateScaleDraw::count ()
{
  return _dateList.count();
}

QwtText DateScaleDraw::label (double v) const
{
  int t = (int) v;
  if (t < 1 || t >= _dateList.count())
    return QString();

  QwtText date;

  switch ((BarLength::Length) _barLength)
  {
    case BarLength::_MINUTE1:
    case BarLength::_MINUTE5:
    case BarLength::_MINUTE10:
    case BarLength::_MINUTE15:
    case BarLength::_MINUTE30:
    case BarLength::_MINUTE60:
      date = _dateList.at(t).toString("d h:m");
      break;
    case BarLength::_DAILY:
      if (_dateList.at(t).date().month() == 1)
        date = _dateList.at(t).toString("yyyy");
      else
        date = _dateList.at(t).toString("MMM");
      break;
    case BarLength::_WEEKLY:
      if (_dateList.at(t).date().month() == 1)
        date = _dateList.at(t).toString("yyyy");
      else
      {
        QString s = _dateList.at(t).toString("MMM");
        s.chop(2);
        date = s;
      }
      break;
    case BarLength::_MONTHLY:
      date = _dateList.at(t).toString("yyyy");
      break;
    default:
      break;
  }

  return date;
}

void DateScaleDraw::date (int x, QDateTime &dt)
{
  if (x < 0 || x >= _dateList.count())
    return;
  
  dt = _dateList.at(x);
}

int DateScaleDraw::x (QDateTime d)
{
  int x = -1;
  Bar bar;
  bar.setDateRange(d, (BarLength::Length) _barLength);

  QString s;
  bar.rangeKey(s);
  if (_data.contains(s))
    x = _data.value(s);

  return x;
}

void DateScaleDraw::info (int index, Setting &set)
{
  QDateTime dt;
  date(index, dt);
  if (dt.isValid())
  {
    set.setData("D", dt.toString("yyyy-MM-dd"));
    set.setData("T", dt.toString("HH:mm:ss"));
  }
}

QList<QDateTime> & DateScaleDraw::dates ()
{
  return _dateList;
}

void DateScaleDraw::draw (QPainter *painter, const QPalette &) const
{
  if (! _dateList.count())
    return;

  QwtScaleDiv sd = scaleDiv();
  int loop = sd.lowerBound();
  int size = sd.upperBound();
  if (size > _dateList.count())
    size = _dateList.count();

  QDate oldDate = _dateList.at(loop).date();
  QDate oldWeek = oldDate;
  oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
  QDate oldMonth = oldDate;
  QDate oldYear = oldDate;

  QDateTime nextHour = _dateList.at(loop);
  QDateTime oldDay = nextHour;
  nextHour.setTime(QTime(nextHour.time().hour(), 0, 0, 0));
  if ((BarLength::Length) _barLength != BarLength::_MINUTE1)
    nextHour = nextHour.addSecs(7200);
  else
    nextHour = nextHour.addSecs(3600);

  for (; loop < size; loop++)
  {
    _dateString.clear();
    
    switch ((BarLength::Length) _barLength)
    {
      case BarLength::_MINUTE1:
      case BarLength::_MINUTE5:
      case BarLength::_MINUTE10:
      case BarLength::_MINUTE15:
      case BarLength::_MINUTE30:
      case BarLength::_MINUTE60:
      {
        QDateTime date = _dateList.at(loop);
        if (date.date().day() != oldDay.date().day())
        {
          oldDay = date;

          // big tick
          drawTick(painter, loop, 10);
          drawLabel(painter, loop);
          //QString text = date.date().toString("MMM d");
        }
        else
        {
          if (date >= nextHour)
          {
            if ((BarLength::Length) _barLength < BarLength::_MINUTE30)
            {
              // draw the short tick
              drawTick(painter, loop, 4);
              //QString text = QString::number(date.time().hour()) + ":00";
            }
          }
        }

        if (date >= nextHour)
        {
          nextHour = date;
          nextHour.setTime(QTime(date.time().hour(), 0, 0, 0));
          if ((BarLength::Length) _barLength != BarLength::_MINUTE1)
            nextHour = nextHour.addSecs(7200);
          else
            nextHour = nextHour.addSecs(3600);
        }
        break;
      }
      case BarLength::_DAILY:
      {
        QDate date = _dateList.at(loop).date();
        if (date.month() != oldDate.month())
        {
          oldDate = date;
          oldWeek = date;
          oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

          // draw the long tick
          drawTick(painter, loop, 10);
          drawLabel(painter, loop);
          // QString text = date.toString("MMM-yy");
        }
        else
        {
          // if start of new week make a tick
          if (date > oldWeek)
          {
            oldWeek = date;
            oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

            // draw the short week tick
            drawTick(painter, loop, 4);
//            drawLabel(painter, loop);
            //QString text = date.toString("d");
          }
        }
        break;
      }
      case BarLength::_WEEKLY:
      {
        QDate date = _dateList.at(loop).date();
        if (date.month() != oldMonth.month())
        {
          oldMonth = date;
          if (date.month() == 1)
          {
            // draw the long tick
            drawTick(painter, loop, 10);
            drawLabel(painter, loop);
            //QString text = date.toString("yyyy");
          }
          else
          {
            // draw the short tick
            drawTick(painter, loop, 4);
            drawLabel(painter, loop);
            //QString text = date.toString("MMM");
            //text.chop(2);
          }
        }
        break;
      }
      case BarLength::_MONTHLY:
      {
        QDate date = _dateList.at(loop).date();
        if (date.year() != oldYear.year())
        {
          oldYear = date;

          // draw the long tick
          drawTick(painter, loop, 10);
          drawLabel(painter, loop);
          //QString text = date.toString("yyyy");
        }
        break;
      }
      default:
        break;
    }
  }
}

/*
void DateScaleDraw::getTicks ()
{
  if (! _dateList.count())
    return;

  _tickList.clear();
  
  int loop = 0;
  int size = _dateList.count();

  QDate oldDate = _dateList.at(loop).date();
  QDate oldWeek = oldDate;
  oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
  QDate oldMonth = oldDate;
  QDate oldYear = oldDate;

  QDateTime nextHour = _dateList.at(loop);
  QDateTime oldDay = nextHour;
  nextHour.setTime(QTime(nextHour.time().hour(), 0, 0, 0));
  if ((BarData::BarLength) _barLength != BarData::Minute1)
    nextHour = nextHour.addSecs(7200);
  else
    nextHour = nextHour.addSecs(3600);

  for (; loop < size; loop++)
  {
    switch ((BarData::BarLength) _barLength)
    {
      case BarData::Minute1:
      case BarData::Minute5:
      case BarData::Minute10:
      case BarData::Minute15:
      case BarData::Minute30:
      case BarData::Minute60:
      {
        QDateTime date = _dateList.at(loop);
        if (date.date().day() != oldDay.date().day())
        {
          oldDay = date;

          // big tick
          Setting tick;
          tick.setData("TYPE", QString("0"));
          tick.setData("TEXT", date.toString("d"));
          _tickList.insert(loop, tick);
        }
        else
        {
          if (date >= nextHour)
          {
            if ((BarData::BarLength) _barLength < BarData::Minute30)
            {
              // draw the short tick
              Setting tick;
              tick.setData("TYPE", QString("1"));
              tick.setData("TEXT", date.toString("h:m"));
              _tickList.insert(loop, tick);
            }
          }
        }

        if (date >= nextHour)
        {
          nextHour = date;
          nextHour.setTime(QTime(date.time().hour(), 0, 0, 0));
          if ((BarData::BarLength) _barLength != BarData::Minute1)
            nextHour = nextHour.addSecs(7200);
          else
            nextHour = nextHour.addSecs(3600);
        }
        break;
      }
      case BarData::DailyBar:
      {
        QDate date = _dateList.at(loop).date();
        if (date.month() != oldDate.month())
        {
          oldDate = date;
          oldWeek = date;
          oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

          // draw the long tick
          Setting tick;
          tick.setData("TYPE", QString("0"));
          if (date.month() == 1)
            tick.setData("TEXT", date.toString("yy"));
          else
            tick.setData("TEXT", date.toString("MMM"));
          _tickList.insert(loop, tick);
        }
        else
        {
          // if start of new week make a tick
          if (date > oldWeek)
          {
            oldWeek = date;
            oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

            // draw the short week tick
            Setting tick;
            tick.setData("TYPE", QString("1"));
            tick.setData("TEXT", date.toString("d"));
            _tickList.insert(loop, tick);
          }
        }
        break;
      }
      case BarData::WeeklyBar:
      {
        QDate date = _dateList.at(loop).date();
        if (date.month() != oldMonth.month())
        {
          oldMonth = date;
          if (date.month() == 1)
          {
            // draw the long tick
            Setting tick;
            tick.setData("TYPE", QString("0"));
            tick.setData("TEXT", date.toString("yy"));
            _tickList.insert(loop, tick);
          }
          else
          {
            // draw the short tick
            Setting tick;
            tick.setData("TYPE", QString("1"));
            QString s = date.toString("MMM");
            s.chop(2);
            tick.setData("TEXT", s);
            _tickList.insert(loop, tick);
          }
        }
        break;
      }
      case BarData::MonthlyBar:
      {
        QDate date = _dateList.at(loop).date();
        if (date.year() != oldYear.year())
        {
          oldYear = date;

          // draw the long tick
          Setting tick;
          tick.setData("TYPE", QString("1"));
          tick.setData("TEXT", date.toString("yy"));
          _tickList.insert(loop, tick);
        }
        break;
      }
      default:
        break;
    }
  }
}
*/
