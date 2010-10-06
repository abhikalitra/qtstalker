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

#include "TesterSettings.h"

#include <QtDebug>

TesterSettings::TesterSettings ()
{
  _equity = 10000;
  _period = 6;
  _dateRange = 5;
  _positionSize = 0.1;
  _futuresMode = 0;
  _commission = 1;
  _commissionValue = 10;
  _long = 0;
  _longBuyPrice = 0;
  _longSellPrice = 3;
  _short = 0;
  _shortBuyPrice = 0;
  _shortSellPrice = 3;
  _maximumLossStop = 0;
  _maximumLossType = 0;
  _maximumLossValue = 0.1;
  _maximumLossExit = 3;
  _profitTargetStop = 0;
  _profitTargetType = 0;
  _profitTargetValue = 0.1;
  _profitTargetExit = 3;
  _trailingStop = 0;
  _trailingType = 0;
  _trailingValue = 0.1;
  _trailingExit = 3;
  _barsStop = 0;
  _barsStopValue = 50;
  _barsStopExit = 3;
}

void TesterSettings::setName (QString d)
{
  _name = d;
}

QString & TesterSettings::name ()
{
  return _name;
}

void TesterSettings::setEquity (double d)
{
  _equity = d;
}

double TesterSettings::equity ()
{
  return _equity;
}

void TesterSettings::setPeriod (int d)
{
  _period = d;
}

int TesterSettings::period ()
{
  return _period;
}

void TesterSettings::setDateRange (int d)
{
  _dateRange = d;
}

int TesterSettings::dateRange ()
{
  return _dateRange;
}

void TesterSettings::setPositionSize (double d)
{
  _positionSize = d;
}

double TesterSettings::positionSize ()
{
  return _positionSize;
}

void TesterSettings::setFuturesMode (int d)
{
  _futuresMode = d;
}

int TesterSettings::futuresMode ()
{
  return _futuresMode;
}

void TesterSettings::setCommission (int d)
{
  _commission = d;
}

int TesterSettings::commission ()
{
  return _commission;
}

void TesterSettings::setCommissionValue (double d)
{
  _commissionValue = d;
}

double TesterSettings::commissionValue ()
{
  return _commissionValue;
}

void TesterSettings::setLong (int d)
{
  _long = d;
}

int TesterSettings::getLong ()
{
  return _long;
}

void TesterSettings::setLongBuyPrice (int d)
{
  _longBuyPrice = d;
}

int TesterSettings::longBuyPrice ()
{
  return _longBuyPrice;
}

void TesterSettings::setLongSellPrice (int d)
{
  _longSellPrice = d;
}

int TesterSettings::longSellPrice ()
{
  return _longSellPrice;
}

void TesterSettings::setShort (int d)
{
  _short = d;
}

int TesterSettings::getShort ()
{
  return _short;
}

void TesterSettings::setShortBuyPrice (int d)
{
  _shortBuyPrice = d;
}

int TesterSettings::shortBuyPrice ()
{
  return _shortBuyPrice;
}

void TesterSettings::setShortSellPrice (int d)
{
  _shortSellPrice = d;
}

int TesterSettings::shortSellPrice ()
{
  return _shortSellPrice;
}

void TesterSettings::setMaximumLossStop (int d)
{
  _maximumLossStop = d;
}

int TesterSettings::maximumLossStop ()
{
  return _maximumLossStop;
}

void TesterSettings::setMaximumLossType (int d)
{
  _maximumLossType = d;
}

int TesterSettings::maximumLossType ()
{
  return _maximumLossType;
}

void TesterSettings::setMaximumLossValue (double d)
{
  _maximumLossValue = d;
}

double TesterSettings::maximumLossValue ()
{
  return _maximumLossValue;
}

void TesterSettings::setMaximumLossExit (int d)
{
  _maximumLossExit = d;
}

int TesterSettings::maximumLossExit ()
{
  return _maximumLossExit;
}

void TesterSettings::setProfitTargetStop (int d)
{
  _profitTargetStop = d;
}

int TesterSettings::profitTargetStop ()
{
  return _profitTargetStop;
}

void TesterSettings::setProfitTargetType (int d)
{
  _profitTargetType = d;
}

int TesterSettings::profitTargetType ()
{
  return _profitTargetType;
}

void TesterSettings::setProfitTargetValue (double d)
{
  _profitTargetValue = d;
}

double TesterSettings::profitTargetValue ()
{
  return _profitTargetValue;
}

void TesterSettings::setProfitTargetExit (int d)
{
  _profitTargetExit = d;
}

int TesterSettings::profitTargetExit ()
{
  return _profitTargetExit;
}

void TesterSettings::setTrailingStop (int d)
{
  _trailingStop = d;
}

int TesterSettings::trailingStop ()
{
  return _trailingStop;
}

void TesterSettings::setTrailingType (int d)
{
  _trailingType = d;
}

int TesterSettings::trailingType ()
{
  return _trailingType;
}

void TesterSettings::setTrailingValue (double d)
{
  _trailingValue = d;
}

double TesterSettings::trailingValue ()
{
  return _trailingValue;
}

void TesterSettings::setTrailingExit (int d)
{
  _trailingExit = d;
}

int TesterSettings::trailingExit ()
{
  return _trailingExit;
}

void TesterSettings::setBarsStop (int d)
{
  _barsStop = d;
}

int TesterSettings::barsStop ()
{
  return _barsStop;
}

void TesterSettings::setBarsStopValue (double d)
{
  _barsStopValue = d;
}

double TesterSettings::barsStopValue ()
{
  return _barsStopValue;
}

void TesterSettings::setBarsStopExit (int d)
{
  _barsStopExit = d;
}

int TesterSettings::barsStopExit ()
{
  return _barsStopExit;
}

void TesterSettings::setReport (QString d)
{
  _report = d;
}

QString & TesterSettings::report ()
{
  return _report;
}

void TesterSettings::setTrades (QStringList d)
{
  _trades = d;
}

QStringList & TesterSettings::trades ()
{
  return _trades;
}

void TesterSettings::setIndicator (QString d)
{
  _indicator = d;
}

QString & TesterSettings::indicator ()
{
  return _indicator;
}

void TesterSettings::setPlotNames (QStringList d)
{
  _plotNames = d;
}

QStringList & TesterSettings::plotNames ()
{
  return _plotNames;
}

void TesterSettings::setEnterLong (QStringList d)
{
  _enterLong = d;
}

QStringList & TesterSettings::enterLong ()
{
  return _enterLong;
}

void TesterSettings::setExitLong (QStringList d)
{
  _exitLong = d;
}

QStringList & TesterSettings::exitLong ()
{
  return _exitLong;
}

void TesterSettings::setEnterShort (QStringList d)
{
  _enterShort = d;
}

QStringList & TesterSettings::enterShort ()
{
  return _enterShort;
}

void TesterSettings::setExitShort (QStringList d)
{
  _exitShort = d;
}

QStringList & TesterSettings::exitShort ()
{
  return _exitShort;
}

void TesterSettings::setSymbols (QStringList d)
{
  _symbols = d;
}

QStringList & TesterSettings::symbols ()
{
  return _symbols;
}

void TesterSettings::setIndicatorSettings (Setting d)
{
  _indicatorSettings = d;
}

Setting & TesterSettings::indicatorSettings ()
{
  return _indicatorSettings;
}

