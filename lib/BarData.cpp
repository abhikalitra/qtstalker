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

#include "BarData.h"

BarData::BarData ()
{
  type = Bars;
  high = -99999999;
  low = 99999999;
  dateList.setAutoDelete(TRUE);
  barList.setAutoDelete(TRUE);
}

BarData::BarData (QStringList f)
{
  type = Other;
  high = -99999999;
  low = 99999999;
  dateList.setAutoDelete(TRUE);
  barList.setAutoDelete(TRUE);
  format = f;
}

BarData::~BarData ()
{
}

int BarData::count ()
{
  return (int) barList.count();
}

void BarData::prepend (Setting *r)
{
  QDateTime dt = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug("BarData::prepend:invalid date");
    return;
  }
  
  Bar *bar = new Bar;
  bar->date = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);
  
  if (type == Bars)
  {
    bar->open = r->getFloat("Open");
    bar->high = r->getFloat("High");
    bar->low = r->getFloat("Low");
    bar->close = r->getFloat("Close");
    bar->volume = r->getFloat("Volume");
    bar->oi = r->getInt("Open Interest");
    
    if (bar->high > high)
      high = bar->high;
    if (bar->low < low)
      low = bar->low;
  }
  else
  {
    int loop = 0;
    bar->open = 0;
    bar->high = 0;
    bar->low = 0;
    bar->close = 0;
    bar->volume = 0;
    bar->oi = 0;
    int end = (int) format.count();

    while (1)
    {
      if (loop == end)
        break;
      else
        bar->open = r->getFloat(format[loop]);
  
      loop++;
      if (loop == end)
        break;
      else
        bar->high = r->getFloat(format[loop]);
  
      loop++;
      if (loop == end)
        break;
      else
        bar->low = r->getFloat(format[loop]);
      
      loop++;
      if (loop == end)
        break;
      else
        bar->close = r->getFloat(format[loop]);

      loop++;
      if (loop == end)
        break;
      else
        bar->volume = r->getFloat(format[loop]);
    }
  }
  
  barList.prepend(bar);
}

void BarData::createDateList ()
{
  dateList.clear();
  
  int loop;
  for (loop = 0; loop < (int) barList.count(); loop++)
  {
    Bar *bar = barList.at(loop);
    
    X *x = new X;
    x->x = loop;
    dateList.replace(bar->date.toString("yyyyMMdd"), x);
  }
}

QDateTime BarData::getDate (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->date;
  else
  {
    QDateTime dt;
    return dt;
  }
}

int BarData::getX (QDateTime date)
{
  X *x = dateList[date.toString("yyyyMMdd")];
  if (x)
    return x->x;
  else
    return -1;
}

double BarData::getOpen (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->open;
  else
    return 0;
}

double BarData::getHigh (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->high;
  else
    return 0;
}

double BarData::getLow (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->low;
  else
    return 0;
}

double BarData::getClose (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->close;
  else
    return 0;
}

double BarData::getVolume (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->volume;
  else
    return 0;
}

int BarData::getOI (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->oi;
  else
    return 0;
}

double BarData::getMax ()
{
  return high;
}

double BarData::getMin ()
{
  return low;
}

double BarData::getOther (int i, int f)
{
  double val = 0;
  
  Bar *bar = barList.at(i);
  if (bar)
  {
    switch (f)
    {
      case 0:
        val = bar->open;
	break;
      case 1:
        val = bar->high;
	break;
      case 2:
        val = bar->low;
	break;
      case 3:
        val = bar->close;
	break;
      default:
        val = bar->volume;
	break;
    }
  }
  
  return val;
}

BarData::BarType BarData::getType ()
{
  return type;
}

QStringList BarData::getFormat ()
{
  return format;
}


