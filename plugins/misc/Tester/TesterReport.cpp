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

#include "TesterReport.h"

#include <QtDebug>

TesterReport::TesterReport ()
{
  _profitFactor = 0;
  _payoffRatio = 0;
  _equityGain = 0;
  _equity = 0;
  _totalProfit = 0;
  _profitableTrades = 0;
  _totalTrades = 0;
  _winTrades = 0;
  _loseTrades = 0;
  _maxDrawdown = 0;
  _avgProfitLoss = 0;
  _totalWinTrades = 0;
  _totalLoseTrades = 0;
  _avgBarsHeld = 0;
  _minBarsHeld = 0;
  _maxBarsHeld = 0;
  _totalCommissions = 0;
}

void TesterReport::setName (QString d)
{
  _name = d;
}

QString & TesterReport::name ()
{
  return _name;
}

void TesterReport::setTrades (QStringList d)
{
  _trades = d;
}

QStringList & TesterReport::trades ()
{
  return _trades;
}

void TesterReport::setProfitFactor (double d)
{
  _profitFactor = d;
}

double TesterReport::profitFactor ()
{
  return _profitFactor;
}

void TesterReport::setPayoffRatio (double d)
{
  _payoffRatio = d;
}

double TesterReport::payoffRatio ()
{
  return _payoffRatio;
}

void TesterReport::setEquityGain (double d)
{
  _equityGain = d;
}

double TesterReport::equityGain ()
{
  return _equityGain;
}

void TesterReport::setEquity (double d)
{
  _equity = d;
}

double TesterReport::equity ()
{
  return _equity;
}

void TesterReport::setTotalProfit (double d)
{
  _totalProfit = d;
}

double TesterReport::totalProfit ()
{
  return _totalProfit;
}

void TesterReport::setProfitableTrades (double d)
{
  _profitableTrades = d;
}

double TesterReport::profitableTrades ()
{
  return _profitableTrades;
}

void TesterReport::setTotalTrades (int d)
{
  _totalTrades = d;
}

int TesterReport::totalTrades ()
{
  return _totalTrades;
}

void TesterReport::setWinTrades (int d)
{
  _winTrades = d;
}

int TesterReport::winTrades ()
{
  return _winTrades;
}

void TesterReport::setLoseTrades (int d)
{
  _loseTrades = d;
}

int TesterReport::loseTrades ()
{
  return _loseTrades;
}

void TesterReport::setMaxDrawdown (double d)
{
  _maxDrawdown = d;
}

double TesterReport::maxDrawdown ()
{
  return _maxDrawdown;
}

void TesterReport::setAvgProfitLoss (double d)
{
  _avgProfitLoss = d;
}

double TesterReport::avgProfitLoss ()
{
  return _avgProfitLoss;
}

void TesterReport::setTotalWinTrades (double d)
{
  _totalWinTrades = d;
}

double TesterReport::totalWinTrades ()
{
  return _totalWinTrades;
}

void TesterReport::setTotalLoseTrades (double d)
{
  _totalLoseTrades = d;
}

double TesterReport::totalLoseTrades ()
{
  return _totalLoseTrades;
}

void TesterReport::setAvgBarsHeld (int d)
{
  _avgBarsHeld = d;
}

int TesterReport::avgBarsHeld ()
{
  return _avgBarsHeld;
}

void TesterReport::setMinBarsHeld (int d)
{
  _minBarsHeld = d;
}

int TesterReport::minBarsHeld ()
{
  return _minBarsHeld;
}

void TesterReport::setMaxBarsHeld (int d)
{
  _maxBarsHeld = d;
}

int TesterReport::maxBarsHeld ()
{
  return _maxBarsHeld;
}

void TesterReport::setTotalCommissions (double d)
{
  _totalCommissions = d;
}

double TesterReport::totalCommissions ()
{
  return _totalCommissions;
}

