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

#include "TesterStops.h"
#include "TesterSignals.h"

#include <QDebug>

TesterStops::TesterStops ()
{
  _triggered = 0;
}

void TesterStops::setSettings (TesterSettings &set)
{
  _settings = set;
}

int TesterStops::checkStops (QList<TesterTrade *> &trades, int tindex, BarData &bars, int index)
{
  _triggered = 0;
  
  _triggered = maximumLossStop(trades, tindex, bars, index);
  if (_triggered)
    return _triggered;

  _triggered = profitTargetStop(trades, tindex, bars, index);
  if (_triggered)
    return _triggered;

  _triggered = trailingStop(trades, tindex, bars, index);
  if (_triggered)
    return _triggered;

  _triggered = barsStop(trades, tindex, index);
  if (_triggered)
    return _triggered;

  return _triggered;
}

int TesterStops::maximumLossStop (QList<TesterTrade *> &trades, int tindex, BarData &bars, int index)
{
  if (! _settings.maximumLossStop())
    return TesterSignals::_None;
  
  if (! trades.count())
    return TesterSignals::_None;
  
  TesterTrade *t = trades.at(tindex);

  Bar bar = bars.getBar(index);
  double price = bar.getData((Bar::BarField) _settings.maximumLossExit());

  if (! _settings.maximumLossType())
  {
    if (! t->type())
    {
      double v = t->enterPrice() * (1 - (_settings.maximumLossValue() / 100));
      if (price <= v)
        return TesterSignals::_MaximumLossStop;
    }
    else
    {
      double v = t->enterPrice() * (1 + (_settings.maximumLossValue() / 100));
      if (price >= v)
        return TesterSignals::_MaximumLossStop;
    }
  }
  else
  {
    if (! t->type())
    {
      double v = t->enterPrice() - _settings.maximumLossValue();
      if (price <= v)
        return TesterSignals::_MaximumLossStop;
    }
    else
    {
      double v = t->enterPrice() + _settings.maximumLossValue();
      if (price >= v)
        return TesterSignals::_MaximumLossStop;
    }
  }
  
  return TesterSignals::_None;
}

int TesterStops::profitTargetStop (QList<TesterTrade *> &trades, int tindex, BarData &bars, int index)
{
  if (! _settings.profitTargetStop())
    return TesterSignals::_None;

  if (! trades.count())
    return TesterSignals::_None;

  TesterTrade *t = trades.at(tindex);
  
  Bar bar = bars.getBar(index);
  double price = bar.getData((Bar::BarField) _settings.profitTargetExit());

  if (! _settings.profitTargetType())
  {
    if (! t->type())
    {
      double v = t->enterPrice() * (1 + (_settings.maximumLossValue() / 100));
      if (price >= v)
        return TesterSignals::_ProfitTargetStop;
    }
    else
    {
      double v = t->enterPrice() * (1 - (_settings.maximumLossValue() / 100));
      if (price <= v)
        return TesterSignals::_ProfitTargetStop;
    }
  }
  else
  {
    if (! t->type())
    {
      double v = t->enterPrice() + _settings.maximumLossValue();
      if (price >= v)
        return TesterSignals::_ProfitTargetStop;
    }
    else
    {
      double v = t->enterPrice() - _settings.maximumLossValue();
      if (price <= v)
        return TesterSignals::_ProfitTargetStop;
    }
  }

  return TesterSignals::_None;
}

int TesterStops::trailingStop (QList<TesterTrade *> &trades, int tindex, BarData &bars, int index)
{
  if (! _settings.trailingStop())
    return TesterSignals::_None;

  if (! trades.count())
    return TesterSignals::_None;

  TesterTrade *t = trades.at(tindex);

  Bar bar = bars.getBar(index);
  double price = bar.getData((Bar::BarField) _settings.trailingExit());
  
  if (! _settings.maximumLossType())
  {
    if (! t->type())
    {
      double v = t->priceHigh() * (1 - (_settings.maximumLossValue() / 100));
      if (price <= v)
        return TesterSignals::_TrailingStop;
    }
    else
    {
      double v = t->priceLow() * (1 + (_settings.maximumLossValue() / 100));
      if (price >= v)
        return TesterSignals::_TrailingStop;
    }
  }
  else
  {
    if (! t->type())
    {
      double v = t->priceHigh() - _settings.maximumLossValue();
      if (price <= v)
        return TesterSignals::_TrailingStop;
    }
    else
    {
      double v = t->priceLow() + _settings.maximumLossValue();
      if (price >= v)
        return TesterSignals::_TrailingStop;
    }
  }

  return TesterSignals::_None;
}

int TesterStops::barsStop (QList<TesterTrade *> &trades, int tindex, int index)
{
  if (! _settings.barsStop())
    return TesterSignals::_None;

  if (! trades.count())
    return TesterSignals::_None;

  TesterTrade *t = trades.at(tindex);
  int bars = index - t->enterIndex();
  if (bars >= _settings.barsStopValue())
    return TesterSignals::_BarsStop;

  return TesterSignals::_None;
}

int TesterStops::triggered ()
{
  return _triggered;
}

