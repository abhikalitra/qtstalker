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

#include "Test.h"
#include "Strip.h"
#include "TestSignal.h"
#include "TestDataBase.h"
#include "CurveBar.h"
#include "Message.h"

#include <QtDebug>

Test::Test (QObject *p) : QObject (p)
{
  clear();
}

Test::~Test ()
{
  clear();
}

void Test::clear ()
{
  _enterLong = 0;
  _exitLong = 0;
  _enterShort = 0;
  _exitShort = 0;
  _volume = 0;
  _enterComm = 10;
  _exitComm = 10;
  _equity = 1000;
  _startEquity = 1000;
  _date = 0;
  _open = 0;
  _high = 0;
  _low = 0;
  _close = 0;
  _name.clear();
  _symbol.clear();
  _maxLossStop = 0;
  _maxLossFlag = FALSE;
  _trailingStop = 0;
  _trailingFlag = FALSE;
  _volume = 0.02;

  qDeleteAll(_trades);
  _trades.clear();
}

int Test::run ()
{
  int status = _NONE;

  QList<Data *> list;
  list << _date << _open << _high << _low << _close;

//  VerifyDataInput vdi;
  QList<int> keys;
//  if (vdi.curveKeys(list, keys))
//  {
//    qDebug() << "Test::run: invalid keys";
//    return 1;
//  }

  int rc = 0;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    int pos = keys.at(loop);
    switch (status)
    {
      case _LONG:
      {
        rc = updateTrade(pos);
        if (rc)
        {
          exitTrade(pos, rc);
          status = _NONE;
          break;
        }

        Data *bar = _exitLong->toData(keys.at(pos));
        if (bar)
        {
          double d = bar->toData(CurveBar::_VALUE)->toDouble();
          if (d > 0)
          {
            status = _NONE;
            exitTrade(pos, (int) TestSignal::_EXIT_LONG);
            continue;
          }
        }
        break;
      }
      case _SHORT:
      {
        rc = updateTrade(pos);
        if (rc)
        {
          exitTrade(pos, rc);
          status = _NONE;
        }

        Data *bar = _exitShort->toData(keys.at(pos));
        if (bar)
        {
          double d = bar->toData(CurveBar::_VALUE)->toDouble();
          if (d > 0)
          {
            status = _NONE;
            exitTrade(pos, (int) TestSignal::_EXIT_SHORT);
            continue;
          }
        }
        break;
      }
      default: // no trade
      {
        // if we are on the last bar, dont enter any trades
        if (loop == keys.count() - 1)
          continue;

        // check if we enter long trade
        if (_enterLong)
        {
          Data *bar = _enterLong->toData(keys.at(pos));
          if (bar)
          {
            double d = bar->toData(CurveBar::_VALUE)->toDouble();
            if (d > 0)
            {
              if (enterTrade(1, keys.at(pos)))
                continue;

              status = _LONG;
              continue;
            }
          }
        }

        // check if we enter short trade
        if (_enterShort)
        {
          Data *bar = _enterShort->toData(keys.at(pos));
          if (bar)
          {
            double d = bar->toData(CurveBar::_VALUE)->toDouble();
            if (d > 0)
            {
              if (enterTrade(-1, keys.at(pos)))
                continue;

              status = _SHORT;
              continue;
            }
          }
        }
      }
    }
  }

  // check if this is the last bar and close the trade
  TestTrade *trade = _trades.at(_trades.count() - 1);
  if (! trade)
    return 1;

  QDateTime dt = trade->exitDate();
  if (! dt.isValid())
    exitTrade(loop - 1, (int) TestSignal::_TEST_END);

  return 0;
}

int Test::enterTrade (int status, int pos)
{
  Data *bar = _open->toData(pos);
  if (! bar)
    return 1;

  Data *date = _date->toData(pos);
  if (! date)
    return 1;

  TestTrade *trade = new TestTrade;
  _trades.append(trade);

  trade->setSymbol(_symbol);
  trade->setName(_name);
  trade->setMaxLossStop(_maxLossStop);
  trade->setTrailingStop(_trailingStop);

  QString type = "L";
  if (status == _SHORT)
    type = "S";

  if (trade->enterTrade(type,
                        _volume,
                        _equity,
                        bar->toData(CurveBar::_VALUE)->toDouble(),
                        date->toData(CurveBar::_DATE)->toDateTime(),
                        _enterComm))
    return 1;

  double value = trade->volume() * trade->enterPrice();

  _equity -= value;
  _equity -= _enterComm;

  return 0;
}

