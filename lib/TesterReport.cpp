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
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(5);
  setLayout(vbox);

  tradeList = new QTableWidget(0, 9);
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
  results = new QTextEdit;
  results->setReadOnly(TRUE);
  vbox->addWidget(results);
}

void TesterReport::getParms (TesterRule &rule)
{
  int loop;
  QStringList l;
  for (loop = 0; loop < (int) tradeList->rowCount(); loop++)
  {
    int loop2;
    QStringList l2;
    for (loop2 = 0; loop2 < 9; loop2++)
      l2.append(tradeList->item(loop, loop2)->text());

    l.append(l2.join(","));
  }

  QString s = l.join("|");
  rule.setTrades(s);

  s = results->toPlainText();
  rule.setSummary(s);
}

void TesterReport::setParms (TesterRule &rule)
{
  clear();

  QString s;
  rule.getTrades(s);

  QStringList l = s.split("|");
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    QStringList l2 = l[loop].split(",");
    int loop2;
    for (loop2 = 0; loop2 < l2.count(); loop2++)
    {
      QTableWidgetItem *item = new QTableWidgetItem;
      tradeList->setItem(tradeList->rowCount() - 1, loop2, item);
      tradeList->resizeColumnToContents(loop2);
    }
  }

  rule.getSummary(s);
  results->append(s);
}

void TesterReport::addTrade (QString &s, TradeItem *trade)
{
  QStringList l = s.split(",");

  if (! l[0].compare("S"))
    trade->setTradePosition(TradeItem::Short);

  QDateTime dt = QDateTime::fromString(l[1]);
  if (! dt.isValid())
  {
    qDebug("TesterReport::addTrade:bad entry date");
    return;
  }
  trade->setEnterDate(dt);

  trade->setEnterPrice(l[2].toDouble());

  dt = QDateTime::fromString(l[3]);
  if (! dt.isValid())
  {
    qDebug("TesterReport::addTrade:bad exit date");
    return;
  }
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
    tradeList->resizeColumnToContents(0);

    trade->getEnterDateString(ts);
    twi = new QTableWidgetItem(ts);
    tradeList->setItem(tradeList->rowCount() - 1, 1, twi);
    tradeList->resizeColumnToContents(1);

    twi = new QTableWidgetItem(QString::number(trade->getEnterPrice()));
    tradeList->setItem(tradeList->rowCount() - 1, 2, twi);
    tradeList->resizeColumnToContents(2);

    trade->getExitDateString(ts);
    twi = new QTableWidgetItem(ts);
    tradeList->setItem(tradeList->rowCount() - 1, 3, twi);
    tradeList->resizeColumnToContents(3);

    twi = new QTableWidgetItem(QString::number(trade->getExitPrice()));
    tradeList->setItem(tradeList->rowCount() - 1, 4, twi);
    tradeList->resizeColumnToContents(4);

    trade->getExitSignalString(ts);
    twi = new QTableWidgetItem(ts);
    tradeList->setItem(tradeList->rowCount() - 1, 5, twi);
    tradeList->resizeColumnToContents(5);

    twi = new QTableWidgetItem(QString::number(trade->getProfit()));
    tradeList->setItem(tradeList->rowCount() - 1, 6, twi);
    tradeList->resizeColumnToContents(6);

    twi = new QTableWidgetItem(QString::number(trade->getBalance()));
    tradeList->setItem(tradeList->rowCount() - 1, 7, twi);
    tradeList->resizeColumnToContents(7);

    twi = new QTableWidgetItem(QString::number(trade->getVolume()));
    tradeList->setItem(tradeList->rowCount() - 1, 8, twi);
    tradeList->resizeColumnToContents(8);
  }

  // main summary
  results->append(tr("Balance") + " = " + QString::number(balance));
  results->append(tr("Net Profit") + " = " + QString::number(balance - account));
  results->append(tr("Net Percentage") + " = " + QString::number(((balance - account) / account) * 100));
  results->append(tr("Investment") + " = " + QString::number(account));
  results->append(tr("Commission") + " = " + QString::number(commission));
  results->append(tr("Drawdown") + " = " + QString::number(accountDrawdown - account));
  results->append(tr("Trades") + " = " + QString::number(longTrades + shortTrades));
  results->append(tr("Long Trades") + " = " + QString::number(longTrades));
  results->append(tr("Short Trades") + " = " + QString::number(shortTrades));

  // win summary
  results->append(tr("Winning Trades") + " = " + QString::number(winLongTrades + winShortTrades));
  results->append(tr("Total Winning Trades") + " = " + QString::number(totalWinLongTrades + totalWinShortTrades));
  results->append(tr("Average Win") + " = " + QString::number((totalWinLongTrades + totalWinShortTrades) / (winLongTrades + winShortTrades)));
  results->append(tr("Largest Win") + " = " + QString::number(largestWin));
  results->append(tr("Winning Long Trades") + " = " + QString::number(winLongTrades));
  results->append(tr("Winning Short Trades") + " = " + QString::number(winShortTrades));

  // lose summary
  results->append(tr("Losing Trades") + " = " + QString::number(loseLongTrades + loseShortTrades));
  results->append(tr("Total Losing Trades") + " = " + QString::number(totalLoseLongTrades + totalLoseShortTrades));
  results->append(tr("Average Loss") + " = " + QString::number((totalLoseLongTrades + totalLoseShortTrades) / (loseLongTrades + loseShortTrades)));
  results->append(tr("Largest Loss") + " = " + QString::number(largestLose));
  results->append(tr("Losing Long Trades") + " = " + QString::number(loseLongTrades));
  results->append(tr("Losing Short Trades") + " = " + QString::number(loseShortTrades));
}

void TesterReport::clear ()
{
  while (tradeList->rowCount())
    tradeList->removeRow(0);

  results->clear();
}

