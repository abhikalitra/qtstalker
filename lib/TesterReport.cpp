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

#include "TesterReport.h"
#include <QLayout>
#include <QGroupBox>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>


TesterReport::TesterReport (QWidget *p) : QWidget (p)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  tradeList = new QTableWidget(0, 9, this);
//  tradeList->setSelectionMode(QTableWidget::Single);
//  tradeList->setSorting(FALSE);
  QStringList tl;
  tl << tr("Type") << tr("Entry") << tr("Entry Price") << tr("Exit") << tr("Exit Price");
  tl << tr("Signal") << tr("Profit") << tr("Account") << tr("Vol");
  tradeList->setHorizontalHeaderLabels(tl);
  vbox->addWidget(tradeList);

//  int loop;
//  for (loop = 0; loop < 9; loop++)
//    tradeList->setColumnReadOnly(loop, TRUE);
    
  // test summary
  
  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->setSpacing(5);
  vbox->addLayout(hbox);

  QGridLayout *grid = new QGridLayout(this);
  grid->setSpacing(5);
  hbox->addLayout(grid);

  QLabel *label = new QLabel(tr("Test Summary"), this);
  grid->addWidget(label, 0, 0);
  
  label = new QLabel(tr("Account Balance "), this);
  grid->addWidget(label, 1, 0);
  summaryBalance = new QLabel(" ", this);
  grid->addWidget(summaryBalance, 1, 1);
  
  label = new QLabel(tr("Net Profit "), this);
  grid->addWidget(label, 2, 0);
  summaryNetProfit = new QLabel(" ", this);
  grid->addWidget(summaryNetProfit, 2, 1);

  label = new QLabel(tr("Net Profit % "), this);
  grid->addWidget(label, 3, 0);
  summaryNetPercentage = new QLabel(" ", this);
  grid->addWidget(summaryNetPercentage, 3, 1);

  label = new QLabel(tr("Initial Investment "), this);
  grid->addWidget(label, 4, 0);
  summaryInvestment = new QLabel(" ", this);
  grid->addWidget(summaryInvestment, 4, 1);

  label = new QLabel(tr("Commissions "), this);
  grid->addWidget(label, 5, 0);
  summaryCommission = new QLabel(" ", this);
  grid->addWidget(summaryCommission, 5, 1);

  label = new QLabel(tr("Largest Drawdown "), this);
  grid->addWidget(label, 6, 0);
  summaryDrawdown = new QLabel(" ", this);
  grid->addWidget(summaryDrawdown, 6, 1);

  label = new QLabel(tr("Trades "), this);
  grid->addWidget(label, 7, 0);
  summaryTrades = new QLabel(" ", this);
  grid->addWidget(summaryTrades, 7, 1);

  label = new QLabel(tr("Long Trades "), this);
  grid->addWidget(label, 8, 0);
  summaryLongTrades = new QLabel(" ", this);
  grid->addWidget(summaryLongTrades, 8, 1);

  label = new QLabel(tr("Short Trades "), this);
  grid->addWidget(label, 9, 0);
  summaryShortTrades = new QLabel(" ", this);
  grid->addWidget(summaryShortTrades, 9, 1);

  grid->setRowStretch(grid->rowCount(), 1);
  
  // win summary

  grid = new QGridLayout(this);
  grid->setSpacing(5);
  hbox->addLayout(grid);
  
  label = new QLabel(tr("Win Summary"), this);
  grid->addWidget(label, 0, 0);

  label = new QLabel(tr("Trades "), this);
  grid->addWidget(label, 1, 0);
  summaryWinTrades = new QLabel(" ", this);
  grid->addWidget(summaryWinTrades, 1, 1);

  label = new QLabel(tr("Profit "), this);
  grid->addWidget(label, 2, 0);
  summaryTotalWinTrades = new QLabel(" ", this);
  grid->addWidget(summaryTotalWinTrades, 2, 1);

  label = new QLabel(tr("Average "), this);
  grid->addWidget(label, 3, 0);
  summaryAverageWin = new QLabel(" ", this);
  grid->addWidget(summaryAverageWin, 3, 1);

  label = new QLabel(tr("Largest "), this);
  grid->addWidget(label, 4, 0);
  summaryLargestWin = new QLabel(" ", this);
  grid->addWidget(summaryLargestWin, 4, 1);

  label = new QLabel(tr("Long Trades "), this);
  grid->addWidget(label, 5, 0);
  summaryWinLongTrades = new QLabel(" ", this);
  grid->addWidget(summaryWinLongTrades, 5, 1);

  label = new QLabel(tr("Short Trades "), this);
  grid->addWidget(label, 6, 0);
  summaryWinShortTrades = new QLabel(" ", this);
  grid->addWidget(summaryWinShortTrades, 6, 1);

  grid->setRowStretch(grid->rowCount(), 1);

  // lose summary

  grid = new QGridLayout(this);
  grid->setSpacing(5);
  hbox->addLayout(grid);

  label = new QLabel(tr("Loss Summary"), this);
  grid->addWidget(label, 0, 0);

  label = new QLabel(tr("Trades "), this);
  grid->addWidget(label, 1, 0);
  summaryLoseTrades = new QLabel(" ", this);
  grid->addWidget(summaryLoseTrades, 1, 1);

  label = new QLabel(tr("Profit "), this);
  grid->addWidget(label, 2, 0);
  summaryTotalLoseTrades = new QLabel(" ", this);
  grid->addWidget(summaryTotalLoseTrades, 2, 1);

  label = new QLabel(tr("Average "), this);
  grid->addWidget(label, 3, 0);
  summaryAverageLose = new QLabel(" ", this);
  grid->addWidget(summaryAverageLose, 3, 1);

  label = new QLabel(tr("Largest "), this);
  grid->addWidget(label, 4, 0);
  summaryLargestLose = new QLabel(" ", this);
  grid->addWidget(summaryLargestLose, 4, 1);

  label = new QLabel(tr("Long Trades "), this);
  grid->addWidget(label, 5, 0);
  summaryLoseLongTrades = new QLabel(" ", this);
  grid->addWidget(summaryLoseLongTrades, 5, 1);

  label = new QLabel(tr("Short Trades "), this);
  grid->addWidget(label, 6, 0);
  summaryLoseShortTrades = new QLabel(" ", this);
  grid->addWidget(summaryLoseShortTrades, 6, 1);

  grid->setRowStretch(grid->rowCount(), 1);
}

