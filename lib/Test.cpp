/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "Test.h"

Test::Test ()
{
  commissions = 0;
  grossProfit = 0;
  netProfit = 0;
  totalComm = 0;
  entryComm = 0;
  exitComm = 0;

  maxDrawDown = 0;
  avgDrawDown = 0;
  winLossRatio = 0;
  winTrades = 0;
  lossTrades = 0;
  trades = 0;
  profitable = 0;
  maxWinTrade = 0;
  maxLossTrade = 0;
  avgWinTrade = 0;
  avgLossTrade = 0;
  maxWinLong = 0;
  maxLossLong = 0;
  maxWinShort = 0;
  maxLossShort = 0;
}

void Test::getName (QString &d)
{
  d = name;
}

void Test::setName (QString &d)
{
  name = d;
}

void Test::getScript (QString &d)
{
  d = script;
}

void Test::setScript (QString &d)
{
  script = d;
}

void Test::getSymbol (QString &d)
{
  d = symbol;
}

void Test::setSymbol (QString &d)
{
  symbol = d;
}

void Test::getEnterField (QString &d)
{
  d = enterField;
}

void Test::setEnterField (QString &d)
{
  enterField = d;
}

void Test::getExitField (QString &d)
{
  d = exitField;
}

void Test::setExitField (QString &d)
{
  exitField = d;
}

int Test::getBars ()
{
  return bars;
}

void Test::setBars (int d)
{
  bars = d;
}

int Test::getBarLength ()
{
  return barLength;
}

void Test::setBarLength (int d)
{
  barLength = d;
}

double Test::getEntryComm ()
{
  return entryComm;
}

void Test::setEntryComm (double d)
{
  entryComm = d;
}

double Test::getExitComm ()
{
  return exitComm;
}

void Test::setExitComm (double d)
{
  exitComm = d;
}


//  summary items

double Test::getCommissions ()
{
  return commissions;
}

void Test::setCommissions (double d)
{
  commissions = d;
}

double Test::getGrossProfit ()
{
  return grossProfit;
}

void Test::setGrossProfit (double d)
{
  grossProfit = d;
}

double Test::getNetProfit ()
{
  return netProfit;
}

void Test::setNetProfit (double d)
{
  netProfit = d;
}

double Test::getTotalComm ()
{
  return totalComm;
}

void Test::setTotalComm (double d)
{
  totalComm = d;
}

double Test::getMaxDrawDown ()
{
  return maxDrawDown;
}

void Test::setMaxDrawDown (double d)
{
  maxDrawDown = d;
}

double Test::getAvgDrawDown ()
{
  return avgDrawDown;
}

void Test::setAvgDrawDown (double d)
{
  avgDrawDown = d;
}

double Test::getWinLossRatio ()
{
  return winLossRatio;
}

void Test::setWinLossRatio (double d)
{
  winLossRatio = d;
}

int Test::getWinTrades ()
{
  return winTrades;
}

void Test::setWinTrades (int d)
{
  winTrades = d;
}

int Test::getLossTrades ()
{
  return lossTrades;
}

void Test::setLossTrades (int d)
{
  lossTrades = d;
}

int Test::getTrades ()
{
  return trades;
}

void Test::setTrades (int d)
{
  trades = d;
}

double Test::getProfitable ()
{
  return profitable;
}

void Test::setProfitable (double d)
{
  profitable = d;
}

double Test::getMaxWinTrade ()
{
  return maxWinTrade;
}

void Test::setMaxWinTrade (double d)
{
  maxWinTrade = d;
}

double Test::getMaxLossTrade ()
{
  return maxLossTrade;
}

void Test::setMaxLossTrade (double d)
{
  maxLossTrade = d;
}

double Test::getAvgWinTrade ()
{
  return avgWinTrade;
}

void Test::setAvgWinTrade (double d)
{
  avgWinTrade = d;
}

double Test::getAvgLossTrade ()
{
  return avgLossTrade;
}

void Test::setAvgLossTrade (double d)
{
  avgLossTrade = d;
}

double Test::getMaxWinLong ()
{
  return maxWinLong;
}

void Test::setMaxWinLong (double d)
{
  maxWinLong = d;
}

double Test::getMaxLossLong ()
{
  return maxLossLong;
}

void Test::setMaxLossLong (double d)
{
  maxLossLong = d;
}

double Test::getMaxWinShort ()
{
  return maxWinShort;
}

void Test::setMaxWinShort (double d)
{
  maxWinShort = d;
}

double Test::getMaxLossShort ()
{
  return maxLossShort;
}

void Test::setMaxLossShort (double d)
{
  maxLossShort = d;
}

