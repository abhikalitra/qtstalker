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
  _equity = 0;
  _enterCommission = 0;
  _exitCommission = 0;
  _profit = 0;
  _signal = _None;
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

double TesterTrade::equity ()
{
  return _equity;
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

void TesterTrade::enterTrade (int type, double equity, QDateTime date, double price, int volume, int index,
                              double commission)
{
  _type = type;
  _equity = equity;
  _enterDate = date;
  _enterPrice = price;
  _volume = volume;
  _enterIndex = index;
  _enterCommission = commission;

  _equity -= _volume * _enterPrice;

  _equity -= _enterCommission;
}

void TesterTrade::exitTrade (QDateTime date, double price, int index, double commission, int signal)
{
  _exitDate = date;
  _exitPrice = price;
  _exitIndex = index;
  _exitCommission = commission;
  _signal = (Signal) signal;
  
  if (! _type)
    _profit = _volume * (_exitPrice - _enterPrice);
  else
    _profit = _volume * (_enterPrice - _exitPrice);

  _equity += _profit;

  _equity -= _exitCommission;
}

void TesterTrade::tradeString (QString &s)
{
  QStringList l;
  
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

  switch (_signal)
  {
    case _None:
      l << "None";
      break;
    case _ExitLong:
      l << "Exit Long";
      break;
    case _ExitShort:
      l << "Exit Short";
      break;
    case _TestEnd:
      l << "Test End";
      break;
    default:
      break;
  }

  s = l.join(",");
}

int TesterTrade::isOpenTrade ()
{
  int rc = 0;
  if (! _exitDate.isValid())
    rc = 1;
  
  return rc;
}

