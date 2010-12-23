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

BarData::~BarData ()
{
  if (_barList.count())
    qDeleteAll(_barList);
}

void BarData::clear ()
{
  if (_barList.count())
    qDeleteAll(_barList);
  
  _high = -99999999;
  _low = 99999999;
  _length = DailyBar;
  _maxBars = 0;
  _range = 6;
}

void BarData::inputFields (QStringList &l)
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

Curve * BarData::input (BarData::InputType field)
{
  Curve *line = new Curve;

  int loop;
  for (loop = 0; loop < count(); loop++)
  {
    Bar *b = bar(loop);
    
    switch (field)
    {
      case Open:
      {
        line->setBar(loop, new CurveBar(b->open()));
        break;
      }
      case High:
      {
        line->setBar(loop, new CurveBar(b->high()));
        break;
      }
      case Low:
      {
        line->setBar(loop, new CurveBar(b->low()));
        break;
      }
      case Close:
      {
        line->setBar(loop, new CurveBar(b->close()));
        break;
      }
      case Volume:
      {
        line->setBar(loop, new CurveBar(b->volume()));
        break;
      }
      case OI:
      {
        line->setBar(loop, new CurveBar(b->oi()));
        break;
      }
      case AveragePrice:
      {
        line->setBar(loop, new CurveBar(avgPrice(loop)));
        break;
      }
      case MedianPrice:
      {
        line->setBar(loop, new CurveBar(medianPrice(loop)));
        break;
      }
      case TypicalPrice:
      {
        line->setBar(loop, new CurveBar(typicalPrice(loop)));
        break;
      }
      case WeightedClosePrice:
      {
        double t = (b->high() + b->low() + (b->close() * 2)) / 4.0;
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
  return _barList.count();
}

void BarData::prepend (Bar *bar)
{
  _barList.prepend(bar);
}

void BarData::append (Bar *bar)
{
  _barList.append(bar);
}

double BarData::max ()
{
  return _high;
}

double BarData::min ()
{
  return _low;
}

BarData::InputType BarData::inputType (QString d)
{
  QStringList l;
  inputFields(l);
  
  InputType rc = Close;
  int t = l.indexOf(d);
  if (t != -1)
    rc = (InputType) t;
  return rc;
}

Bar * BarData::bar (int d)
{
  return _barList.at(d);
}

void BarData::setMinMax ()
{
  int loop;
  for (loop = 0; loop < (int) _barList.count(); loop++)
  {
    Bar *bar = _barList.at(loop);

    if (bar->high() > _high)
      _high = bar->high();

    if (bar->low() < _low)
      _low = bar->low();
  }
}

void BarData::setBarLength (BarLength d)
{
  _length = d;
}

void BarData::setBarLength (QString d)
{
  QStringList l;
  barLengthList(l);
  _length = (BarLength) l.indexOf(d);
}

BarData::BarLength & BarData::barLength ()
{
  return _length;
}

QString & BarData::symbol ()
{
  return _symbol;
}

void BarData::setSymbol (QString d)
{
  _symbol = d;
}

QString & BarData::name ()
{
  return _name;
}

void BarData::setName (QString d)
{
  _name = d;
}

double BarData::avgPrice (int d)
{
  Bar *b = bar(d);
  double t = (b->open() + b->high() + b->low() + b->close()) / 4.0;
  return t;
}

double BarData::medianPrice (int d)
{
  Bar *b = bar(d);
  double t = (b->high() + b->low()) / 2.0;
  return t;
}

double BarData::typicalPrice (int d)
{
  Bar *b = bar(d);
  double t = (b->high() + b->low() + b->close()) / 3.0;
  return t;
}

QString & BarData::exchange ()
{
  return _exchange;
}

void BarData::setExchange (QString d)
{
  _exchange = d;
}

void BarData::barLengthList (QStringList &l)
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

int BarData::setKey (QString d)
{
  QStringList l = d.split(":");
  if (l.count() != 2)
    return 1;
  
  _exchange  = l.at(0);
  _symbol = l.at(1);
  
  return 0;
}

QString BarData::key ()
{
  QString s = _exchange + ":" + _symbol;
  return s;
}

void BarData::parse (QString &d)
{
  QStringList l = d.split(";", QString::SkipEmptyParts);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList l2 = l.at(loop).split(",");
    if (l2.count() != 8)
      continue;
    
    int pos = 2;
    Bar *bar = new Bar;
    bar->setDates(l2.at(0), l2.at(1));
    bar->setOpen(l2.at(pos++));
    bar->setHigh(l2.at(pos++));
    bar->setLow(l2.at(pos++));
    bar->setClose(l2.at(pos++));
    bar->setVolume(l2.at(pos++));
    bar->setOI(l2.at(pos++));

    append(bar);
  }
}

void BarData::stringSettings (QString &d)
{
  QStringList l;
  l << _exchange;
  l << _symbol;
  l << QString::number(_length);
  l << QString::number (_range);
  d = l.join(",");
}

int BarData::setStringSettings (QString &d)
{
  QStringList l = d.split(",", QString::SkipEmptyParts);
  if (l.count() != 4)
    return 1;

  int pos = 0;
  _exchange = l.at(pos++);
  _symbol = l.at(pos++);
  _length = (BarData::BarLength) l.at(pos++).toInt();
  _range = l.at(pos++).toInt();

  return 0;
}

int BarData::maxBars ()
{
  return _maxBars;
}

void BarData::setMaxBars (int d)
{
  _maxBars = d;
}

QDateTime BarData::startDate ()
{
  return _startDate;
}

void BarData::setStartDate (QDateTime d)
{
  _startDate = d;
}

QString BarData::startDateString ()
{
  return _startDate.toString("yyyyMMddHHmmss");
}

QDateTime BarData::endDate ()
{
  return _endDate;
}

void BarData::setEndDate (QDateTime d)
{
  _endDate = d;
}

QString BarData::endDateString ()
{
  return _endDate.toString("yyyyMMddHHmmss");
}

QString BarData::table ()
{
  return _table;
}

void BarData::setTable (QString d)
{
  _table = d;
}

QString BarData::type ()
{
  return _type;
}

void BarData::setType (QString d)
{
  _type = d;
}

int BarData::range ()
{
  return _range;
}

void BarData::setRange (int d)
{
  _range = d;
}
