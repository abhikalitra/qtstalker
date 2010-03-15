/*
 *  TestReport
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


#include <QVBoxLayout>
#include <QtDebug>

#include "TestReport.h"

TestReport::TestReport ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(0);
  setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  grid->setColumnStretch(3, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // gross profit
  QLabel *label = new QLabel(tr("Gross Profit"));
  grid->addWidget(label, row, col++);

  grossProfit = new QLabel("0");
  grid->addWidget(grossProfit, row++, col--);

  // net profit
  label = new QLabel(tr("Net Profit"));
  grid->addWidget(label, row, col++);

  netProfit = new QLabel("0");
  grid->addWidget(netProfit, row++, col--);

  // max drawdown
  label = new QLabel(tr("Maximum Drawdown"));
  grid->addWidget(label, row, col++);

  maxDrawDown = new QLabel("0");
  grid->addWidget(maxDrawDown, row++, col--);

  // avg drawdown
  label = new QLabel(tr("Average Drawdown"));
  grid->addWidget(label, row, col++);

  avgDrawDown = new QLabel("0");
  grid->addWidget(avgDrawDown, row++, col--);

  // commissions
  label = new QLabel(tr("Commissions"));
  grid->addWidget(label, row, col++);

  commissions = new QLabel("0");
  grid->addWidget(commissions, row++, col--);

  // win/loss ratio
  label = new QLabel(tr("Win/Loss Ratio"));
  grid->addWidget(label, row, col++);

  winLossRatio = new QLabel("0");
  grid->addWidget(winLossRatio, row++, col--);

  // total trades
  label = new QLabel(tr("Total Trades"));
  grid->addWidget(label, row, col++);

  totalTrades = new QLabel("0");
  grid->addWidget(totalTrades, row++, col--);

  // percent profitable
  label = new QLabel(tr("Percent Profitable"));
  grid->addWidget(label, row, col++);

  percentProfitable = new QLabel("0");
  grid->addWidget(percentProfitable, row++, col--);

  // winning trades
  label = new QLabel(tr("Winning Trades"));
  grid->addWidget(label, row, col++);

  winningTrades = new QLabel("0");
  grid->addWidget(winningTrades, row++, col--);

  // losing trades
  label = new QLabel(tr("Losing Trades"));
  grid->addWidget(label, row, col++);

  losingTrades = new QLabel("0");
  grid->addWidget(losingTrades, row++, col--);

  col = 2;
  row = 0;

  // maxWinTrade
  label = new QLabel(tr("Largest Winning Trade"));
  grid->addWidget(label, row, col++);

  maxWinTrade = new QLabel("0");
  grid->addWidget(maxWinTrade, row++, col--);

  // maxLosingTrade
  label = new QLabel(tr("Largest Losing Trade"));
  grid->addWidget(label, row, col++);

  maxLossTrade = new QLabel("0");
  grid->addWidget(maxLossTrade, row++, col--);

  // avgWinTrade
  label = new QLabel(tr("Average Winning Trade"));
  grid->addWidget(label, row, col++);

  avgWinTrade = new QLabel("0");
  grid->addWidget(avgWinTrade, row++, col--);

  // avgLosingTrade
  label = new QLabel(tr("Average Losing Trade"));
  grid->addWidget(label, row, col++);

  avgLossTrade = new QLabel("0");
  grid->addWidget(avgLossTrade, row++, col--);

  // maxWinLong
  label = new QLabel(tr("Largest Winning Long"));
  grid->addWidget(label, row, col++);

  maxWinLong = new QLabel("0");
  grid->addWidget(maxWinLong, row++, col--);

  // maxLosingLong
  label = new QLabel(tr("Largest Losing Long"));
  grid->addWidget(label, row, col++);

  maxLossLong = new QLabel("0");
  grid->addWidget(maxLossLong, row++, col--);

  // maxWinShort
  label = new QLabel(tr("Largest Winning Short"));
  grid->addWidget(label, row, col++);

  maxWinShort = new QLabel("0");
  grid->addWidget(maxWinShort, row++, col--);

  // maxLosingShort
  label = new QLabel(tr("Largest Losing Short"));
  grid->addWidget(label, row, col++);

  maxLossShort = new QLabel("0");
  grid->addWidget(maxLossShort, row++, col--);

  // balance
  label = new QLabel(tr("Account Balance"));
  grid->addWidget(label, row, col++);

  balance = new QLabel("0");
  grid->addWidget(balance, row++, col--);

  tradeLog = new QTreeWidget;
  tradeLog->setColumnCount(10);
  tradeLog->setRootIsDecorated(FALSE);
  QStringList l;
  l << tr("Enter Date") << tr("Enter Signal") << tr("Volume") << tr("Enter Price");
  l << tr("Exit Date") << tr("Exit Signal") << tr("Exit Price") << tr("Profit");
  l << "High" << "Low";
  tradeLog->setHeaderLabels(l);
  tradeLog->setColumnHidden(8, TRUE);
  tradeLog->setColumnHidden(9, TRUE);
  vbox->addWidget(tradeLog);
}

void TestReport::createSummary (QList<TestTrade *> &trades, double account, double entryComm,
				double exitComm)
{
  double totalDraw = 0;
  double tmaxDrawDown = 0;
  int longTrades = 0;
  int winTrades = 0;
  int lossTrades = 0;
  int shortTrades = 0;
  double totalLossTrades = 0;
  int numDrawDown = 0;
  double gross = 0;
  double tmaxWinTrade = 0;
  double tmaxLossTrade = 0;
  double tmaxWinLong = 0;
  double tmaxLossLong = 0;
  double tmaxWinShort = 0;
  double tmaxLossShort = 0;
  double commission = 0;
  double net = 0;
  double tavgDrawDown = 0;
  double tavgWinTrade = 0;
  double tavgLossTrade = 0;
  double twinLossRatio = 0;
  double profitable = 0;
  double tbalance = account;
  double tprofit = 0;
  int loop;
  QString s;

  tradeLog->clear();
  tradeLog->setSortingEnabled(FALSE);

  for (loop = 0; loop < trades.count(); loop++)
  {
    TestTrade *trade = trades.at(loop);

    tprofit = tprofit + trade->getProfit();

    if (trade->getDrawDown() < 0)
    {
      totalDraw = totalDraw + trade->getDrawDown();
      numDrawDown++;
    }

    if (trade->getDrawDown() < tmaxDrawDown)
      tmaxDrawDown = trade->getDrawDown();

    if (trade->getType() == 0)
      longTrades++;
    else
      shortTrades++;

    if (trade->getProfit() >= 0)
    {
      winTrades++;

      gross = gross + trade->getProfit();

      if (trade->getProfit() > tmaxWinTrade)
	tmaxWinTrade = trade->getProfit();

      if (trade->getType() == 0)
      {
	if (trade->getProfit() > tmaxWinLong)
          tmaxWinLong = trade->getProfit();
      }
      else
      {
	if (trade->getProfit() > tmaxWinShort)
	  tmaxWinShort = trade->getProfit();
      }
    }
    else
    {
      lossTrades++;

      totalLossTrades = totalLossTrades + trade->getProfit();

      if (trade->getProfit() < tmaxLossTrade)
	tmaxLossTrade = trade->getProfit();

      if (trade->getType() == 0)
      {
	if (trade->getProfit() < tmaxLossLong)
	  tmaxLossLong = trade->getProfit();
      }
      else
      {
	if (trade->getProfit() < tmaxLossShort)
	  tmaxLossShort = trade->getProfit();
      }
    }

    QStringList l;
    trade->getLogMessage(l);

    int loop2;
    QTreeWidgetItem *item = new QTreeWidgetItem(tradeLog);
    for (loop2 = 0; loop2 < l.count(); loop2++)
      item->setText(loop2, l[loop2]);
  }

  for (loop = 0; loop < tradeLog->columnCount(); loop++)
    tradeLog->resizeColumnToContents(loop);
  tradeLog->setSortingEnabled(TRUE);

  // commissions
  commission = (entryComm + exitComm) * trades.count();

  // net
  net = tprofit - commission;

  tbalance = tbalance + net;

  // avg draw down
  if (numDrawDown)
    tavgDrawDown = totalDraw / (double) numDrawDown;

  // avg win trade
  if (gross && winTrades)
    tavgWinTrade = gross / (double) winTrades;

  // avg loss trade
  if (totalLossTrades && lossTrades)
    tavgLossTrade = totalLossTrades / (double) lossTrades;

  // avg win / avg loss ratio
  if (tavgWinTrade && tavgLossTrade)
  {
    twinLossRatio = tavgWinTrade / tavgLossTrade;
    if (twinLossRatio < 0)
      twinLossRatio = twinLossRatio * -1;
  }

  // % profitable
  if (winTrades && trades.count())
    profitable = ((double) winTrades / (double) trades.count()) * 100;

  grossProfit->setNum(gross);
  netProfit->setNum(net);
  maxDrawDown->setNum(tmaxDrawDown);
  avgDrawDown->setNum(tavgDrawDown);
  commissions->setNum(commission);
  winLossRatio->setNum(twinLossRatio);
  totalTrades->setNum(trades.count());
  percentProfitable->setNum(profitable);
  winningTrades->setNum(winTrades);
  losingTrades->setNum(lossTrades);
  maxWinTrade->setNum(tmaxWinTrade);
  maxLossTrade->setNum(tmaxLossTrade);
  avgWinTrade->setNum(tavgWinTrade);
  avgLossTrade->setNum(tavgLossTrade);
  maxWinLong->setNum(tmaxWinLong);
  maxLossLong->setNum(tmaxLossLong);
  maxWinShort->setNum(tmaxWinShort);
  maxLossShort->setNum(tmaxLossShort);
  balance->setNum(tbalance);
}

void TestReport::getGrossProfit (QString &d)
{
  d = grossProfit->text();
}

void TestReport::setGrossProfit (QString &d)
{
  grossProfit->setText(d);
}

void TestReport::getNetProfit (QString &d)
{
  d = netProfit->text();
}

void TestReport::setNetProfit (QString &d)
{
  netProfit->setText(d);
}

void TestReport::getMaxDrawDown (QString &d)
{
  d = maxDrawDown->text();
}

void TestReport::setMaxDrawDown (QString &d)
{
  maxDrawDown->setText(d);
}

void TestReport::getAvgDrawDown (QString &d)
{
  d = avgDrawDown->text();
}

void TestReport::setAvgDrawDown (QString &d)
{
  avgDrawDown->setText(d);
}

void TestReport::getCommissions (QString &d)
{
  d = commissions->text();
}

void TestReport::setCommissions (QString &d)
{
  commissions->setText(d);
}

void TestReport::getWinLossRatio (QString &d)
{
  d = winLossRatio->text();
}

void TestReport::setWinLossRatio (QString &d)
{
  winLossRatio->setText(d);
}

void TestReport::getTotalTrades (QString &d)
{
  d = totalTrades->text();
}

void TestReport::setTotalTrades (QString &d)
{
  totalTrades->setText(d);
}

void TestReport::getPercentProfitable (QString &d)
{
  d = percentProfitable->text();
}

void TestReport::setPercentProfitable (QString &d)
{
  percentProfitable->setText(d);
}

void TestReport::getWinningTrades (QString &d)
{
  d = winningTrades->text();
}

void TestReport::setWinningTrades (QString &d)
{
  winningTrades->setText(d);
}

void TestReport::getLosingTrades (QString &d)
{
  d = losingTrades->text();
}

void TestReport::setLosingTrades (QString &d)
{
  losingTrades->setText(d);
}

void TestReport::getMaxWinTrade (QString &d)
{
  d = maxWinTrade->text();
}

void TestReport::setMaxWinTrade (QString &d)
{
  maxWinTrade->setText(d);
}

void TestReport::getMaxLossTrade (QString &d)
{
  d = maxLossTrade->text();
}

void TestReport::setMaxLossTrade (QString &d)
{
  maxLossTrade->setText(d);
}

void TestReport::getAvgWinTrade (QString &d)
{
  d = avgWinTrade->text();
}

void TestReport::setAvgWinTrade (QString &d)
{
  avgWinTrade->setText(d);
}

void TestReport::getAvgLossTrade (QString &d)
{
  d = avgLossTrade->text();
}

void TestReport::setAvgLossTrade (QString &d)
{
  avgLossTrade->setText(d);
}

void TestReport::getMaxWinLong (QString &d)
{
  d = maxWinLong->text();
}

void TestReport::setMaxWinLong (QString &d)
{
  maxWinLong->setText(d);
}

void TestReport::getMaxLossLong (QString &d)
{
  d = maxLossLong->text();
}

void TestReport::setMaxLossLong (QString &d)
{
  maxLossLong->setText(d);
}

void TestReport::getMaxWinShort (QString &d)
{
  d = maxWinShort->text();
}

void TestReport::setMaxWinShort (QString &d)
{
  maxWinShort->setText(d);
}

void TestReport::getMaxLossShort (QString &d)
{
  d = maxLossShort->text();
}

void TestReport::setMaxLossShort (QString &d)
{
  maxLossShort->setText(d);
}

void TestReport::getTradeLog (QString &d)
{
  QStringList l;
  int loop;
  for (loop = 0; loop < tradeLog->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = tradeLog->topLevelItem(loop);
    if (! item)
      continue;

    int loop2;
    QStringList l2;
    for (loop2 = 0; loop2 < item->columnCount(); loop2++)
      l2.append(item->text(loop2));

    l.append(l2.join(","));
  }

  if (l.count())
    d = l.join("\n");
  else
    d = " ";
}

void TestReport::setTradeLog (QString &d)
{
  tradeLog->clear();
  tradeLog->setSortingEnabled(FALSE);

  QStringList l = d.split("\n");
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    QStringList l2 = l[loop].split(",");
    int loop2;
    QTreeWidgetItem *item = new QTreeWidgetItem(tradeLog);
    for (loop2 = 0; loop2 < l2.count(); loop2++)
      item->setText(loop2, l2[loop2]);
  }

  for (loop = 0; loop < tradeLog->columnCount(); loop++)
    tradeLog->resizeColumnToContents(loop);

  tradeLog->setSortingEnabled(TRUE);
}

void TestReport::getBalance (QString &d)
{
  d = balance->text();
}

void TestReport::setBalance (QString &d)
{
  balance->setText(d);
}

