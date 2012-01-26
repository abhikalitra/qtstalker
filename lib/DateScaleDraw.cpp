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
#include "GlobalSymbol.h"
#include "DateRange.h"
#include "KeyCurveBar.h"
#include "KeySymbol.h"
#include "TypeBarLength.h"

#include <QString>
#include <QDebug>

DateScaleDraw::DateScaleDraw ()
{
  _barLength = -1;
}

void DateScaleDraw::clear ()
{
  _dateList.clear();
  _data.clear();
  _dates.clear();
}

void DateScaleDraw::setDates (Entity &d)
{
  QList<int> keys;
  d.ekeys(keys);
  
  KeyCurveBar cbkeys;
  KeySymbol symkeys;
  Data length;
  g_currentSymbol.toData(symkeys.indexToString(KeySymbol::_LENGTH), length);

  DateRange dr;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity cb;
    d.toIndex(keys.at(loop), cb);
    
    Data td;
    cb.toData(cbkeys.indexToString(KeyCurveBar::_DATE), td);

    QDateTime tsd, ted;
    dr.dateInterval(td.toDateTime(), (TypeBarLength::Key) length.toInteger(), tsd, ted);
    QString s = dr.rangeKey(tsd, ted);

    _data.insert(s, keys.at(loop));

    _dates.insert(keys.at(loop), td.toDateTime());
  }
}

void DateScaleDraw::setDates ()
{
  QList<int> keys = _dates.keys();
  qSort(keys);

  _dateList.clear();
  int loop = 0;
  for (; loop < keys.size(); loop++)
    _dateList << _dates.value(keys.at(loop));

  KeySymbol symkeys;
  Data length;
  g_currentSymbol.toData(symkeys.indexToString(KeySymbol::_LENGTH), length);
  _barLength = length.toInteger();
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

  switch ((TypeBarLength::Key) _barLength)
  {
    case TypeBarLength::_MINUTE1:
    case TypeBarLength::_MINUTE5:
    case TypeBarLength::_MINUTE10:
    case TypeBarLength::_MINUTE15:
    case TypeBarLength::_MINUTE30:
    case TypeBarLength::_MINUTE60:
      date = _dateList.at(t).toString("d h:m");
      break;
    case TypeBarLength::_DAILY:
      if (_dateList.at(t).date().month() == 1)
        date = _dateList.at(t).toString("yyyy");
      else
        date = _dateList.at(t).toString("MMM");
      break;
    case TypeBarLength::_WEEKLY:
      if (_dateList.at(t).date().month() == 1)
        date = _dateList.at(t).toString("yyyy");
      else
      {
        QString s = _dateList.at(t).toString("MMM");
        s.chop(2);
        date = s;
      }
      break;
    case TypeBarLength::_MONTHLY:
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
  DateRange dr;
  QDateTime tsd, ted;
  dr.dateInterval(d, (TypeBarLength::Key) _barLength, tsd, ted);

  QString s = dr.rangeKey(tsd, ted);
  if (_data.contains(s))
    x = _data.value(s);

  return x;
}

void DateScaleDraw::info (int index, Entity &set)
{
  QDateTime dt;
  date(index, dt);
  if (dt.isValid())
  {
    set.set(QString("D"), dt.toString("yyyy-MM-dd"));
    set.set(QString("T"), dt.toString("HH:mm:ss"));
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
  if ((TypeBarLength::Key) _barLength != TypeBarLength::_MINUTE1)
    nextHour = nextHour.addSecs(7200);
  else
    nextHour = nextHour.addSecs(3600);

  for (; loop < size; loop++)
  {
    _dateString.clear();

    switch ((TypeBarLength::Key) _barLength)
    {
      case TypeBarLength::_MINUTE1:
      case TypeBarLength::_MINUTE5:
      case TypeBarLength::_MINUTE10:
      case TypeBarLength::_MINUTE15:
      case TypeBarLength::_MINUTE30:
      case TypeBarLength::_MINUTE60:
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
            if ((TypeBarLength::Key) _barLength < TypeBarLength::_MINUTE30)
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
          if ((TypeBarLength::Key) _barLength != TypeBarLength::_MINUTE1)
            nextHour = nextHour.addSecs(7200);
          else
            nextHour = nextHour.addSecs(3600);
        }
        break;
      }
      case TypeBarLength::_DAILY:
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
      case TypeBarLength::_WEEKLY:
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
      case TypeBarLength::_MONTHLY:
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
