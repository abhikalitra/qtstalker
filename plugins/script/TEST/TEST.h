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

#ifndef PLUGIN_TEST_HPP
#define PLUGIN_TEST_HPP

#include <QList>

#include "Plugin.h"
#include "Curve.h"
#include "Operator.h"
#include "Setting.h"

class TEST : public Plugin
{
  public:
    enum Status
    {
      _NONE,
      _LONG,
      _SHORT,
      _LONG_ENTER_DELAY,
      _SHORT_ENTER_DELAY,
      _EXIT_DELAY
    };
    
    TEST ();
    ~TEST ();
    void init ();
    int command (Command *);
    int setRule (QString, Command *);
    int test ();
    int enterTrade (int status, int pos);
    int exitTrade (int pos, int signal);
    int updateTrade (int pos);
    int saveSummary ();
    int maxLossStop (int);
    int trailingStop (int);

  private:
    QString _name;
    QString _symbol;
    QString _tag;
    Setting _bestReport;
    Curve *_enterLong;
    Curve *_enterLong2;
    Curve *_exitLong;
    Curve *_exitLong2;
    Curve *_enterShort;
    Curve *_enterShort2;
    Curve *_exitShort;
    Curve *_exitShort2;
    Curve *_buyPrice;
    Curve *_sellPrice;
    Curve *_closePrice;
    Curve *_date;
    QList<Setting *> _trades;
    double _volume;
    double _enterCommission;
    double _exitCommission;
    double _equity;
    double _startEquity;
    Operator::Type _enterLongOp;
    Operator::Type _exitLongOp;
    Operator::Type _enterShortOp;
    Operator::Type _exitShortOp;
    int _delay;
    double _maxLossStop;
    double _trailingStop;
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif