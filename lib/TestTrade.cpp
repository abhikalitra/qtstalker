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

#include "TestTrade.h"
#include "TestSignal.h"

#include <QDebug>

TestTrade::TestTrade ()
{
  clear();
}

void TestTrade::clear ()
{
  _type = "L";
  _enterPrice = 0;
  _exitPrice = 0;
  _volume = 0.02;
  _barsHeld = 0;
  _enterComm = 10;
  _exitComm = 10;
  _drawdown = 0;
  _signal = 0;
  _profit = 0;
  _trailingStopPercentage = 0;
  _trailingStop = 0;
  _trailingStopFlag = FALSE;
  _maxLossStopPercentage = 0;
  _maxLossStopFlag = FALSE;
}

int TestTrade::enterTrade (QString type, double volume, double equity, double price, QDateTime dt, double comm)
{
  _volume = 1;
  if (volume != 0)
  {
    _volume = (equity * volume) / price;
    if (! _volume)
      return 1;
  }

  _type = type;
  _enterDate = dt;
  _enterPrice = price;
  _barsHeld = 1;
  _enterComm = comm;

  return 0;
}

int TestTrade::updateTrade (double price, QDateTime)
{
  if (_type == "L")
    _profit = _volume * (price - _enterPrice); // long
  else
    _profit = _volume * (_enterPrice - price); // short

  if (_profit < _drawdown)
    _drawdown = _profit;

  _barsHeld++;

  if (maxLossStop(price))
    return TestSignal::_MAXIMUM_LOSS_STOP;

  if (trailingStop(price))
    return TestSignal::_TRAILING_STOP;

  return 0;
}

void TestTrade::exitTrade (int signal, double price, QDateTime dt, double comm)
{
  _exitDate = dt;
  _exitPrice = price;
  _signal = signal;
  _exitComm = comm;

  if (_type == "L")
    _profit = _volume * (_exitPrice - _enterPrice); // long
  else
    _profit = _volume * (_enterPrice - _exitPrice); // short
}

void TestTrade::setSymbol (QString d)
{
  _symbol = d;
}

QString TestTrade::symbol ()
{
  return _symbol;
}

void TestTrade::setName (QString d)
{
  _name = d;
}

QString TestTrade::name ()
{
  return _name;
}

void TestTrade::setType (QString d)
{
  _type = d;
}

QString TestTrade::type ()
{
  return _type;
}

void TestTrade::setEnterDate (QDateTime d)
{
  _enterDate = d;
}

QDateTime TestTrade::enterDate ()
{
  return _enterDate;
}

void TestTrade::setExitDate (QDateTime d)
{
  _exitDate = d;
}

QDateTime TestTrade::exitDate ()
{
  return _exitDate;
}

void TestTrade::setEnterPrice (double d)
{
  _enterPrice = d;
}

double TestTrade::enterPrice ()
{
  return _enterPrice;
}

void TestTrade::setExitPrice (double d)
{
  _exitPrice = d;
}

double TestTrade::exitPrice ()
{
  return _exitPrice;
}

void TestTrade::setVolume (double d)
{
  _volume = d;
}

double TestTrade::volume ()
{
  return _volume;
}

void TestTrade::setBarsHeld (int d)
{
  _barsHeld = d;
}

int TestTrade::barsHeld ()
{
  return _barsHeld;
}

void TestTrade::setEnterComm (double d)
{
  _enterComm = d;
}

double TestTrade::enterComm ()
{
  return _enterComm;
}

void TestTrade::setExitComm (double d)
{
  _exitComm = d;
}

double TestTrade::exitComm ()
{
  return _exitComm;
}

void TestTrade::setDrawdown (double d)
{
  _drawdown = d;
}

double TestTrade::drawdown ()
{
  return _drawdown;
}

void TestTrade::setSignal (int d)
{
  _signal = d;
}

int TestTrade::signal ()
{
  return _signal;
}

void TestTrade::setProfit (double d)
{
  _profit = d;
}

double TestTrade::profit ()
{
  return _profit;
}

void TestTrade::setTrailingStop (double d)
{
  if (d == 0)
    return;

  _trailingStopPercentage = d;
  _trailingStop = 0;
  _trailingStopFlag = TRUE;
}

int TestTrade::trailingStop (double price)
{
  if (! _trailingStopFlag)
    return 0;

  double risk = price * _trailingStopPercentage;

  if (_trailingStop == 0)
  {
    if (_type == "L")
      _trailingStop = _enterPrice - (_enterPrice * _trailingStopPercentage);
    else
      _trailingStop = _enterPrice + (_enterPrice * _trailingStopPercentage);
  }

  if (_type == "L")
  {
    double stop = price - risk;

    if (stop > _trailingStop)
      _trailingStop = stop;

    if (stop < _trailingStop)
      return 1;
  }
  else
  {
    double stop = price + risk;

    if (stop < _trailingStop)
      _trailingStop = stop;

    if (stop > _trailingStop)
      return 1;
  }

  return 0;
}

void TestTrade::setMaxLossStop (double d)
{
  if (d == 0)
    return;

  _maxLossStopPercentage = d;
  _maxLossStopFlag = TRUE;
}

int TestTrade::maxLossStop (double price)
{
  if (! _maxLossStopFlag)
    return 0;

  double risk = (_volume * _enterPrice) * _maxLossStopPercentage;

  double profit = 0;
  if (_type == "L")
    profit = _volume * (price - _enterPrice); // long
  else
    profit = _volume * (_enterPrice - price); // short

  if (profit * 1 >= risk)
    return 1;

  return 0;
}
