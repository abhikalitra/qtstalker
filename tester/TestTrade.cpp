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

#include "TestTrade.h"

#include <QObject>
#include <QDebug>

TestTrade::TestTrade ()
{
  enterPrice = 0;
  exitPrice = 0;
  type = 0;
  volume = 1;
  volumePercentage = 20;
  drawDown = 0;
  value = 0;
  profit = 0;
  signal = 0;
  trailing = 0;
  trail = 0;
  high = 0;
  low = 0;
}

QDateTime & TestTrade::getEnterDate ()
{
  return enterDate;
}

void TestTrade::setEnterDate (QDateTime &d)
{
  enterDate = d;
}

QDateTime & TestTrade::getExitDate ()
{
  return exitDate;
}

void TestTrade::setExitDate (QDateTime &d)
{
  exitDate = d;
}

double TestTrade::getEnterPrice ()
{
  return enterPrice;
}

void TestTrade::setEnterPrice (double d, double l)
{
  enterPrice = d;
  low = l;
  
  if (! trailing)
    return;
  
  trail = calculateTrailingStop(d);
}

double TestTrade::getExitPrice ()
{
  return exitPrice;
}

void TestTrade::setExitPrice (double d, double h)
{
  exitPrice = d;
  high = h;
}

int TestTrade::getType ()
{
  return type;
}

void TestTrade::setType (int d)
{
  type = d;
}

int TestTrade::getVolume ()
{
  return volume;
}

void TestTrade::setVolume (int d)
{
  volume = d;
}

double TestTrade::getVolumePercentage ()
{
  return volumePercentage;
}

void TestTrade::setVolumePercentage (double d)
{
  volumePercentage = d;
}

double TestTrade::getValue ()
{
  return value;
}

double TestTrade::getDrawDown ()
{
  return drawDown;
}

double TestTrade::getProfit ()
{
  return profit;
}

void TestTrade::getLogMessage (QStringList &l)
{
  l.clear();

  QDateTime dt = getEnterDate();
  l.append(dt.toString(Qt::ISODate));

  if (getType() == 0)
    l.append(QObject::tr("Enter Long"));
  else
    l.append(QObject::tr("Enter Short"));

  l.append(QString::number(getVolume()));
  l.append(QString::number(getEnterPrice()));

  dt = getExitDate();
  l.append(dt.toString(Qt::ISODate));

  switch ((Signal) signal)
  {
    case SignalTestEnd:
      l.append(QObject::tr("Test End"));
      break;
    case SignalTrailingStop:
      l.append(QObject::tr("Trailing Stop"));
      break;
    case SignalExitLong:
      l.append(QObject::tr("Exit Long"));
      break;
    case SignalExitShort:
      l.append(QObject::tr("Exit Short"));
      break;
    default:
      break;
  }

  l.append(QString::number(getExitPrice()));
  l.append(QString::number(getProfit()));
  
  l.append(QString::number(getHigh())); // hidden column
  l.append(QString::number(getLow())); // hidden column
}

int TestTrade::update ()
{
  double ev = enterPrice * volume;
  value = exitPrice * volume;

  if (type == 0)
    profit = value - ev;
  else
    profit = ev - value;

  if (profit < drawDown)
    drawDown = profit;

  // check trailing stop if set
  if (! trailing)
    return 0;
  
  // long
  if (type == 0)
  {
    if (exitPrice < trail)
    {
      signal = (int) SignalTrailingStop;
      return 1;
    }

    double td = calculateTrailingStop(exitPrice);
    if (td > trail)
      trail = td;
  }
  else
  {
    // short
    if (exitPrice > trail)
    {
      signal = (int) SignalTrailingStop;
      return 1;
    }

    double td = calculateTrailingStop(exitPrice);
    if (td < trail)
      trail = td;
  }
  
  return 0;
}

int TestTrade::getSignal ()
{
  return signal;
}

void TestTrade::setSignal (int d)
{
  signal = d;
}

void TestTrade::setTrailing (double d)
{
  trailing = d;
}

double TestTrade::calculateTrailingStop (double price)
{
  if (type == 0)
    return price * ((double) 1 - (trailing / (double) 100));
  else
    return price * ((double) 1 + (trailing / (double) 100));
}

double TestTrade::getTrail ()
{
  return trail;
}

double TestTrade::getHigh ()
{
  return high;
}

double TestTrade::getLow ()
{
  return low;
}

