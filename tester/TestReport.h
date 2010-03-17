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



#ifndef TESTREPORT_H
#define TESTREPORT_H

#include <QLabel>
#include <QList>
#include <QTreeWidget>
#include <QWidget>

#include "TestTrade.h"

class TestReport : public QWidget
{
  Q_OBJECT

  public:
    TestReport ();
    void createSummary (QList<TestTrade *> &trades, double account, double entryComm, double exitComm);
    void getGrossProfit (QString &);
    void setGrossProfit (QString &);
    void getNetProfit (QString &);
    void setNetProfit (QString &);
    void getMaxDrawDown (QString &);
    void setMaxDrawDown (QString &);
    void getAvgDrawDown (QString &);
    void setAvgDrawDown (QString &);
    void getCommissions (QString &);
    void setCommissions (QString &);
    void getWinLossRatio (QString &);
    void setWinLossRatio (QString &);
    void getTotalTrades (QString &);
    void setTotalTrades (QString &);
    void getPercentProfitable (QString &);
    void setPercentProfitable (QString &);
    void getWinningTrades (QString &);
    void setWinningTrades (QString &);
    void getLosingTrades (QString &);
    void setLosingTrades (QString &);
    void getMaxWinTrade (QString &);
    void setMaxWinTrade (QString &);
    void getMaxLossTrade (QString &);
    void setMaxLossTrade (QString &);
    void getAvgWinTrade (QString &);
    void setAvgWinTrade (QString &);
    void getAvgLossTrade (QString &);
    void setAvgLossTrade (QString &);
    void getMaxWinLong (QString &);
    void setMaxWinLong (QString &);
    void getMaxLossLong (QString &);
    void setMaxLossLong (QString &);
    void getMaxWinShort (QString &);
    void setMaxWinShort (QString &);
    void getMaxLossShort (QString &);
    void setMaxLossShort (QString &);
    void getTradeLog (QString &);
    void setTradeLog (QString &);
    void getBalance (QString &);
    void setBalance (QString &);
    void getTrades (QList<TestTrade *> &);

  private:
    QTreeWidget *tradeLog;
    QLabel *grossProfit;
    QLabel *netProfit;
    QLabel *maxDrawDown;
    QLabel *avgDrawDown;
    QLabel *commissions;
    QLabel *winLossRatio;
    QLabel *totalTrades;
    QLabel *percentProfitable;
    QLabel *winningTrades;
    QLabel *losingTrades;
    QLabel *maxWinTrade;
    QLabel *maxLossTrade;
    QLabel *avgWinTrade;
    QLabel *avgLossTrade;
    QLabel *maxWinLong;
    QLabel *maxLossLong;
    QLabel *maxWinShort;
    QLabel *maxLossShort;
    QLabel *balance;
};

#endif

