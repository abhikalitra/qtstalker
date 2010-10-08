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

#ifndef TESTER_TRADE_HPP
#define TESTER_TRADE_HPP

#include <QDateTime>

class TesterTrade
{
  public:
    enum Signal
    {
      _None,
      _ExitLong,
      _ExitShort,
      _TestEnd
    };

    TesterTrade ();
    QDateTime & enterDate ();
    QDateTime & exitDate ();
    double enterPrice ();
    double exitPrice ();
    int type ();
    int volume ();
    int enterIndex ();
    int exitIndex ();
    double equity ();
    double enterCommission ();
    double exitCommission ();
    double profit ();
    void enterTrade (int type, double equity, QDateTime date, double price, int volume, int index, double commission);
    void exitTrade (QDateTime date, double price, int index, double commission, int signal);
    void update (double price);
    void tradeString (QString &);
    int isOpenTrade ();
    double drawDown ();
    int barsHeld ();
    double commissions ();

  private:
    QDateTime _enterDate;
    QDateTime _exitDate;
    double _enterPrice;
    double _exitPrice;
    int _type;
    int _volume;
    int _enterIndex;
    int _exitIndex;
    double _equity;
    double _enterCommission;
    double _exitCommission;
    double _profit;
    double _low;
    double _high;
    Signal _signal;
};

#endif

