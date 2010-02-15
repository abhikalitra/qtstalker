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

#ifndef TEST_HPP
#define TEST_HPP

#include <QString>

class Test
{
  public:
    Test ();
    void getName (QString &);
    void setName (QString &);
    void getScript (QString &);
    void setScript (QString &);
    void getSymbol (QString &);
    void setSymbol (QString &);
    int getEnterField ();
    void setEnterField (int);
    int getExitField ();
    void setExitField (int);
    int getBars ();
    void setBars (int);
    int getBarLength ();
    void setBarLength (int);
    double getEntryComm ();
    void setEntryComm (double);
    double getExitComm ();
    void setExitComm (double);
    int getDelay ();
    void setDelay (int);
    double getAccount ();
    void setAccount (double);
    double getVolumePercentage ();
    void setVolumePercentage (double);

    // summary stuff
    void getTradeLog (QString &);
    void setTradeLog (QString &);
    double getCommissions ();
    void setCommissions (double);
    double getGrossProfit ();
    void setGrossProfit (double);
    double getNetProfit ();
    void setNetProfit (double);
    double getTotalComm ();
    void setTotalComm (double);
    double getMaxDrawDown ();
    void setMaxDrawDown (double);
    double getAvgDrawDown ();
    void setAvgDrawDown (double);
    double getWinLossRatio ();
    void setWinLossRatio (double);
    int getWinTrades ();
    void setWinTrades (int);
    int getLossTrades ();
    void setLossTrades (int);
    int getTrades ();
    void setTrades (int);
    double getProfitable ();
    void setProfitable (double);
    double getMaxWinTrade ();
    void setMaxWinTrade (double);
    double getMaxLossTrade ();
    void setMaxLossTrade (double);
    double getAvgWinTrade ();
    void setAvgWinTrade (double);
    double getAvgLossTrade ();
    void setAvgLossTrade (double);
    double getMaxWinLong ();
    void setMaxWinLong (double);
    double getMaxLossLong ();
    void setMaxLossLong (double);
    double getMaxWinShort ();
    void setMaxWinShort (double);
    double getMaxLossShort ();
    void setMaxLossShort (double);
    double getBalance ();
    void setBalance (double);


  protected:
    QString name;
    QString script;
    QString symbol;
    int enterField;
    int exitField;
    int bars;
    int barLength;
    double entryComm;
    double exitComm;
    int delay;
    double account;
    double volumePercentage;

    // summary data
    QString tradeLog;
    double commissions;
    double grossProfit;
    double netProfit;
    double totalComm;
    double maxDrawDown;
    double avgDrawDown;
    double winLossRatio;
    int winTrades;
    int lossTrades;
    int trades;
    double profitable;
    double maxWinTrade;
    double maxLossTrade;
    double avgWinTrade;
    double avgLossTrade;
    double maxWinLong;
    double maxLossLong;
    double maxWinShort;
    double maxLossShort;
    double balance;
};

#endif

