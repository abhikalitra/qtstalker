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

// *************************************************************************************************
// *************************************************************************************************

#ifndef CURVE_DATA_HPP
#define CURVE_DATA_HPP

#include "Data.h"

class CurveData : public Data
{
  public:
    enum Parm
    {
      _LABEL = -10,
      _TYPE = -20,
      _CHART = -30,
      _Z = -40,
      _PEN = -50,
      _STYLE = -60
    };

    CurveData ();
    ~CurveData ();
    void clear ();
    int set (int, Data *);
    Data * getData (int);
    QList<int> barKeys ();
    QString toString ();
    int fromString (QString);
    int barKeyCount ();
    void barKeyRange (int &start, int &end);

  protected:
    QHash<int, Data *> _bars;
    int _startIndex;
    int _endIndex;
};

#endif
