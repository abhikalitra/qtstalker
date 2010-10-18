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

#include "TesterThreadTrade.h"
#include "QuoteServerRequest.h"
#include "IndicatorPluginFactory.h"
#include "TesterSignals.h"

#include <QDebug>

TesterThreadTrade::TesterThreadTrade (TesterSettings &set)
{
  _settings = set;
  _stops.setSettings(set);
  _status = _None;
  _barLoop = 0;
  _currentTrade = -1;
}

TesterThreadTrade::~TesterThreadTrade ()
{
  _elIndicator.clear();
  _xlIndicator.clear();
  _esIndicator.clear();
  _xsIndicator.clear();
}

int TesterThreadTrade::getBars (QString symbol)
{
  _bars.setKey(symbol);
  
  QStringList tl;
  tl << "Quotes" << "Date" << _bars.getExchange() << _bars.getSymbol();
  QString s;
  _bars.barLengthText((BarData::BarLength) _settings.period(), s);
  tl << s << "0" << "0" << QString::number(_settings.dateRange());
  QString command = tl.join(",") + "\n";

  QuoteServerRequest qsr;
  if (qsr.run(command))
  {
    qDebug() << "TesterThreadTrade::getBars: qsr error";
    return 1;
  }

  _bars.setBarLength((BarData::BarLength) _settings.period());
  _bars.setBars(qsr.data());

  return 0;
}

int TesterThreadTrade::getIndicator ()
{
  _elIndicator.setSettings(_settings.eLSettings());
  if (getIndicator2(_elIndicator, _settings.eLIndicator()))
    return 1;
  
  _xlIndicator.setSettings(_settings.xLSettings());
  if (getIndicator2(_xlIndicator, _settings.xLIndicator()))
    return 1;

  _esIndicator.setSettings(_settings.eSSettings());
  if (getIndicator2(_esIndicator, _settings.eSIndicator()))
    return 1;
  
  _xsIndicator.setSettings(_settings.xSSettings());
  if (getIndicator2(_xsIndicator, _settings.xSIndicator()))
    return 1;

  return 0;
}

int TesterThreadTrade::getIndicator2 (Indicator &i, QString &name)
{
  IndicatorPluginFactory fac;
  IndicatorPlugin *plug = fac.plugin(name);
  if (! plug)
  {
    qDebug() << "TesterThreadTrade::getIndicator2: no plugin" << name;
    return 1;
  }

  if (plug->getIndicator(i, _bars))
  {
    qDebug() << "TesterThreadTrade::getIndicator2: indicator error" << name;
    return 1;
  }

  return 0;
}

int TesterThreadTrade::getRules ()
{
  if (_enterLongRules.createRules(_elIndicator, _settings.enterLong(), _bars))
    return 1;

  if (_exitLongRules.createRules(_xlIndicator, _settings.exitLong(), _bars))
    return 1;

  if (_enterShortRules.createRules(_esIndicator, _settings.enterShort(), _bars))
    return 1;

  if (_exitShortRules.createRules(_xsIndicator, _settings.exitShort(), _bars))
    return 1;

  return 0;
}

