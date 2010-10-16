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
#include "BarRange.h"
#include "Globals.h"

#include <QString>
#include <QDebug>

DateScaleDraw::DateScaleDraw ()
{
}

void DateScaleDraw::setDates ()
{
  _dateList.clear();
  _data.clear();

  _barLength = g_barData.getBarLength();

  int loop;
  for (loop = 0; loop < (int) g_barData.count(); loop++)
  {
    Bar bar = g_barData.getBar(loop);
    if (! bar.count())
      continue;

    QString s;
    bar.getRangeKey(s);
    _data.insert(s, loop);

    _dateList.append(bar.date());
  }
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
  
  switch ((BarData::BarLength) _barLength)
  {
    case BarData::Minute1:
    case BarData::Minute5:
    case BarData::Minute10:
    case BarData::Minute15:
    case BarData::Minute30:
    case BarData::Minute60:
      date = _dateList.at(t).toString("dd HH:mm");
      break;
    case BarData::DailyBar:
    case BarData::WeeklyBar:
      date = _dateList.at(t).toString("yy-MMM-dd");
      break;
    case BarData::MonthlyBar:
      date = _dateList.at(t).toString("yyyy-MMM");
      break;
    default:
      break;
  }
  
//  return _dateList.at(t).toString("yy-MMM-dd");
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
  BarRange br;
  br.setBarRange(d, (BarData::BarLength) _barLength);

  QString s;
  br.key(s);
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

