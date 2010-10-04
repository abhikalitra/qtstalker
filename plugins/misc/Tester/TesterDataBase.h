/*
 *  Qtstalker stock charter
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

#ifndef TESTER_DATA_BASE_HPP
#define TESTER_DATA_BASE_HPP

#include <QString>
#include <QStringList>

#include "DataBase.h"
#include "Setting.h"

class TesterDataBase : public DataBase
{
  public:
    enum Parm
    {
      _Name,
      _Equity,
      _Position,
      _Period,
      _DateRange,
      _PositionSize,
      _FuturesMode,
      _Commission,
      _CommissionValue,
      _Long,
      _LongBuyPrice,
      _LongSellPrice,
      _Short,
      _ShortBuyPrice,
      _ShortSellPrice,
      _MaximumLossStop,
      _MaximumLossType,
      _MaximumLossValue,
      _MaximumLossExit,
      _ProfitTargetStop,
      _ProfitTargetType,
      _ProfitTargetValue,
      _ProfitTargetExit,
      _TrailingStop,
      _TrailingType,
      _TrailingValue,
      _TrailingExit,
      _BarsStop,
      _BarsStopValue,
      _BarsStopExit,
      _Report,
      _Indicator,
      _PlotNames,
      _EnterLong,
      _ExitLong,
      _EnterShort,
      _ExitShort,
      _IndicatorSettings,
      _Symbols
    };
    
    TesterDataBase ();
    void getRules (QStringList &);
    int getRule (Setting &);
    void setRule (Setting &);
    void deleteRule (QString &);
};

#endif
