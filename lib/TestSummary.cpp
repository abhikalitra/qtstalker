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

#include "TestSummary.h"

#include <QDebug>

TestSummary::TestSummary ()
{
  clear();
}

void TestSummary::clear ()
{
  _equityGain = 0;
  _equity = 0;
  _totalProfit = 0;
  _profitableTrades = 0;
  _totalTrades = 0;
  _winTrades = 0;
  _loseTrades = 0;
  _maxDrawdown = 0;
  _maxLoss = 0;
  _avgProfitLoss = 0;
  _winTradesTotal = 0;
  _loseTradesTotal = 0;
  _avgBarsHeld = 0;
  _minBarsHeld = 0;
  _maxBarsHeld = 0;
  _commTotal = 0;
}

void TestSummary::setSymbol (QString d)
{
  _symbol = d;
}

QString TestSummary::symbol ()
{
  return _symbol;
}

void TestSummary::setName (QString d)
{
  _name = d;
}

QString TestSummary::name ()
{
  return _name;
}

void TestSummary::setEquityGain (double d)
{
  _equityGain = d;
}

double TestSummary::equityGain ()
{
  return _equityGain;
}

void TestSummary::setEquity (double d)
{
  _equity = d;
}

double TestSummary::equity ()
{
  return _equity;
}

void TestSummary::setTotalProfit (double d)
{
  _totalProfit = d;
}

double TestSummary::totalProfit ()
{
  return _totalProfit;
}

void TestSummary::setProfitableTrades (double d)
{
  _profitableTrades = d;
}

double TestSummary::profitableTrades ()
{
  return _profitableTrades;
}

void TestSummary::setTotalTrades (int d)
{
  _totalTrades = d;
}

int TestSummary::totalTrades ()
{
  return _totalTrades;
}

void TestSummary::setWinTrades (int d)
{
  _winTrades = d;
}

int TestSummary::winTrades ()
{
  return _winTrades;
}

void TestSummary::setLoseTrades (int d)
{
  _loseTrades = d;
}

int TestSummary::loseTrades ()
{
  return _loseTrades;
}

void TestSummary::setMaxDrawdown (double d)
{
  _maxDrawdown = d;
}

double TestSummary::maxDrawdown ()
{
  return _maxDrawdown;
}

void TestSummary::setMaxLoss (double d)
{
  _maxLoss = d;
}

double TestSummary::maxLoss ()
{
  return _maxLoss;
}

void TestSummary::setAvgProfitLoss (double d)
{
  _avgProfitLoss = d;
}

double TestSummary::avgProfitLoss ()
{
  return _avgProfitLoss;
}

void TestSummary::setWinTradesTotal (double d)
{
  _winTradesTotal = d;
}

double TestSummary::winTradesTotal ()
{
  return _winTradesTotal;
}

void TestSummary::setLoseTradesTotal (double d)
{
  _loseTradesTotal = d;
}

double TestSummary::loseTradesTotal ()
{
  return _loseTradesTotal;
}

void TestSummary::setAvgBarsHeld (int d)
{
  _avgBarsHeld = d;
}

int TestSummary::avgBarsHeld ()
{
  return _avgBarsHeld;
}

void TestSummary::setMinBarsHeld (int d)
{
  _minBarsHeld = d;
}

int TestSummary::minBarsHeld ()
{
  return _minBarsHeld;
}

void TestSummary::setMaxBarsHeld (int d)
{
  _maxBarsHeld = d;
}

int TestSummary::maxBarsHeld ()
{
  return _maxBarsHeld;
}

void TestSummary::setCommTotal (double d)
{
  _commTotal = d;
}

double TestSummary::commTotal ()
{
  return _commTotal;
}
