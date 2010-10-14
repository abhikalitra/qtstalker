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

#ifndef TESTER_REPORT_HPP
#define TESTER_REPORT_HPP

#include <QStringList>

class TesterReport
{
  public:
    TesterReport ();
    void setName (QString);
    QString & name ();
    void setTrades (QStringList);
    QStringList & trades ();
    void setProfitFactor (double);
    double profitFactor ();
    void setPayoffRatio (double);
    double payoffRatio ();
    void setEquityGain (double);
    double equityGain ();
    void setEquity (double);
    double equity ();
    void setTotalProfit (double);
    double totalProfit ();
    void setProfitableTrades (double);
    double profitableTrades ();
    void setTotalTrades (int);
    int totalTrades ();
    void setWinTrades (int);
    int winTrades ();
    void setLoseTrades (int);
    int loseTrades ();
    void setMaxDrawdown (double);
    double maxDrawdown ();
    void setAvgProfitLoss (double);
    double avgProfitLoss ();
    void setTotalWinTrades (double);
    double totalWinTrades ();
    void setTotalLoseTrades (double);
    double totalLoseTrades ();
    void setAvgBarsHeld (int);
    int avgBarsHeld ();
    void setMinBarsHeld (int);
    int minBarsHeld ();
    void setMaxBarsHeld (int);
    int maxBarsHeld ();
    void setTotalCommissions (double);
    double totalCommissions ();

  private:
    QString _name;
    QStringList _trades;
    double _profitFactor;
    double _payoffRatio;
    double _equityGain;
    double _equity;
    double _totalProfit;
    double _profitableTrades;
    int _totalTrades;
    int _winTrades;
    int _loseTrades;
    double _maxDrawdown;
    double _avgProfitLoss;
    double _totalWinTrades;
    double _totalLoseTrades;
    int _avgBarsHeld;
    int _minBarsHeld;
    int _maxBarsHeld;
    double _totalCommissions;
};

#endif
