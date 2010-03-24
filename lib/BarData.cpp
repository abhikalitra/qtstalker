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
  length = Bar::DailyBar;
  barsRequested = 0;
}

BarData::~BarData ()
{
  qDeleteAll(barList);
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
  PlotLine *in = new PlotLine;
  int loop;
  QColor color("red");
  for (loop = 0; loop < count(); loop++)
  {
    Bar *bar = getBar(loop);
    
    switch (field)
    {
      case Open:
      {
        in->append(color, bar->getOpen());
        break;
      }
      case High:
      {
        in->append(color, bar->getHigh());
        break;
      }
      case Low:
      {
        in->append(color, bar->getLow());
        break;
      }
      case Volume:
      {
        in->append(color, bar->getVolume());
        break;
      }
      case OI:
      {
        in->append(color, bar->getOI());
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
        double t = (bar->getHigh() + bar->getLow() + (bar->getClose() * 2)) / 4.0;
        in->append(color, t);
        break;
      }
      default:
      {
        in->append(color, bar->getClose());
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
  QStringList l;
  getInputFields(l);
  
  InputType rc = Close;
  int t = l.indexOf(d);
  if (t != -1)
    rc = (InputType) t;
  return rc;
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

void BarData::setBarLength (Bar::BarLength d)
{
  length = d;
}

void BarData::setBarLength (QString &d)
{
  QStringList l;
  Bar tbar;
  tbar.getBarLengthList(l);
  length = (Bar::BarLength) l.indexOf(d);
}

Bar::BarLength BarData::getBarLength ()
{
  return length;
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

double BarData::getAvgPrice (int d)
{
  Bar *bar = getBar(d);
  double t = (bar->getOpen() + bar->getHigh() + bar->getLow() + bar->getClose()) / 4.0;
  return t;
}

double BarData::getMedianPrice (int d)
{
  Bar *bar = getBar(d);
  double t = (bar->getHigh() + bar->getLow()) / 2.0;
  return t;
}

double BarData::getTypicalPrice (int d)
{
  Bar *bar = getBar(d);
  double t = (bar->getHigh() + bar->getLow() + bar->getClose()) / 3.0;
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

