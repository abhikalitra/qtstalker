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

#include <QObject>
#include <QtDebug>

BarData::BarData ()
{
  clear();
}

void BarData::clear ()
{
  _high = -99999999;
  _low = 99999999;
  _length = DailyBar;
  _barList.clear();
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

Curve * BarData::getInput (BarData::InputType field)
{
  Curve *line = new Curve;

  int loop;
  for (loop = 0; loop < count(); loop++)
  {
    Bar bar = getBar(loop);
    
    switch (field)
    {
      case Open:
      {
        line->setBar(loop, new CurveBar(bar.getData(Bar::BarFieldOpen)));
        break;
      }
      case High:
      {
        line->setBar(loop, new CurveBar(bar.getData(Bar::BarFieldHigh)));
        break;
      }
      case Low:
      {
        line->setBar(loop, new CurveBar(bar.getData(Bar::BarFieldLow)));
        break;
      }
      case Close:
      {
        line->setBar(loop, new CurveBar(bar.getData(Bar::BarFieldClose)));
        break;
      }
      case Volume:
      {
        line->setBar(loop, new CurveBar(bar.getData(Bar::BarFieldVolume)));
        break;
      }
      case OI:
      {
        line->setBar(loop, new CurveBar(bar.getData(Bar::BarFieldOI)));
        break;
      }
      case AveragePrice:
      {
        line->setBar(loop, new CurveBar(getAvgPrice(loop)));
        break;
      }
      case MedianPrice:
      {
        line->setBar(loop, new CurveBar(getMedianPrice(loop)));
        break;
      }
      case TypicalPrice:
      {
        line->setBar(loop, new CurveBar(getTypicalPrice(loop)));
        break;
      }
      case WeightedClosePrice:
      {
        double t = (bar.getData(Bar::BarFieldHigh) + bar.getData(Bar::BarFieldLow) + (bar.getData(Bar::BarFieldClose) * 2)) / 4.0;
        line->setBar(loop, new CurveBar(t));
        break;
      }
      default:
        break;
    }
  }

  return line;
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

    if (bar.getData(Bar::BarFieldHigh) > _high)
      _high = bar.getData(Bar::BarFieldHigh);

    if (bar.getData(Bar::BarFieldLow) < _low)
      _low = bar.getData(Bar::BarFieldLow);
  }
}

void BarData::setBarLength (BarLength d)
{
  _length = d;
}

void BarData::setBarLength (QString &d)
{
  QStringList l;
  getBarLengthList(l);
  _length = (BarLength) l.indexOf(d);
}

BarData::BarLength & BarData::getBarLength ()
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

QString & BarData::getTable ()
{
  return _table;
}

void BarData::setTable (QString &d)
{
  _table = d;
}

double BarData::getAvgPrice (int d)
{
  Bar bar = getBar(d);
  double t = (bar.getData(Bar::BarFieldOpen) + bar.getData(Bar::BarFieldHigh) + bar.getData(Bar::BarFieldLow) + bar.getData(Bar::BarFieldClose)) / 4.0;
  return t;
}

double BarData::getMedianPrice (int d)
{
  Bar bar = getBar(d);
  double t = (bar.getData(Bar::BarFieldHigh) + bar.getData(Bar::BarFieldLow)) / 2.0;
  return t;
}

double BarData::getTypicalPrice (int d)
{
  Bar bar = getBar(d);
  double t = (bar.getData(Bar::BarFieldHigh) + bar.getData(Bar::BarFieldLow) + bar.getData(Bar::BarFieldClose)) / 3.0;
  return t;
}

QString & BarData::getExchange ()
{
  return _exchange;
}

void BarData::setExchange (QString &d)
{
  _exchange = d;
}

void BarData::getBarLengthList (QStringList &l)
{
  l.clear();
  l << QObject::tr("1 Minute");
  l << QObject::tr("5 Minute");
  l << QObject::tr("10 Minute");
  l << QObject::tr("15 Minute");
  l << QObject::tr("30 Minute");
  l << QObject::tr("60 Minute");
  l << QObject::tr("Daily");
  l << QObject::tr("Weekly");
  l << QObject::tr("Monthly");
}

void BarData::setBars (QString &d)
{
  _barList.clear();

  if (d.isEmpty())
    return;

  if (d == "ERROR")
    return;
  
  QStringList l = d.split(":");

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList l2 = l.at(loop).split(",");
    if (l2.count() < 7 && l2.count() > 8)
      continue;

    Bar bar;
    bar.setDates(l2[0], l2[1]);
    bar.setData(Bar::BarFieldOpen, l2[2]);
    bar.setData(Bar::BarFieldHigh, l2[3]);
    bar.setData(Bar::BarFieldLow, l2[4]);
    bar.setData(Bar::BarFieldClose, l2[5]);
    bar.setData(Bar::BarFieldVolume, l2[6]);

    if (l2.count() == 8)
      bar.setData(Bar::BarFieldOI, l2[7]);

    append(bar);
  }
}

void BarData::barLengthText (BarData::BarLength k, QString &d)
{
  d.clear();
  
  switch (k)
  {
    case Minute1:
      d = "1";
      break;
    case Minute5:
      d = "5";
      break;
    case Minute10:
      d = "10";
      break;
    case Minute15:
      d = "15";
      break;
    case Minute30:
      d = "30";
      break;
    case Minute60:
      d = "60";
      break;
    case DailyBar:
      d = "D";
      break;
    case WeeklyBar:
      d = "W";
      break;
    case MonthlyBar:
      d = "M";
      break;
    default:
      break;
  }
}

QString BarData::getKey ()
{
  QString s = _exchange + ":" + _symbol;
  return s;
}

