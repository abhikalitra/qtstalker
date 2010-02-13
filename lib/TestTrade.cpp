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

TestTrade::TestTrade ()
{
  enterPrice = 0;
  exitPrice = 0;
  type = 0;
  volume = 1;
  drawDown = 0;
  value = 0;
  profit = 0;
  indexStart = 0;
  indexEnd = 0;
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

void TestTrade::getEntryLogMessage (QString &d)
{
  QDateTime dt;
  getEnterDate(dt);
  d = dt.toString(Qt::ISODate);

  if (getType() == 0)
  {
    d.append(QObject::tr(" Enter Long: "));
    d.append(QString::number(getVolume()));
    d.append(QObject::tr(" at "));
    d.append(QString::number(getEnterPrice()));
  }
  else
  {
    d.append(QObject::tr(" Enter Short: "));
    d.append(QString::number(getVolume()));
    d.append(QObject::tr(" at "));
    d.append(QString::number(getEnterPrice()));
  }
}

void TestTrade::getExitLogMessage (QString &d)
{
  QDateTime dt;
  getExitDate(dt);
  d = dt.toString(Qt::ISODate);

  if (getType() == 0)
  {
    d.append(QObject::tr(" Exit Long: "));
    d.append(QString::number(getVolume()));
    d.append(QObject::tr(" at "));
    d.append(QString::number(getExitPrice()));
    if (getProfit() >= 0)
      d.append(QObject::tr(" with a profit of $") + QString::number(getProfit()));
    else
      d.append(QObject::tr(" with a loss of $") + QString::number(getProfit()));
  }
  else
  {
    d.append(QObject::tr(" Exit Short: "));
    d.append(QString::number(getVolume()));
    d.append(QObject::tr(" at "));
    d.append(QString::number(getExitPrice()));
    if (getProfit() >= 0)
      d.append(QObject::tr(" with a profit of $") + QString::number(getProfit()));
    else
      d.append(QObject::tr(" with a loss of $") + QString::number(getProfit()));
  }
}

void TestTrade::update (PlotLine *data)
{
  int loop;
  for (loop = indexStart; loop <= indexEnd; loop++)
  {
    double ev = enterPrice * volume;
    value = data->getData(loop) * volume;

    if (type == 0)
      profit = value - ev;
    else
      profit = ev - value;

    if (profit < drawDown)
      drawDown = profit;
  }
}