int TesterThreadTrade::next (QList<TesterTrade *> &trades, double &equity)
{
  if (_barLoop >= _bars.count())
  {
    if (_currentTrade == -1)
      return 1;
    
    // check if we have an open trade
    TesterTrade *t = trades.at(_currentTrade);
    if (t->isOpenTrade())
      exitTrade(trades, _bars.count() - 1, TesterSignals::_TestEnd, equity);
    _currentTrade = -1;
    
    return 1;
  }
  
  switch(_status)
  {
    case _None:
      if (_settings.getLong())
      {
        if (enterTradeCheck(trades, _enterLongRules, _elIndicator))
        {
          _status = _EnterLong;
          break;
        }
      }
          
      if (_settings.getShort())
      {
        if (enterTradeCheck(trades, _enterShortRules, _esIndicator))
          _status = _EnterShort;
      }
      break;
    case _EnterLong:
      _status = _Long;
      if (enterTrade(trades, _bars.getKey(), equity))
        _status = _None;
      break;
    case _EnterShort:
      _status = _Short;
      if (enterTrade(trades, _bars.getKey(), equity))
        _status = _None;
      break;
    case _Long:
      if (enterTradeCheck(trades, _exitLongRules, _xlIndicator))
      {
        _status = _ExitTrade;
        break;
      }

      if (_stops.checkStops(trades, _currentTrade, _bars, _barLoop))
        _status = _ExitStop;
      break;
    case _Short:
      if (enterTradeCheck(trades, _exitShortRules, _xsIndicator))
      {
        _status = _ExitTrade;
        break;
      }
          
      if (_stops.checkStops(trades, _currentTrade, _bars, _barLoop))
        _status = _ExitStop;
      break;
    case _ExitTrade:
    {
      TesterTrade *trade = trades.at(_currentTrade);
      int signal = TesterSignals::_ExitLong;
      if (trade->type())
        signal = TesterSignals::_ExitShort;
          
      exitTrade(trades, _barLoop, signal, equity);
      _status = _None;
      break;
    }
    case _ExitStop:
      exitTrade(trades, _barLoop, _stops.triggered(), equity);
      _status = _None;
      break;
    default:
      break;
  }

  _barLoop++;

  return 0;
}

int TesterThreadTrade::enterTradeCheck (QList<TesterTrade *> &trades, IndicatorPlotRules &rules, Indicator &indicator)
{
  // update current trade
  if (trades.count())
  {
    TesterTrade *t = trades.at(_currentTrade);
    Bar bar = _bars.getBar(_barLoop);
    double price = bar.getData((Bar::BarField) _settings.longSellPrice());
    if (t->type())
      price = bar.getData((Bar::BarField) _settings.shortSellPrice());
    t->update(price);
  }

  return rules.test(indicator, _barLoop);
}

int TesterThreadTrade::enterTrade (QList<TesterTrade *> &trades, QString symbol, double &equity)
{
  TesterTrade *t = new TesterTrade;

  // type 0 == long, 1 == short
  int type = 0;
  if (_status == _Short)
    type = 1;

  Bar bar = _bars.getBar(_barLoop);

  // enter price
  double price = bar.getData((Bar::BarField) _settings.longBuyPrice());
  if (type)
    price = bar.getData((Bar::BarField) _settings.shortBuyPrice());

  // figure out volume
  int volume = (equity * _settings.positionSize()) / price;
  if (volume < 1)
    return 1;

  double comm = getCommission(t, 0);
  
  t->enterTrade(symbol, type, equity, bar.date(), price, volume, _barLoop, comm);

  trades.append(t);
  _currentTrade = trades.count() - 1;

  return 0;
}

void TesterThreadTrade::exitTrade (QList<TesterTrade *> &trades, int index, int signal, double &equity)
{
  TesterTrade *t = trades.at(_currentTrade);
  if (! t)
    return;

  Bar bar = _bars.getBar(index);
  
  double price = bar.getData((Bar::BarField) _settings.longSellPrice());
  if (t->type())
    price = bar.getData((Bar::BarField) _settings.shortSellPrice());

  double comm = getCommission(t, 1);

  t->exitTrade(bar.date(), price, index, comm, signal, equity);
}

double TesterThreadTrade::getCommission (TesterTrade *trade, int flag)
{
  double commission = 0;
  double value = _settings.commissionValue();
  
  switch (_settings.commission())
  {
    case 0:
      if (! flag)
        commission = (trade->volume() * trade->enterPrice()) * value;
      else
        commission = (trade->volume() * trade->exitPrice()) * value;
      break;
    case 1:
      commission = value;
      break;
    case 2:
      commission = value * trade->volume();
      break;
    default:
      break;
  }

  return commission;
}

