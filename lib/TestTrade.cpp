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
  indexStart = 0;
  indexEnd = 0;
  signal = 0;
  trailing = 0;
}

void TestTrade::getEnterDate (QDateTime &d)
{
  d = enterDate;
}

void TestTrade::setEnterDate (QDateTime &d, int index)
{
  enterDate = d;
  indexStart = index;
}

void TestTrade::getExitDate (QDateTime &d)
{
  d = exitDate;
}

void TestTrade::setExitDate (QDateTime &d, int index)
{
  exitDate = d;
  indexEnd = index;
}

double TestTrade::getEnterPrice ()
{
  return enterPrice;
}

void TestTrade::setEnterPrice (double d)
{
  enterPrice = d;
}

double TestTrade::getExitPrice ()
{
  return exitPrice;
}

void TestTrade::setExitPrice (double d)
{
  exitPrice = d;
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

  QDateTime dt;
  getEnterDate(dt);
  l.append(dt.toString(Qt::ISODate));

  if (getType() == 0)
    l.append(QObject::tr("Enter Long"));
  else
    l.append(QObject::tr("Enter Short"));

  l.append(QString::number(getVolume()));
  l.append(QString::number(getEnterPrice()));

  getExitDate(dt);
  l.append(dt.toString(Qt::ISODate));

  switch ((Signal) signal)
  {
    case SignalTestEnd:
      l.append(QObject::tr("Test End"));
      break;
    case SignalTrailingStop:
      l.append(QObject::tr("Trailing Stop"));
      break;
    default:
      if (getType() == 0)
        l.append(QObject::tr("Exit Long"));
      else
        l.append(QObject::tr("Exit Short"));
      break;
  }

  l.append(QString::number(getExitPrice()));
  l.append(QString::number(getProfit()));
}

void TestTrade::update (PlotLine *line, BarData *data, double account)
{
  if (volumePercentage > 0 && account > 0)
  {
    double money = account * (volumePercentage / 100);
    volume = money / enterPrice;
  }
  else
    volume = 1;

  // check if we have an exit price
  if (indexEnd == 0)
  {
    // no exit signal provided, we exit on last data bar
    indexEnd = line->getSize() - 1;
    exitPrice = line->getData(indexEnd);
    data->getDate(indexEnd, exitDate);
    signal = (int) SignalTestEnd;
  }
  else
    signal = (int) SignalExitLong;


  double trail = 0;
  if (trailing)
    trail = calculateTrailingStop(enterPrice);

  int loop;
  for (loop = indexStart; loop <= indexEnd; loop++)
  {
    double ev = enterPrice * volume;
    value = line->getData(loop) * volume;

    if (type == 0)
      profit = value - ev;
    else
      profit = ev - value;

    if (profit < drawDown)
      drawDown = profit;

    // check trailing stop if set
    if (trailing)
    {
      double td = calculateTrailingStop(line->getData(loop));

      // long
      if (type == 0)
      {
        if (line->getData(loop) < trail)
        {
          signal = (int) SignalTrailingStop;
	  exitPrice = line->getData(loop);
	  data->getDate(loop, exitDate);
	  return;
        }

        if (td > trail)
	  trail = td;
      }
      else
      {
	// short
        if (line->getData(loop) > trail)
        {
          signal = (int) SignalTrailingStop;
	  exitPrice = line->getData(loop);
	  data->getDate(loop, exitDate);
	  return;
        }

        if (td < trail)
	  trail = td;
      }
    }
  }
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
  double ts = 0;
  if (type == 0)
    ts = price * (1 - (trailing / 100));
  else
    ts = price * (1 + (trailing / 100));

  return ts;
}


