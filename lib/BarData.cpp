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
  high = -99999999;
  low = 99999999;
  barLength = DailyBar;
  barsRequested = 0;

  inputList << QObject::tr("Open");
  inputList << QObject::tr("High");
  inputList << QObject::tr("Low");
  inputList << QObject::tr("Close");
  inputList << QObject::tr("Volume");
  inputList << QObject::tr("OI");
  inputList << QObject::tr("AvgPrice");
  inputList << QObject::tr("MedianPrice");
  inputList << QObject::tr("TypicalPrice");
  inputList << QObject::tr("WCPrice");
  
  exchangeList << "NYSE";
  exchangeList << "";
}

BarData::~BarData ()
{
  qDeleteAll(barList);
}

void BarData::getInputFields (QStringList &l)
{
  l = inputList;
}

PlotLine * BarData::getInput (BarData::InputType field)
{
  PlotLine *in = new PlotLine;
  int loop;
  QColor color("red");
  for (loop = 0; loop < count(); loop++)
  {
    switch (field)
    {
      case Open:
      {
        in->append(color, getOpen(loop));
        break;
      }
      case High:
      {
        in->append(color, getHigh(loop));
        break;
      }
      case Low:
      {
        in->append(color, getLow(loop));
        break;
      }
      case Volume:
      {
        in->append(color, getVolume(loop));
        break;
      }
      case OI:
      {
        in->append(color, getOI(loop));
        break;
      }
      case AveragePrice:
      {
        double t = getAvgPrice(loop);
        in->append(color, t);
        break;
      }
      case MedianPrice:
      {
        double t = getMedianPrice(loop);
        in->append(color, t);
        break;
      }
      case TypicalPrice:
      {
        double t = getTypicalPrice(loop);
        in->append(color, t);
        break;
      }
      case WeightedClosePrice:
      {
        double t = (getHigh(loop) + getLow(loop) + (getClose(loop) * 2)) / 4.0;
        in->append(color, t);
        break;
      }
      default:
      {
        in->append(color, getClose(loop));
        break;
      }
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
  barList.prepend(bar);
}

void BarData::append (Bar *bar)
{
  barList.append(bar);
}

void BarData::getDate (int i, QDateTime &dt)
{
  dt = barList.at(i)->getDate();
}

double BarData::getOpen (int i)
{
  return barList.at(i)->getOpen();
}

double BarData::getHigh (int i)
{
  return barList.at(i)->getHigh();
}

double BarData::getLow (int i)
{
  return barList.at(i)->getLow();
}

double BarData::getClose (int i)
{
  return barList.at(i)->getClose();
}

double BarData::getVolume (int i)
{
  return barList.at(i)->getVolume();
}

double BarData::getOI (int i)
{
  return barList.at(i)->getOI();
}

double BarData::getMax ()
{
  return high;
}

double BarData::getMin ()
{
  return low;
}

BarData::InputType BarData::getInputType (QString &d)
{
  InputType rc = Close;
  int t = inputList.indexOf(d);
  if (t != -1)
    rc = (InputType) t;
  return rc;
}

void BarData::getBarLengthList (QStringList &l)
{
  l.clear();
  l.append(QObject::tr("1 Minute"));
  l.append(QObject::tr("5 Minute"));
  l.append(QObject::tr("10 Minute"));
  l.append(QObject::tr("15 Minute"));
  l.append(QObject::tr("30 Minute"));
  l.append(QObject::tr("60 Minute"));
  l.append(QObject::tr("Daily"));
  l.append(QObject::tr("Weekly"));
  l.append(QObject::tr("Monthly"));
}

Bar * BarData::getBar (int d)
{
  return barList.at(d);
}

void BarData::setMinMax ()
{
  int loop;
  for (loop = 0; loop < (int) barList.count(); loop++)
  {
    Bar *bar = barList.at(loop);

    if (bar->getHigh() > high)
      high = bar->getHigh();

    if (bar->getLow() < low)
      low = bar->getLow();
  }
}

void BarData::setBarLength (BarData::BarLength d)
{
  barLength = d;
}

void BarData::setBarLength (QString &d)
{
  QStringList l;
  getBarLengthList(l);
  barLength = (BarLength) l.indexOf(d);
}

BarData::BarLength BarData::getBarLength ()
{
  return barLength;
}

QString & BarData::getSymbol ()
{
  return symbol;
}

void BarData::setSymbol (QString &d)
{
  symbol = d;
}

QString & BarData::getName ()
{
  return name;
}

void BarData::setName (QString &d)
{
  name = d;
}

int BarData::getBarsRequested ()
{
  return barsRequested;
}

void BarData::setBarsRequested (int d)
{
  barsRequested = d;
}

void BarData::getDateString (int d, QString &s)
{
  barList.at(d)->getDateString(s);
}

void BarData::getTimeString (int d, QString &s)
{
  barList.at(d)->getTimeString(s);
}

void BarData::getDateTimeString (int d, QString &s)
{
  barList.at(d)->getDateTimeString(s);
}

double BarData::getAvgPrice (int d)
{
  double t = (getOpen(d) + getHigh(d) + getLow(d) + getClose(d)) / 4.0;
  return t;
}

double BarData::getMedianPrice (int d)
{
  double t = (getHigh(d) + getLow(d)) / 2.0;
  return t;
}

double BarData::getTypicalPrice (int d)
{
  double t = (getHigh(d) + getLow(d) + getClose(d)) / 3.0;
  return t;
}

QString & BarData::getTableName ()
{
  return tableName;
}

void BarData::setTableName (QString &d)
{
  tableName = d;
}

QString & BarData::getPlugin ()
{
  return plugin;
}

void BarData::setPlugin (QString &d)
{
  plugin = d;
}

QString & BarData::getExchange ()
{
  return exchange;
}

void BarData::setExchange (QString &d)
{
  exchange = d;
}

QString & BarData::getCurrency ()
{
  return currency;
}

void BarData::setCurrency (QString &d)
{
  currency = d;
}

