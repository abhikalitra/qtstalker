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

#include "TesterThread.h"
#include "QuoteServerRequest.h"
#include "IndicatorPluginFactory.h"
#include "Operator.h"
#include "TesterSignals.h"
#include "TesterReport.h"
#include "TesterReportDataBase.h"

#include <QDebug>

TesterThread::TesterThread (QObject *p, TesterSettings &set) : QThread (p)
{
  _settings = set;
  _stops.setSettings(set);
  _stopFlag = 0;

  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void TesterThread::run ()
{
  _elIndicator.setSettings(_settings.eLSettings());
  _xlIndicator.setSettings(_settings.xLSettings());
  _esIndicator.setSettings(_settings.eSSettings());
  _xsIndicator.setSettings(_settings.xSSettings());

  QList<TesterTrade *> trades;
  _stopFlag = 0;

  QStringList symbols = _settings.symbols();
  
  int symbolLoop = 0;
  for (; symbolLoop < symbols.count(); symbolLoop++)
  {
    if (_stopFlag)
    {
      qDebug() << "TesterThread::run: stopped";
      qDeleteAll(trades);
      emit signalStopped(_settings.name());
      quit();
      return;
    }

    // get bar data
    BarData bars;
    bars.setKey(symbols.at(symbolLoop));
    if (getBars(bars))
      continue;

    IndicatorPlotRules enterLongRules;
    if (enterLongRules.createRules(_elIndicator, _settings.enterLong(), bars))
    {
      qDeleteAll(trades);
      bars.clear();
      quit();
      return;
    }

    IndicatorPlotRules exitLongRules;
    if (exitLongRules.createRules(_xlIndicator, _settings.exitLong(), bars))
    {
      qDeleteAll(trades);
      bars.clear();
      quit();
      return;
    }

    IndicatorPlotRules enterShortRules;
    if (enterShortRules.createRules(_esIndicator, _settings.enterShort(), bars))
    {
      qDeleteAll(trades);
      bars.clear();
      quit();
      return;
    }

    IndicatorPlotRules exitShortRules;
    if (exitShortRules.createRules(_xsIndicator, _settings.exitShort(), bars))
    {
      qDeleteAll(trades);
      bars.clear();
      quit();
      return;
    }
    
    // get enter long indicator data
    QStringList plotNames;
    if (getIndicator(bars, plotNames, _elIndicator, _settings.eLIndicator()))
    {
      bars.clear();
      continue;
    }

    // get exit long indicator data
    if (getIndicator(bars, plotNames, _xlIndicator, _settings.xLIndicator()))
    {
      _elIndicator.clear();
      bars.clear();
      continue;
    }

    // get enter short indicator data
    if (getIndicator(bars, plotNames, _esIndicator, _settings.eSIndicator()))
    {
      _elIndicator.clear();
      _xlIndicator.clear();
      bars.clear();
      continue;
    }

    // get exit short indicator data
    if (getIndicator(bars, plotNames, _xsIndicator, _settings.xSIndicator()))
    {
      _elIndicator.clear();
      _xlIndicator.clear();
      _esIndicator.clear();
      bars.clear();
      continue;
    }

    Status status = _None;
    int barLoop = 0;
    for (; barLoop < bars.count(); barLoop++)
    {
      switch(status)
      {
        case _None:
          if (_settings.getLong())
          {
            if (enterTradeCheck(enterLongRules, barLoop, trades, bars, _elIndicator))
            {
              status = _EnterLong;
              break;
            }
          }
          
          if (_settings.getShort())
          {
            if (enterTradeCheck(enterShortRules, barLoop, trades, bars, _esIndicator))
              status = _EnterShort;
          }
          break;
        case _EnterLong:
          status = _Long;
          if (enterTrade(symbols.at(symbolLoop), trades, bars, barLoop, status))
            status = _None;
          break;
        case _EnterShort:
          status = _Short;
          if (enterTrade(symbols.at(symbolLoop), trades, bars, barLoop, status))
            status = _None;
          break;
        case _Long:
          if (enterTradeCheck(exitLongRules, barLoop, trades, bars, _xlIndicator))
          {
            status = _ExitTrade;
            break;
          }

          if (_stops.checkStops(trades, bars, barLoop))
            status = _ExitStop;
          break;
        case _Short:
          if (enterTradeCheck(exitShortRules, barLoop, trades, bars, _xsIndicator))
          {
            status = _ExitTrade;
            break;
          }
          
          if (_stops.checkStops(trades, bars, barLoop))
            status = _ExitStop;
          break;
        case _ExitTrade:
        {
          TesterTrade *trade = trades.at(trades.count() - 1);
          int signal = TesterSignals::_ExitLong;
          if (trade->type())
            signal = TesterSignals::_ExitShort;
          
          exitTrade(trades, bars, barLoop, signal);
          status = _None;
          break;
        }
        case _ExitStop:
          exitTrade(trades, bars, barLoop, _stops.triggered());
          status = _None;
          break;
        default:
          break;
      }
    }

    // check if we have an open trade
    if (trades.count())
    {
      TesterTrade *t = trades.at(trades.count() - 1);
      if (t->isOpenTrade())
        exitTrade(trades, bars, bars.count() - 1, TesterSignals::_TestEnd);
    }

    _elIndicator.clear();
    _xlIndicator.clear();
    _esIndicator.clear();
    _xsIndicator.clear();
    
    bars.clear();
  }

  getReport(trades);

  _elIndicator.clear();
  _xlIndicator.clear();
  _esIndicator.clear();
  _xsIndicator.clear();
  
  emit signalDone(_settings.name());

  qDeleteAll(trades);
  
  quit();
}

void TesterThread::stop ()
{
  _stopFlag = 1;
}

int TesterThread::getBars (BarData &bd)
{
  QStringList tl;
  tl << "Quotes" << "Date" << bd.getExchange() << bd.getSymbol();
  QString s;
  bd.barLengthText((BarData::BarLength) _settings.period(), s);
  tl << s << "0" << "0" << QString::number(_settings.dateRange());
  QString command = tl.join(",") + "\n";

  QuoteServerRequest qsr;
  if (qsr.run(command))
  {
    qDebug() << "TesterThread::getBars: qsr error";
    return 1;
  }

  bd.setBarLength((BarData::BarLength) _settings.period());
  bd.setBars(qsr.data());

  return 0;
}

int TesterThread::getIndicator (BarData &bd, QStringList &plotNames, Indicator &indicator, QString name)
{
  IndicatorPluginFactory fac;
  IndicatorPlugin *plug = fac.plugin(name);
  if (! plug)
  {
    qDebug() << "TesterThread::getIndicator: no plugin" << name;
    return 1;
  }

  if (plug->getIndicator(indicator, bd))
  {
    qDebug() << "TesterThread::getIndicator: indicator error" << name;
    return 1;
  }

  plug->plotNames(indicator, plotNames);

  return 0;
}

int TesterThread::enterTradeCheck (IndicatorPlotRules &rules, int index, QList<TesterTrade *> &trades,
                                   BarData &bars, Indicator &indicator)
{
  if (! rules.count())
    return 0;
  
  int loop = 0;
  int count = 0;
  for (; loop < rules.count(); loop++)
  {
    IndicatorPlotRule *rule = rules.getRule(loop);
    
    Curve *curve = indicator.line(rule->name());
    if (! curve)
      return 0;

    double value = 0;
    CurveBar *bar = curve->bar(index);
    if (! bar)
      return 0;
    value = bar->data();

    double value2 = 0;
    curve = indicator.line(rule->value());
    if (curve)
    {
      bar = curve->bar(index);
      if (! bar)
        return 0;
      value2 = bar->data();
    }
    else
    {
      bool ok;
      value2 = rule->value().toDouble(&ok);
      if (! ok)
        return 0;
    }

    switch ((Operator::Type) rule->op())
    {
      case Operator::_LessThan:
        if (value < value2)
          count++;
        break;
      case Operator::_LessThanEqual:
        if (value <= value2)
          count++;
        break;
      case Operator::_Equal:
        if (value == value2)
          count++;
        break;
      case Operator::_GreaterThanEqual:
        if (value >= value2)
          count++;
        break;
      case Operator::_GreaterThan:
        if (value > value2)
          count++;
        break;
      default:
        break;
    }
  }

  // update current trade
  if (trades.count())
  {
    TesterTrade *t = trades.at(trades.count() - 1);
    Bar bar = bars.getBar(index);
    double price = bar.getData((Bar::BarField) _settings.longSellPrice());
    if (t->type())
      price = bar.getData((Bar::BarField) _settings.shortSellPrice());
    t->update(price);
  }
  
  if (count == rules.count())
    return 1;

  return 0;
}

int TesterThread::enterTrade (QString symbol, QList<TesterTrade *> &trades, BarData &bars, int index, int status)
{
  TesterTrade *t = new TesterTrade;

  // type 0 == long, 1 == short
  int type = 0;
  if ((Status) status == _Short)
    type = 1;

  Bar bar = bars.getBar(index);

  // enter price
  double price = bar.getData((Bar::BarField) _settings.longBuyPrice());
  if (type)
    price = bar.getData((Bar::BarField) _settings.shortBuyPrice());

  // equity
  double equity = _settings.equity();
  if (trades.count())
  {
    TesterTrade *tt = trades.at(trades.count() - 1);
    equity = tt->equity();
  }

  // figure out volume
  int volume = (equity * _settings.positionSize()) / price;
  if (volume < 1)
  {
//qDebug() << volume << equity << _settings.positionSize() << price;    
    return 1;
  }

  double comm = getCommission(t, 0);
  
  t->enterTrade(symbol, type, equity, bar.date(), price, volume, index, comm);

  trades.append(t);

  return 0;
}

void TesterThread::exitTrade (QList<TesterTrade *> &trades, BarData &bars, int index, int signal)
{
  TesterTrade *t = trades.at(trades.count() - 1);

  Bar bar = bars.getBar(index);
  
  double price = bar.getData((Bar::BarField) _settings.longSellPrice());
  if (t->type())
    price = bar.getData((Bar::BarField) _settings.shortSellPrice());

  double comm = getCommission(t, 1);

  t->exitTrade(bar.date(), price, index, comm, signal);
}

double TesterThread::getCommission (TesterTrade *trade, int flag)
{
//  l << tr("Percent") << tr("$ Per Trade") << tr("$ Per Share");
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

void TesterThread::getReport (QList<TesterTrade *> &trades)
{
  int win = 0;
  int loss = 0;
  double winTotal = 0;
  double lossTotal = 0;
  double profit = 0;
  double equity = _settings.equity();
  double drawDown = 0;
  int maxBars = 0;
  int minBars = 99999999;
  int totalBars = 0;
  double commissions = 0;
  QStringList list;
  int loop = 0;
  for (; loop < trades.count(); loop++)
  {
    TesterTrade *t = trades.at(loop);

    if (t->profit() < 0)
    {
      loss++;
      lossTotal += t->profit();
    }
    else
    {
      win++;
      winTotal += t->profit();
    }

    profit += t->profit();

    equity = t->equity();

    int bars = t->barsHeld();
    totalBars += bars;
    if (bars < minBars)
      minBars = bars;
    if (bars > maxBars)
      maxBars = bars;

    if (t->drawDown() < drawDown)
      drawDown = t->drawDown();

    commissions += t->commissions();

    QString s;
    t->tradeString(s);
    list.append(s);
  }

  TesterReport report;
  report.setName(_settings.name());
  report.setTrades(list);

  if (winTotal == 0 || lossTotal == 0)
    report.setProfitFactor(0);
  else
    report.setProfitFactor((double) winTotal / lossTotal);

  if (winTotal == 0 || lossTotal == 0)
    report.setPayoffRatio(0);
  else
    report.setPayoffRatio((double) ((winTotal / (double) win) / (lossTotal / (double) loss)));
  
  report.setEquityGain((double) ((profit / _settings.equity()) * 100));
  report.setEquity(equity);
  report.setTotalProfit(profit);
  report.setProfitableTrades((double) (((double) win / (double) trades.count()) * 100));
  report.setTotalTrades(trades.count());
  report.setWinTrades(win);
  report.setLoseTrades(loss);
  report.setMaxDrawdown(drawDown);
  report.setAvgProfitLoss((double) (profit / (double) trades.count()));
  report.setTotalWinTrades(winTotal);
  report.setTotalLoseTrades(lossTotal);
  report.setAvgBarsHeld((int) totalBars / trades.count());
  report.setMinBarsHeld(minBars);
  report.setMaxBarsHeld(maxBars);
  report.setTotalCommissions(commissions);

  TesterReportDataBase db;
  db.transaction();
  db.setReport(report);
  db.commit();
}

