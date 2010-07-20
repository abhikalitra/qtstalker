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

#include "DateBar.h"
#include "BarRange.h"

#include <QObject>
#include <QtDebug>

DateBar::DateBar ()
{
  barLength = (int) BarData::DailyBar;
}

void DateBar::clear ()
{
  data.clear();
  dateList.clear();
}

int DateBar::count ()
{
  return (int) data.count();
}

void DateBar::createDateList (BarData &bd)
{
  clear();
  
  barLength = bd.getBarLength();

  int loop;
  for (loop = 0; loop < (int) bd.count(); loop++)
  {
    Bar bar = bd.getBar(loop);
    if (! bar.count())
      continue;

    QString s;
    bar.getRangeKey(s);
    data.insert(s, loop);

    dateList.append(bar.date());
  }
}

void DateBar::getDate (int i, QDateTime &dt)
{
  dt = dateList.at(i);
}

int DateBar::getX (QDateTime &d)
{
  int x = -1;
  BarRange br;
  br.setBarRange(d, (BarData::BarLength) barLength);
  
  QString s;
  br.key(s);
  if (data.contains(s))
    x = data.value(s);

  return x;
}

void DateBar::setBarLength (int d)
{
  barLength = d;
}

int DateBar::getBarLength ()
{
  return barLength;
}

void DateBar::getDateString (int i, QString &d)
{
  QDateTime dt = dateList.at(i);
  d = dt.toString("yyyy-MM-dd");
}

void DateBar::getTimeString (int i, QString &d)
{
  QDateTime dt = dateList.at(i);
  d = dt.toString("HH:mm:ss");
}

void DateBar::getDateTimeString (int i, QString &d)
{
  QDateTime dt = dateList.at(i);
  d = dt.toString(Qt::ISODate);
}


