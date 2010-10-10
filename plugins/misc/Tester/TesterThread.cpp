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
  _indicator.setSettings(_settings.indicatorSettings());

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
      quit();
      return;
    }

    // get bar data
    BarData bars;
    bars.setKey(symbols.at(symbolLoop));
    if (getBars(bars))
      continue;

    QList<PlotRule *> enterLongRules;
    if (getRules(_settings.enterLong(), bars, enterLongRules))
    {
      qDeleteAll(trades);
      quit();
      return;
    }

    QList<PlotRule *> exitLongRules;
    if (getRules(_settings.exitLong(), bars, exitLongRules))
    {
      qDeleteAll(trades);
      qDeleteAll(enterLongRules);
      quit();
      return;
    }

    QList<PlotRule *> enterShortRules;
    if (getRules(_settings.enterShort(), bars, enterShortRules))
    {
      qDeleteAll(trades);
      qDeleteAll(enterLongRules);
      qDeleteAll(exitLongRules);
      quit();
      return;
    }

    QList<PlotRule *> exitShortRules;
    if (getRules(_settings.exitShort(), bars, exitShortRules))
    {
      qDeleteAll(trades);
      qDeleteAll(enterLongRules);
      qDeleteAll(exitLongRules);
      qDeleteAll(enterShortRules);
      quit();
      return;
    }
    
    // get indicator data
    QStringList plotNames;
    if (getIndicator(bars, plotNames))
    {
      qDeleteAll(enterLongRules);
      qDeleteAll(exitLongRules);
      qDeleteAll(enterShortRules);
      qDeleteAll(exitShortRules);
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
            if (enterTradeCheck(enterLongRules, barLoop, trades, bars))
            {
              status = _EnterLong;
              break;
            }
          }
          
          if (_settings.getShort())
          {
            if (enterTradeCheck(enterShortRules, barLoop, trades, bars))
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
          if (enterTradeCheck(exitLongRules, barLoop, trades, bars))
          {
            status = _ExitTrade;
            break;
          }

          if (_stops.checkStops(trades, bars, barLoop))
            status = _ExitStop;
          break;
        case _Short:
          if (enterTradeCheck(exitShortRules, barLoop, trades, bars))
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

    qDeleteAll(enterLongRules);
    qDeleteAll(exitLongRules);
    qDeleteAll(enterShortRules);
    qDeleteAll(exitShortRules);
  }

  QString report;
  QStringList tradeList;
  getReport(trades, report, tradeList);

  _indicator.clear();
  
  emit signalDone(report, tradeList);

  qDeleteAll(trades);
  
  quit();
}

void TesterThread::stop ()
{
  _stopFlag = 1;
}

int TesterThread::getRules (QStringList &l, BarData &bars, QList<PlotRule *> &rules)
{
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList l2 = l.at(loop).split(",");
    if (l2.count() != 4)
      return 1;

    PlotRule *r = new PlotRule;
    r->enable = l2.at(0).toInt();
    if (! r->enable)
      continue;
    
    r->name = l2.at(1);
    r->op = l2.at(2).toInt();
    r->value = l2.at(3);
    rules.append(r);

    // add any OHLC inputs to indicator for later usage
    bars.getInputFields(l2);
    if (l2.indexOf(r->value) != -1)
    {
      Curve *curve = _indicator.line(r->value);
      if (! curve)
      {
        curve = bars.getInput(bars.getInputType(r->value));
        _indicator.setLine(r->value, curve);
      }
    }
  }

  return 0;
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

int TesterThread::getIndicator (BarData &bd, QStringList &plotNames)
{
  IndicatorPluginFactory fac;
  IndicatorPlugin *plug = fac.plugin(_settings.indicator());
  if (! plug)
  {
    qDebug() << "TesterThread::getIndicator: no plugin" << _settings.indicator();
    return 1;
  }

  if (plug->getIndicator(_indicator, bd))
  {
    qDebug() << "TesterThread::getIndicator: indicator error" << _settings.indicator();
    return 1;
  }

  plug->plotNames(_indicator, plotNames);

  return 0;
}

int TesterThread::enterTradeCheck (QList<PlotRule *> &rules, int index, QList<TesterTrade *> &trades,
                                   BarData &bars)
{
  if (! rules.count())
    return 0;
  
  int loop = 0;
  int count = 0;
  for (; loop < rules.count(); loop++)
  {
    PlotRule *rule = rules.at(loop);
    
    Curve *curve = _indicator.line(rule->name);
    if (! curve)
      return 0;

    double value = 0;
    CurveBar *bar = curve->bar(index);
    if (! bar)
      return 0;
    value = bar->data();

    double value2 = 0;
    curve = _indicator.line(rule->value);
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
      value2 = rule->value.toDouble(&ok);
      if (! ok)
        return 0;
    }

    switch ((Operator::Type) rule->op)
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

void TesterThread::getReport (QList<TesterTrade *> &trades, QString &report, QStringList &list)
{
  report.clear();
  list.clear();

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

  report.append(tr("Profit Factor") + ": \t" + QString::number(winTotal / lossTotal, 'f', 2) + "\n");
  report.append(tr("Payoff Ratio") + ": \t" + QString::number((winTotal / (double) win) / (lossTotal / (double) loss), 'f', 2) + "\n");
  report.append(tr("% Equity Gain") + ": \t" + QString::number((profit / _settings.equity()) * 100, 'f', 2) + "\n");
  report.append(tr("Equity") + ": \t\t" + QString::number(equity, 'f', 2) + "\n");
  report.append(tr("Total Profit") + ": \t" + QString::number(profit, 'f', 2) + "\n");
  report.append(tr("% Profitable Trades") + ": \t" + QString::number(((double) win / (double) trades.count()) * 100, 'f', 2) + "\n");
  report.append(tr("Total Trades") + ": \t" + QString::number(trades.count()) + "\n");
  report.append(tr("Winning Trades") + ": \t" + QString::number(win) + "\n");
  report.append(tr("Losing Trades") + ": \t" + QString::number(loss) + "\n");
  report.append(tr("Maximum Drawdown") + ": \t" + QString::number(drawDown, 'f', 2) + "\n");
  report.append(tr("Avg Profit/Loss") + ": \t" + QString::number(profit / (double) trades.count(), 'f', 2) + "\n");
  report.append(tr("Total Winning Trades") + ": \t" + QString::number(winTotal, 'f', 2) + "\n");
  report.append(tr("Total Losing Trades") + ": \t" + QString::number(lossTotal, 'f', 2) + "\n");
  report.append(tr("Avg Bars Held") + ": \t" + QString::number(totalBars / trades.count(), 'f', 2) + "\n");
  report.append(tr("Min Bars Held") + ": \t" + QString::number(minBars) + "\n");
  report.append(tr("Max Bars Held") + ": \t" + QString::number(maxBars) + "\n");
  report.append(tr("Total Commissions") + ": \t" + QString::number(commissions, 'f', 2) + "\n");
}

