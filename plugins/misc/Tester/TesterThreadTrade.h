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

#ifndef TESTER_THREAD_TRADE_HPP
#define TESTER_THREAD_TRADE_HPP

#include <QStringList>
#include <QList>

#include "Indicator.h"
#include "TesterSettings.h"
#include "BarData.h"
#include "TesterTrade.h"
#include "TesterStops.h"
#include "IndicatorPlotRules.h"

class TesterThreadTrade
{
  public:
    enum Status
    {
      _None,
      _Long,
      _Short,
      _EnterLong,
      _EnterShort,
      _ExitTrade,
      _ExitStop
    };
    
    TesterThreadTrade (TesterSettings &);
    ~TesterThreadTrade ();
    int getBars (QString);
    int getIndicator ();
    int getIndicator2 (Indicator &, QString &);
    int getRules ();
    int next (QList<TesterTrade *> &, double &equity);
    int enterTradeCheck (QList<TesterTrade *> &, IndicatorPlotRules &, Indicator &);
    int enterTrade (QList<TesterTrade *> &, QString symbol, double &equity);
    void exitTrade (QList<TesterTrade *> &, int index, int signal, double &equity);
    double getCommission (TesterTrade *, int flag);

  private:
    Indicator _elIndicator;
    Indicator _xlIndicator;
    Indicator _esIndicator;
    Indicator _xsIndicator;
    TesterSettings _settings;
    BarData _bars;
    TesterStops _stops;
    IndicatorPlotRules _enterLongRules;
    IndicatorPlotRules _exitLongRules;
    IndicatorPlotRules _enterShortRules;
    IndicatorPlotRules _exitShortRules;
    int _barLoop;
    Status _status;
    int _currentTrade;
};

#endif
