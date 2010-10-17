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

#include "TesterTrade.h"
#include "TesterSignals.h"

#include <QtDebug>
#include <QStringList>

TesterTrade::TesterTrade ()
{
  _enterPrice = 0;
  _exitPrice = 0;
  _type = -1;
  _volume = 0;
  _enterIndex = -1;
  _exitIndex = -1;
  _enterCommission = 0;
  _exitCommission = 0;
  _profit = 0;
  _signal = TesterSignals::_None;
  _low = 0;
  _high = 0;
  _priceHigh = 0;  
  _priceLow = 0;
  _equity = 0;
}

QDateTime & TesterTrade::enterDate ()
{
  return _enterDate;
}

QDateTime & TesterTrade::exitDate ()
{
  return _exitDate;
}

double TesterTrade::enterPrice ()
{
  return _enterPrice;
}

double TesterTrade::exitPrice ()
{
  return _exitPrice;
}

int TesterTrade::type ()
{
  return _type;
}

int TesterTrade::volume ()
{
  return _volume;
}

int TesterTrade::enterIndex ()
{
  return _enterIndex;
}

int TesterTrade::exitIndex ()
{
  return _exitIndex;
}

double TesterTrade::enterCommission ()
{
  return _enterCommission;
}

double TesterTrade::exitCommission ()
{
  return _exitCommission;
}

double TesterTrade::profit ()
{
  return _profit;
}

double TesterTrade::equity ()
{
  return _equity;
}

void TesterTrade::enterTrade (QString symbol, int type, double &equity, QDateTime date, double price,
                              int volume, int index, double commission)
{
  _symbol = symbol;
  _type = type;
  _enterDate = date;
  _enterPrice = price;
  _priceHigh = price;
  _priceLow = price;
  _volume = volume;
  _enterIndex = index;
  _enterCommission = commission;

  double value = _volume * _enterPrice;
  
  equity -= value;

  equity -= _enterCommission;

  _low = value;
  _high = value;
}

void TesterTrade::exitTrade (QDateTime date, double price, int index, double commission, int signal, double &equity)
{
  _exitDate = date;
  _exitPrice = price;
  _exitIndex = index;
  _exitCommission = commission;
  _signal = signal;
  
  if (! _type)
    _profit = _volume * (_exitPrice - _enterPrice);
  else
    _profit = _volume * (_enterPrice - _exitPrice);

  equity += _volume * _enterPrice;
  equity += _profit;

  equity -= _exitCommission;

  _equity = equity;
}

void TesterTrade::tradeString (QString &s)
{
  QStringList l;

  l << _symbol;
  
  if (! _type)
    l << "L";
  else
    l << "S";
  
  l << QString::number(_volume);
  
  l << _enterDate.toString("yyyy-MM-dd");
  
  l << QString::number(_enterPrice);
  
  l << _exitDate.toString("yyyy-MM-dd");
  
  l << QString::number(_exitPrice);
  
  l << QString::number(_profit);

  TesterSignals sig;
  QString s2;
  sig.signalText((TesterSignals::Signal) _signal, s2);
  l << s2;

  l << QString::number(_equity);
  
  s = l.join(",");
}

int TesterTrade::isOpenTrade ()
{
  int rc = 0;
  if (! _exitDate.isValid())
    rc = 1;
  
  return rc;
}

void TesterTrade::update (double price)
{
  if (! _type)
    _profit = _volume * (price - _enterPrice);
  else
    _profit = _volume * (_enterPrice - price);

  if (_profit < _low)
    _low = _profit;
  
  if (_profit > _high)
    _high = _profit;

  if (price > _priceHigh)
    _priceHigh = price;

  if (price < _priceLow)
    _priceLow = price;
}

double TesterTrade::drawDown ()
{
  return _low;
}

int TesterTrade::barsHeld ()
{
  return _exitIndex - _enterIndex;
}

double TesterTrade::commissions ()
{
  return _exitCommission + _enterCommission;
}

QString & TesterTrade::symbol ()
{
  return _symbol;
}

double TesterTrade::priceHigh ()
{
  return _priceHigh;
}

double TesterTrade::priceLow ()
{
  return _priceLow;
}

