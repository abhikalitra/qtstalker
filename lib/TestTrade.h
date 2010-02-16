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

#ifndef TRADE_HPP
#define TRADE_HPP

#include <QString>
#include <QDateTime>

#include "PlotLine.h"
#include "BarData.h"

class TestTrade
{
  public:
    enum Signal
    {
      SignalNone,
      SignalEnterLong,
      SignalExitLong,
      SignalEnterShort,
      SignalTestEnd,
      SignalTrailingStop
    };

    TestTrade ();
    void getEnterDate (QDateTime &);
    void setEnterDate (QDateTime &, int index);
    void getExitDate (QDateTime &);
    void setExitDate (QDateTime &, int index);
    double getEnterPrice ();
    void setEnterPrice (double);
    double getExitPrice ();
    void setExitPrice (double);
    int getType ();
    void setType (int);
    int getVolume ();
    void setVolume (int);
    double getVolumePercentage ();
    void setVolumePercentage (double);
    double getValue ();
    double getDrawDown ();
    double getProfit ();
    void getLogMessage (QString &);
    void update (PlotLine *line, BarData *data, double account);
    int getSignal ();
    void setSignal (int);
    void setTrailing (double);
    double calculateTrailingStop (double price);

  private:
    QDateTime enterDate;
    QDateTime exitDate;
    double enterPrice;
    double exitPrice;
    int type;
    int volume;
    double volumePercentage;
    double drawDown;
    double value;
    double profit;
    int indexStart;
    int indexEnd;
    int signal;
    double trailing;
};

#endif