int Test::exitTrade (int pos, int signal)
{
  Data *bar = _close->toData(pos);
  if (! bar)
    return 1;

  TestTrade *trade = _trades.at(_trades.count() - 1);
  if (! trade)
    return 1;

  Data *date = _date->toData(pos);
  if (! date)
    return 1;

  trade->exitTrade(signal,
                   bar->toData(CurveBar::_VALUE)->toDouble(),
                   date->toData(CurveBar::_DATE)->toDateTime(),
                   _exitComm);

  double value = trade->volume() * bar->toData(CurveBar::_VALUE)->toDouble();
  _equity += value;
  _equity -= _exitComm;

  return 0;
}

int Test::updateTrade (int pos)
{
  Data *bar = _close->toData(pos);
  if (! bar)
    return 0;

  Data *date = _date->toData(pos);
  if (! date)
    return 0;

  TestTrade *trade = _trades.at(_trades.count() - 1);
  if (! trade)
    return 0;

  return trade->updateTrade(bar->toData(CurveBar::_VALUE)->toDouble(), date->toData(CurveBar::_DATE)->toDateTime());
}

int Test::saveSummary ()
{
  int win = 0;
  int loss = 0;
  int conLoss = 0;
  int tConLoss = 0;
  double winTotal = 0;
  double lossTotal = 0;
  double profit = 0;
  double drawDown = 0;
  double tDrawDown = 0;
  int maxBars = 0;
  int minBars = 99999999;
  int totalBars = 0;
  double commissions = 0;
  double equity = 0;
  int loop = 0;
  TestSummary report;
  for (; loop < _trades.count(); loop++)
  {
    TestTrade *trade = _trades.at(loop);

    if (trade->profit() < 0)
    {
      loss++;
      lossTotal += trade->profit();

      tConLoss++;
      if (tConLoss > conLoss)
	conLoss = tConLoss;

      tDrawDown += trade->profit();
      if (tDrawDown < drawDown)
	drawDown = tDrawDown;
    }
    else
    {
      win++;
      winTotal += trade->profit();

      tConLoss = 0;
      tDrawDown = 0;
    }

    profit += trade->profit();

    int bars = trade->barsHeld();
    totalBars += bars;
    if (bars < minBars)
      minBars = bars;
    if (bars > maxBars)
      maxBars = bars;

    commissions += trade->enterComm();
    commissions += trade->exitComm();

//    equity = trade->value("EQUITY").toDouble();
  }

  Strip strip;
  QString s;
  double t = 0;
  t = (double) ((equity - _startEquity) / _startEquity) * 100;
  report.setEquityGain(t);

  report.setEquity(equity);
  report.setTotalProfit(profit);

  t = (double) (((double) win / (double) _trades.count()) * 100);
  report.setProfitableTrades(t);

  report.setTotalTrades(_trades.count());
  report.setWinTrades(win);
  report.setLoseTrades(loss);
  report.setMaxDrawdown(drawDown);
  report.setMaxLoss(conLoss);

  t = (double) (winTotal / (lossTotal * -1));
  report.setAvgProfitLoss(t);

  report.setWinTradesTotal(winTotal);
  report.setLoseTradesTotal(lossTotal);

  t = (int) totalBars / _trades.count();
  report.setAvgBarsHeld(t);

  report.setMinBarsHeld(minBars);
  report.setMaxBarsHeld(maxBars);
  report.setCommTotal(commissions);

  // save summary
  report.setName(_name);
  report.setSymbol(_symbol);

  TestDataBase db;
  db.transaction();
  if (db.saveSummary(&report, _trades))
    return 1;
  db.commit();

  return 0;
}

void Test::setName (QString d)
{
  _name = d;
}

void Test::setSymbol (QString d)
{
  _symbol = d;
}

void Test::setEnterLong (Data *d)
{
  _enterLong = d;
}

void Test::setExitLong (Data *d)
{
  _exitLong = d;
}

void Test::setEnterShort (Data *d)
{
  _enterShort = d;
}

void Test::setExitShort (Data *d)
{
  _exitShort = d;
}

void Test::setDate (Data *d)
{
  _date = d;
}

void Test::setOpen (Data *d)
{
  _open = d;
}

void Test::setHigh (Data *d)
{
  _high = d;
}

void Test::setLow (Data *d)
{
  _low = d;
}

void Test::setClose (Data *d)
{
  _close = d;
}

void Test::setEnterComm (double d)
{
  _enterComm = d;
}

void Test::setExitComm (double d)
{
  _exitComm = d;
}

void Test::setEquity (double d)
{
  _startEquity = d;
  _equity = d;
}

void Test::setMaxLossStop (double d)
{
  _maxLossStop = d;
  _maxLossFlag = TRUE;
}

void Test::setTrailingStop (bool d)
{
  _trailingFlag = d;
}

void Test::setVolume (double d)
{
  _volume = d;
}
