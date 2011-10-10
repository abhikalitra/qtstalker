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

#include <QObject>
#include <QList>

#include "Data.h"
#include "TestTrade.h"

class Test : public QObject
{
  public:
    enum Status
    {
      _NONE,
      _LONG,
      _SHORT
    };

    Test (QObject *);
    ~Test ();
    void clear ();
    int run ();
    int enterTrade (int status, int pos);
    int exitTrade (int pos, int signal);
    int updateTrade (int pos);
    int saveSummary ();

    void setName (QString);
    void setSymbol (QString);
    void setEnterLong (Data *);
    void setExitLong (Data *);
    void setEnterShort (Data *);
    void setExitShort (Data *);
    void setDate (Data *);
    void setOpen (Data *);
    void setHigh (Data *);
    void setLow (Data *);
    void setClose (Data *);
    void setEnterComm (double);
    void setExitComm (double);
    void setEquity (double);
    void setMaxLossStop (double);
    void setTrailingStop (bool);
    void setVolume (double);

  private:
    QString _name;
    QString _symbol;
    Data *_enterLong;
    Data *_exitLong;
    Data *_enterShort;
    Data *_exitShort;
    Data *_open;
    Data *_high;
    Data *_low;
    Data *_close;
    Data *_date;
    QList<TestTrade *> _trades;
    double _volume;
    double _enterComm;
    double _exitComm;
    double _equity;
    double _startEquity;
    double _maxLossStop;
    bool _maxLossFlag;
    double _trailingStop;
    bool _trailingFlag;
};

#endif
