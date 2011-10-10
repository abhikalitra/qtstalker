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

#ifndef TEST_TRADE_HPP
#define TEST_TRADE_HPP

#include <QString>
#include <QDateTime>

class TestTrade
{
  public:
    TestTrade ();
    void clear();
    int enterTrade (QString type, double volume, double equity, double price, QDateTime dt, double comm);
    int updateTrade (double price, QDateTime dt);
    void exitTrade (int signal, double price, QDateTime dt, double comm);
    void setSymbol (QString);
    QString symbol ();
    void setName (QString);
    QString name ();
    void setType (QString);
    QString type ();
    void setEnterDate (QDateTime);
    QDateTime enterDate ();
    void setExitDate (QDateTime);
    QDateTime exitDate ();
    void setEnterPrice (double);
    double enterPrice ();
    void setExitPrice (double);
    double exitPrice ();
    void setVolume (double);
    double volume ();
    void setBarsHeld (int);
    int barsHeld ();
    void setEnterComm (double);
    double enterComm ();
    void setExitComm (double);
    double exitComm ();
    void setDrawdown (double);
    double drawdown ();
    void setSignal (int);
    int signal ();
    void setProfit (double);
    double profit ();
    void setTrailingStop (double);
    int trailingStop (double);
    void setMaxLossStop (double);
    int maxLossStop (double);

  private:
    QString _symbol;
    QString _name;
    QString _type;
    QDateTime _enterDate;
    QDateTime _exitDate;
    double _enterPrice;
    double _exitPrice;
    double _volume;
    int _barsHeld;
    double _enterComm;
    double _exitComm;
    double _drawdown;
    int _signal;
    double _profit;
    double _trailingStopPercentage;
    double _trailingStop;
    bool _trailingStopFlag;
    double _maxLossStopPercentage;
    bool _maxLossStopFlag;
};

#endif