TesterReport::~TesterReport ()
{
}

void TesterReport::getSummary (QStringList &rl)
{
  rl.clear();

  int loop;
  for (loop = 0; loop < (int) tradeList->rowCount(); loop++)
  {
    QStringList l;
    int loop2;
    for (loop2 = 0; loop2 < 9; loop2++)
      l.append(tradeList->item(loop, loop2)->text());

    rl.append(l.join(","));
  }
}

void TesterReport::addTrade (QString &s, TradeItem *trade)
{
  QStringList l = s.split(",");

  if (! l[0].compare("S"))
    trade->setTradePosition(TradeItem::Short);

  Bar bar;
//  if (bar.setDate(l[1]))
//  {
//    qDebug("TesterReport::addTrade:bad entry date");
//    return;
//  }
  QDateTime dt;
  bar.getDate(dt);
  trade->setEnterDate(dt);

  trade->setEnterPrice(l[2].toDouble());

//  if (bar.setDate(l[3]))
//  {
//    qDebug("TesterReport::addTrade:bad exit date");
//    return;
//  }
  bar.getDate(dt);
  trade->setExitDate(dt);

  trade->setExitPrice(l[4].toDouble());

  trade->setExitSignal(l[5]);

  trade->setVolume(l[8].toInt());
}

void TesterReport::createSummary (QList<TradeItem*> &trades, double account)
{
  int shortTrades = 0;
  int longTrades = 0;
  int winLongTrades = 0;
  int loseLongTrades = 0;
  int winShortTrades = 0;
  int loseShortTrades = 0;
  double totalWinLongTrades = 0;
  double totalLoseLongTrades = 0;
  double totalWinShortTrades = 0;
  double totalLoseShortTrades = 0;
  double largestWin = 0;
  double largestLose = 0;
  double accountDrawdown = account;
  double commission = 0;
  double balance = account;

  int loop;
  for (loop = 0; loop < (int) trades.count(); loop++)
  {
    TradeItem *trade = trades.at(loop);

    // get long/short trades
    if (trade->getTradePosition() == TradeItem::Long)
    {
      longTrades++;

      if (trade->getProfit() < 0)
      {
        loseLongTrades++;
	totalLoseLongTrades = totalLoseLongTrades + trade->getProfit();

	if (trade->getProfit() < largestLose)
	  largestLose = trade->getProfit();
      }
      else
      {
        winLongTrades++;
	totalWinLongTrades = totalWinLongTrades + trade->getProfit();

	if (trade->getProfit() > largestWin)
	  largestWin = trade->getProfit();
      }
    }
    else
    {
      shortTrades++;

      if (trade->getProfit() < 0)
      {
        loseShortTrades++;
      	totalLoseShortTrades = totalLoseShortTrades + trade->getProfit();

	if (trade->getProfit() < largestLose)
	  largestLose = trade->getProfit();
      }
      else
      {
        winShortTrades++;
      	totalWinShortTrades = totalWinShortTrades + trade->getProfit();

	if (trade->getProfit() > largestWin)
	  largestWin = trade->getProfit();
      }
    }

    commission = commission + trade->getEntryCom() + trade->getExitCom();
    balance = trade->getBalance();

    if (trade->getBalance() < accountDrawdown)
      accountDrawdown = trade->getBalance();

    tradeList->setRowCount(tradeList->rowCount() + 1);
    QString ts;
    trade->getTradePositionString(ts);
    QTableWidgetItem *twi = new QTableWidgetItem(ts);
    tradeList->setItem(tradeList->rowCount() - 1, 0, twi);
    trade->getEnterDateString(ts);
    twi = new QTableWidgetItem(ts);
    tradeList->setItem(tradeList->rowCount() - 1, 1, twi);

    twi = new QTableWidgetItem(QString::number(trade->getEnterPrice()));
    tradeList->setItem(tradeList->rowCount() - 1, 2, twi);

    trade->getExitDateString(ts);

    twi = new QTableWidgetItem(ts);
    tradeList->setItem(tradeList->rowCount() - 1, 3, twi);

    twi = new QTableWidgetItem(QString::number(trade->getExitPrice()));
    tradeList->setItem(tradeList->rowCount() - 1, 4, twi);

    trade->getExitSignalString(ts);

    twi = new QTableWidgetItem(ts);
    tradeList->setItem(tradeList->rowCount() - 1, 5, twi);

    twi = new QTableWidgetItem(QString::number(trade->getProfit()));
    tradeList->setItem(tradeList->rowCount() - 1, 6, twi);

    twi = new QTableWidgetItem(QString::number(trade->getBalance()));
    tradeList->setItem(tradeList->rowCount() - 1, 7, twi);

    twi = new QTableWidgetItem(QString::number(trade->getVolume()));
    tradeList->setItem(tradeList->rowCount() - 1, 8, twi);
  }

  // main summary
  summaryBalance->setNum(balance);
  summaryNetProfit->setNum(balance - account);
  summaryNetPercentage->setNum(((balance - account) / account) * 100);
  summaryInvestment->setNum(account);
  summaryCommission->setNum(commission);
  summaryDrawdown->setNum(accountDrawdown - account);
  summaryTrades->setNum(longTrades + shortTrades);
  summaryLongTrades->setNum(longTrades);
  summaryShortTrades->setNum(shortTrades);

  // win summary
  summaryWinTrades->setNum(winLongTrades + winShortTrades);
  summaryTotalWinTrades->setNum(totalWinLongTrades + totalWinShortTrades);
  summaryAverageWin->setNum((totalWinLongTrades + totalWinShortTrades) / (winLongTrades + winShortTrades));
  summaryLargestWin->setNum(largestWin);
  summaryWinLongTrades->setNum(winLongTrades);
  summaryWinShortTrades->setNum(winShortTrades);

  // lose summary
  summaryLoseTrades->setNum(loseLongTrades + loseShortTrades);
  summaryTotalLoseTrades->setNum(totalLoseLongTrades + totalLoseShortTrades);
  summaryAverageLose->setNum((totalLoseLongTrades + totalLoseShortTrades) / (loseLongTrades + loseShortTrades));
  summaryLargestLose->setNum(largestLose);
  summaryLoseLongTrades->setNum(loseLongTrades);
  summaryLoseShortTrades->setNum(loseShortTrades);
}

void TesterReport::clear ()
{
  while (tradeList->rowCount())
    tradeList->removeRow(0);
}

