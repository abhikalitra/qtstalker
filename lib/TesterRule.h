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

#ifndef TESTERRULE_HPP
#define TESTERRULE_HPP


#include <QHash>
#include <QString>
#include <QStringList>




#include "Indicator.h"




class TesterRule
{
  public:
    enum TesterParm
    {
      EnterLong,
      ExitLong,
      EnterShort,
      ExitShort,
      CustomLongStop,
      CustomShortStop,
      CustomLongCheck,
      CustomShortCheck,
      MaxLossCheck,
      MaxLossLong,
      MaxLossShort,
      MaxLoss,
      ProfitCheck,
      ProfitLong,
      ProfitShort,
      Profit,
      TrailingCheck,
      TrailingLong,
      TrailingShort,
      Trailing,
      TradeLong,
      TradeShort,
      CommissionType,
      EntryCom,
      ExitCom,
      Account,
      VolumePercent,
      Bars,
      PriceField,
      BarLength,
      Symbols,
      Summary
    };

    TesterRule ();
    void setName (QString &);
    void getName (QString &);
    void setParms (QString &);
    void getParms (QString &);
    void setTrades (QString &);
    void getTrades (QString &);
    void setSummary (QString &);
    void getSummary (QString &);

    void setEnterLong (Indicator &);
    void getEnterLong (Indicator &);
    void setExitLong (Indicator &);
    void getExitLong (Indicator &);
    void setEnterShort (Indicator &);
    void getEnterShort (Indicator &);
    void setExitShort (Indicator &);
    void getExitShort (Indicator &);
    void setCustomLongStop (Indicator &);
    void getCustomLongStop (Indicator &);
    void setCustomShortStop (Indicator &);
    void getCustomShortStop (Indicator &);

    void getSymbols (QStringList &);
    void setSymbols (QStringList &);
    
    void setData (TesterParm, QString &);
    void getData (TesterParm, QString &);
    double getDouble (TesterParm);
    void setDouble (TesterParm, double);
    int getInt (TesterParm);
    void setInt (TesterParm, int);

  protected:
    QHash<TesterParm, QString> data;
    Indicator enterLong;
    Indicator exitLong;
    Indicator enterShort;
    Indicator exitShort;
    Indicator customLongStop;
    Indicator customShortStop;
    QString name;
    QString summary;
    QString trades;
};

#endif
