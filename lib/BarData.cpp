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
#include <qobject.h>

BarData::BarData ()
{
  high = -99999999;
  low = 99999999;
  dateList.setAutoDelete(TRUE);
  barList.setAutoDelete(TRUE);
  barType = Daily;
}

BarData::~BarData ()
{
}

QStringList BarData::getInputFields ()
{
  QStringList l;
  l.append(QObject::tr("Open"));
  l.append(QObject::tr("High"));
  l.append(QObject::tr("Low"));
  l.append(QObject::tr("Close"));
  l.append(QObject::tr("Volume"));
  l.append(QObject::tr("Open Interest"));
  return l;
}

PlotLine * BarData::getInput (BarData::InputType field)
{
  PlotLine *in = new PlotLine();
  int loop;
  for (loop = 0; loop < (int) barList.count(); loop++)
  {
    switch(field)
    {
      case Open:
        in->append(getOpen(loop));
	break;
      case High:
        in->append(getHigh(loop));
	break;
      case Low:
        in->append(getLow(loop));
	break;
      case Volume:
        in->append(getVolume(loop));
	break;
      case OpenInterest:
        in->append(getOI(loop));
	break;
      default:
        in->append(getClose(loop));
        break;
    }
  }

  return in;
}

int BarData::count ()
{
  return (int) barList.count();
}

void BarData::prepend (Bar *bar)
{
  if (bar->getHigh() > high)
    high = bar->getHigh();
    
  if (bar->getLow() < low)
    low = bar->getLow();
  
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
    
    if (barType == Daily)
      dateList.replace(bar->getDate().getDateString(FALSE), x);
    else
      dateList.replace(bar->getDate().getDateTimeString(FALSE), x);
  }
}

BarDate BarData::getDate (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->getDate();
  else
  {
    BarDate dt;
    return dt;
  }
}

int BarData::getX (BarDate date)
{
  X *x = 0;
  
  if (barType == Daily)
    x = dateList[date.getDateString(FALSE)];
  else
    x = dateList[date.getDateTimeString(FALSE)];
  
  if (x)
    return x->x;
  else
    return -1;

/*    
  if (barCompression == Weekly)
  {
    QDateTime dt = date;
    dt = dt.addDays(- (dt.date().dayOfWeek() - 1));
  
    int loop = 0;
    for (loop = 0; loop < 6; loop++)
    {
      X *x = dateList[dt.toString("yyyyMMdd")];
      if (x)
       return x->x;
      else
        dt = dt.addDays(1);
    }
    
    return -1;
  }

  if (barCompression == Monthly)
  {
    QDateTime dt = date;
    dt.date().setYMD(date.date().year(), 1, 1);
  
    int loop = 0;
    for (loop = 0; loop < dt.date().daysInMonth(); loop++)
    {
      X *x = dateList[dt.toString("yyyyMMdd")];
      if (x)
       return x->x;
      else
        dt = dt.addDays(1);
    }
    
    return -1;
  }
  
  return -1;
*/  
}

double BarData::getOpen (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->getOpen();
  else
    return 0;
}

double BarData::getHigh (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->getHigh();
  else
    return 0;
}

double BarData::getLow (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->getLow();
  else
    return 0;
}

double BarData::getClose (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->getClose();
  else
    return 0;
}

double BarData::getVolume (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->getVolume();
  else
    return 0;
}

double BarData::getOI (int i)
{
  Bar *bar = barList.at(i);
  if (bar)
    return bar->getOI();
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

void BarData::setBarType (BarData::BarType d)
{
  barType = d;
}

BarData::BarType BarData::getBarType ()
{
  return barType;
}

BarData::InputType BarData::getInputType (QString d)
{
  InputType t = Close;
  
  while (1)
  {
    if (! d.compare(QObject::tr("Open")))
    {
      t = Open;
      break;
    }
    
    if (! d.compare(QObject::tr("High")))
    {
      t = High;
      break;
    }

    if (! d.compare(QObject::tr("Low")))
    {
      t = Low;
      break;
    }

    if (! d.compare(QObject::tr("Close")))
    {
      t = Close;
      break;
    }

    if (! d.compare(QObject::tr("Volume")))
    {
      t = Volume;
      break;
    }

    if (! d.compare(QObject::tr("Open Interest")))
      t = OpenInterest;
    
    break;
  }
  
  return t;
}

QStringList BarData::getBarCompressionList ()
{
  QStringList l;
  l.append(QObject::tr("5 Minute"));
  l.append(QObject::tr("15 Minute"));
  l.append(QObject::tr("30 Minute"));
  l.append(QObject::tr("60 Minute"));
  l.append(QObject::tr("Daily"));
  l.append(QObject::tr("Weekly"));
  l.append(QObject::tr("Monthly"));
  return l;  
}

Bar * BarData::getBar (int d)
{
  return barList.at(d);
}

void BarData::copy (BarData *d)
{
  int loop;
  for (loop = barList.count() - 1; loop > -1; loop--)
  {
    Bar *bar = new Bar;
    Bar *tbar = getBar(loop);
    tbar->copy(bar);
    d->prepend(bar);
  }
  
  d->createDateList();
  d->setBarType(getBarType());
}

void BarData::setMinMax ()
{
  int loop;
  for (loop = 0; loop < (int) barList.count(); loop++)
  {
    Bar *bar = barList.at(loop);
    if (bar->getMax() > high)
      high = bar->getMax();
    if (bar->getMin() < low)
      low = bar->getMin();
  }
}

void BarData::deleteBar (int i)
{
  barList.remove(i);
}

