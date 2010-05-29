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

#ifndef RETRACEMENT_HPP
#define RETRACEMENT_HPP

#include "COPlugin.h"

class Retracement : public COPlugin
{
  public:
    Retracement ();
    void draw (PlotData &, DateBar &, Scaler &);
    void getInfo (Setting *);
    void dialog ();
    void load (QSqlQuery &q);
    void save ();
    void create ();
    int create2 (QDateTime &, double);
    int create3 (QDateTime &, double);
    void moving (QDateTime &, double, int);
    void getIcon (QIcon &);
    int inDateRange (QDateTime &, QDateTime &, DateBar &);
    int getHighLow (double &, double &);
    int CUS (QStringList &);
    
  protected:
    QDateTime _date;
    QDateTime _date2;
    QColor _color;
    double _high;
    double _low;
    int _extend;
    double _line1;
    double _line2;
    double _line3;
    double _line4;
    double _line5;
    double _line6;
};

#endif
