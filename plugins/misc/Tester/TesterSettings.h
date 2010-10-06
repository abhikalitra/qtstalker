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

#ifndef TESTER_SETTINGS_HPP
#define TESTER_SETTINGS_HPP

#include <QStringList>
#include "Setting.h"

class TesterSettings
{
  public:
    TesterSettings ();
    void setName (QString);
    QString & name ();
    void setEquity (double);
    double equity ();
    void setPeriod (int);
    int period ();
    void setDateRange (int);
    int dateRange ();
    void setPositionSize (double);
    double positionSize ();
    void setFuturesMode (int);
    int futuresMode ();
    void setCommission (int);
    int commission ();
    void setCommissionValue (double);
    double commissionValue ();
    void setLong (int);
    int getLong ();
    void setLongBuyPrice (int);
    int longBuyPrice ();
    void setLongSellPrice (int);
    int longSellPrice ();
    void setShort (int);
    int getShort ();
    void setShortBuyPrice (int);
    int shortBuyPrice ();
    void setShortSellPrice (int);
    int shortSellPrice ();
    void setMaximumLossStop (int);
    int maximumLossStop ();
    void setMaximumLossType (int);
    int maximumLossType ();
    void setMaximumLossValue (double);
    double maximumLossValue ();
    void setMaximumLossExit (int);
    int maximumLossExit ();
    void setProfitTargetStop (int);
    int profitTargetStop ();
    void setProfitTargetType (int);
    int profitTargetType ();
    void setProfitTargetValue (double);
    double profitTargetValue ();
    void setProfitTargetExit (int);
    int profitTargetExit ();
    void setTrailingStop (int);
    int trailingStop ();
    void setTrailingType (int);
    int trailingType ();
    void setTrailingValue (double);
    double trailingValue ();
    void setTrailingExit (int);
    int trailingExit ();
    void setBarsStop (int);
    int barsStop ();
    void setBarsStopValue (double);
    double barsStopValue ();
    void setBarsStopExit (int);
    int barsStopExit ();
    void setReport (QString);
    QString & report ();
    void setTrades (QStringList);
    QStringList & trades ();
    void setIndicator (QString);
    QString & indicator ();
    void setPlotNames (QStringList);
    QStringList & plotNames ();
    void setEnterLong (QStringList);
    QStringList & enterLong ();
    void setExitLong (QStringList);
    QStringList & exitLong ();
    void setEnterShort (QStringList);
    QStringList & enterShort ();
    void setExitShort (QStringList);
    QStringList & exitShort ();
    void setSymbols (QStringList);
    QStringList & symbols ();
    void setIndicatorSettings (Setting);
    Setting & indicatorSettings ();

  private:
    QString _name;
    double _equity;
    int _period;
    int _dateRange;
    double _positionSize;
    int _futuresMode;
    int _commission;
    double _commissionValue;
    int _long;
    int _longBuyPrice;
    int _longSellPrice;
    int _short;
    int _shortBuyPrice;
    int _shortSellPrice;
    int _maximumLossStop;
    int _maximumLossType;
    double _maximumLossValue;
    int _maximumLossExit;
    int _profitTargetStop;
    int _profitTargetType;
    double _profitTargetValue;
    int _profitTargetExit;
    int _trailingStop;
    int _trailingType;
    double _trailingValue;
    int _trailingExit;
    int _barsStop;
    double _barsStopValue;
    int _barsStopExit;
    QString _report;
    QStringList _trades;
    QString _indicator;
    QStringList _plotNames;
    QStringList _enterLong;
    QStringList _exitLong;
    QStringList _enterShort;
    QStringList _exitShort;
    QStringList _symbols;
    Setting _indicatorSettings;
};

#endif
