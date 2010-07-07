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

#include "BarData.h"
#include "PlotFactory.h"

#include <QObject>
#include <QtDebug>

BarData::BarData ()
{
  _high = -99999999;
  _low = 99999999;
  _length = Bar::DailyBar;
  _dateRange = 5;
  _dateRangeOverride = 0;
  _startDate = QDateTime::currentDateTime();
  _endDate = _startDate;
}

void BarData::getInputFields (QStringList &l)
{
  l.clear();
  l << QObject::tr("Open");
  l << QObject::tr("High");
  l << QObject::tr("Low");
  l << QObject::tr("Close");
  l << QObject::tr("Volume");
  l << QObject::tr("OI");
  l << QObject::tr("AvgPrice");
  l << QObject::tr("MedianPrice");
  l << QObject::tr("TypicalPrice");
  l << QObject::tr("WCPrice");
}

PlotLine * BarData::getInput (BarData::InputType field)
{
  QColor color("red");
  return getInput(field, (int) PlotFactory::PlotTypeLine, color);
}

PlotLine * BarData::getInput (BarData::InputType field, int lineType, QColor &color)
{
  PlotFactory fac;
  PlotLine *in = fac.plot(lineType);
  if (! in)
    return 0;

  int loop;
  for (loop = 0; loop < count(); loop++)
  {
    Bar bar = getBar(loop);
    
    switch (field)
    {
      case Open:
      {
        PlotLineBar b(color, bar.getOpen());
        in->setData(loop, b);
        break;
      }
      case High:
      {
        PlotLineBar b(color, bar.getHigh());
        in->setData(loop, b);
        break;
      }
      case Low:
      {
        PlotLineBar b(color, bar.getLow());
        in->setData(loop, b);
        break;
      }
      case Close:
      {
        PlotLineBar b(color, bar.getClose());
        in->setData(loop, b);
        break;
      }
      case Volume:
      {
        PlotLineBar b(color, bar.getVolume());
        in->setData(loop, b);
        break;
      }
      case OI:
      {
        PlotLineBar b(color, bar.getOI());
        in->setData(loop, b);
        break;
      }
      case AveragePrice:
      {
        PlotLineBar b(color, getAvgPrice(loop));
        in->setData(loop, b);
        break;
      }
      case MedianPrice:
      {
        PlotLineBar b(color, getMedianPrice(loop));
        in->setData(loop, b);
        break;
      }
      case TypicalPrice:
      {
        PlotLineBar b(color, getTypicalPrice(loop));
        in->setData(loop, b);
        break;
      }
      case WeightedClosePrice:
      {
        double t = (bar.getHigh() + bar.getLow() + (bar.getClose() * 2)) / 4.0;
        PlotLineBar b(color, t);
        in->setData(loop, b);
        break;
      }
      default:
        break;
    }
  }

  return in;
}

int BarData::count ()
{
  return (int) _barList.count();
}

void BarData::prepend (Bar bar)
{
  _barList.prepend(bar);
}

void BarData::append (Bar bar)
{
  _barList.append(bar);
}

double BarData::getMax ()
{
  return _high;
}

double BarData::getMin ()
{
  return _low;
}

BarData::InputType BarData::getInputType (QString &d)
{
  QStringList l;
  getInputFields(l);
  
  InputType rc = Close;
  int t = l.indexOf(d);
  if (t != -1)
    rc = (InputType) t;
  return rc;
}

Bar & BarData::getBar (int d)
{
  return _barList[d];
}

void BarData::setMinMax ()
{
  int loop;
  for (loop = 0; loop < (int) _barList.count(); loop++)
  {
    Bar bar = _barList.at(loop);

    if (bar.getHigh() > _high)
      _high = bar.getHigh();

    if (bar.getLow() < _low)
      _low = bar.getLow();
  }
}

void BarData::setBarLength (Bar::BarLength d)
{
  _length = d;
}

void BarData::setBarLength (QString &d)
{
  QStringList l;
  Bar tbar;
  tbar.getBarLengthList(l);
  _length = (Bar::BarLength) l.indexOf(d);
}

Bar::BarLength BarData::getBarLength ()
{
  return _length;
}

QString & BarData::getSymbol ()
{
  return _symbol;
}

void BarData::setSymbol (QString &d)
{
  _symbol = d;
}

QString & BarData::getName ()
{
  return _name;
}

void BarData::setName (QString &d)
{
  _name = d;
}

double BarData::getAvgPrice (int d)
{
  Bar bar = getBar(d);
  double t = (bar.getOpen() + bar.getHigh() + bar.getLow() + bar.getClose()) / 4.0;
  return t;
}

double BarData::getMedianPrice (int d)
{
  Bar bar = getBar(d);
  double t = (bar.getHigh() + bar.getLow()) / 2.0;
  return t;
}

double BarData::getTypicalPrice (int d)
{
  Bar bar = getBar(d);
  double t = (bar.getHigh() + bar.getLow() + bar.getClose()) / 3.0;
  return t;
}

QString & BarData::getTableName ()
{
  return _tableName;
}

void BarData::setTableName (QString &d)
{
  _tableName = d;
}

QString & BarData::getPlugin ()
{
  return _plugin;
}

void BarData::setPlugin (QString &d)
{
  _plugin = d;
}

QString & BarData::getExchange ()
{
  return _exchange;
}

void BarData::setExchange (QString &d)
{
  _exchange = d;
}

void BarData::setDateRange (int d)
{
  _dateRange = d;
}

int BarData::dateRange ()
{
  return _dateRange;
}

void BarData::setDateRangeOverride (int d)
{
  _dateRangeOverride = d;
}

int BarData::dateRangeOverride ()
{
  return _dateRangeOverride;
}

void BarData::setStartDate (QDateTime &d)
{
  _startDate = d;
}

QDateTime & BarData::startDate ()
{
  return _startDate;
}

void BarData::setEndDate (QDateTime &d)
{
  _endDate = d;
}

QDateTime & BarData::endDate ()
{
  return _endDate;
}

