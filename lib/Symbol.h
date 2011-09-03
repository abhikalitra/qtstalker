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

// ******************************************************
// ******************************************************

#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "Data.h"

class Symbol : public Data
{
  public:
    enum Parm
    {
      _SYMBOL = -10,
      _EXCHANGE = -20,
      _NAME = -30,
      _LENGTH = -40,
      _RANGE = -50,
      _START_DATE = -60,
      _END_DATE = -70,
      _TABLE = -80,
      _TYPE = -90
    };

    Symbol ();
    ~Symbol ();
    void clear ();
    int set (int, Data *);
    Data * getData (int);
    QList<int> barKeys ();
    QString toString ();
    int fromString (QString);
    int barKeyCount ();
    void append (Data *);

  private:
    QHash<int, Data *> _bars;
    int _startIndex;
    int _endIndex;
};

#endif
