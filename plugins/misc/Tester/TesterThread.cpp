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
#include "TesterReport.h"
#include "TesterReportDataBase.h"
#include "TesterThreadTrade.h"

#include <QDebug>

TesterThread::TesterThread (QObject *p, TesterSettings &set) : QThread (p)
{
  _settings = set;
  _stopFlag = 0;

  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void TesterThread::run ()
{
  QList<TesterThreadTrade *> threadTrades;
  _stopFlag = 0;
  QStringList symbols = _settings.symbols();
  int loop = 0;
  for (; loop < symbols.count(); loop++)
  {
    TesterThreadTrade *t = new TesterThreadTrade(_settings);
    
    if (t->getBars(symbols.at(loop)))
    {
      delete t;
      continue;
    }
    
    if (t->getIndicator())
    {
      delete t;
      continue;
    }

    if (t->getRules())
    {
      delete t;
      continue;
    }

    threadTrades.append(t);
  }

  QList<TesterTrade *> trades;
  double equity = _settings.equity();

  while (threadTrades.count())
  {
    if (_stopFlag)
    {
      qDebug() << "TesterThread::run: stopped";
      qDeleteAll(trades);
      qDeleteAll(threadTrades);
      emit signalStopped(_settings.name());
      quit();
      return;
    }

    for (loop = 0; loop < threadTrades.count(); loop++)
    {
      if (_stopFlag)
      {
        qDebug() << "TesterThread::run: stopped";
        qDeleteAll(trades);
        qDeleteAll(threadTrades);
        emit signalStopped(_settings.name());
        quit();
        return;
      }

      TesterThreadTrade *t = threadTrades.at(loop);
      if (t->next(trades, equity))
      {
        delete t;
        threadTrades.removeAt(loop);
      }
    }
  }

  getReport(trades, equity);

  emit signalDone(_settings.name());

  qDeleteAll(trades);

  quit();
}

void TesterThread::stop ()
{
  _stopFlag = 1;
}

void TesterThread::getReport (QList<TesterTrade *> &trades, double equity)
{
  int win = 0;
  int loss = 0;
  double winTotal = 0;
  double lossTotal = 0;
  double profit = 0;
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

