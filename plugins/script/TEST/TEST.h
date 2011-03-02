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

#ifndef TEST_HPP
#define TEST_HPP

#include "ScriptPlugin.h"
#include "Curve.h"
#include "Operator.h"

class TEST : public ScriptPlugin
{
  public:
    enum Method
    {
      _SET_RULE,
      _TEST,
      _SET_VOLUME,
      _SET_ENTER_COMM,
      _SET_EXIT_COMM,
      _SET_EQUITY,
      _SET_PRICES,
      _RESET,
      _SAVE
    };
    
    TEST ();
    ~TEST ();
    void init ();
    int command (Command *);
    int setRule (Command *);
    int test (Command *);
    int setVolume (Command *);
    int setEnterComm (Command *);
    int setExitComm (Command *);
    int setEquity (Command *);
    int enterTrade (int status, int pos);
    int exitTrade (int pos, int signal);
    int updateTrade (int pos);
    int setPrices (Command *);
    int reset (Command *);
    int save (Command *);

  private:
    QStringList _method;
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
    QList<Setting *> _trades;
    double _volume;
    double _enterCommission;
    double _exitCommission;
    double _equity;
    Operator::Type _enterLongOp;
    Operator::Type _exitLongOp;
    Operator::Type _enterShortOp;
    Operator::Type _exitShortOp;
    double _enterLongValue;
    double _exitLongValue;
    double _enterShortValue;
    double _exitShortValue;
};

extern "C"
{
  ScriptPlugin * createScriptPlugin ();
}

#endif
