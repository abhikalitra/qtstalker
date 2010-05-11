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

#ifndef SELL_HPP
#define SELL_HPP

#include "COPlugin.h"

class Sell : public COPlugin
{
  public:
    Sell ();
    void draw (PlotData &, DateBar &, Scaler &);
    void getInfo (Setting *);
    void dialog ();
    void load (QSqlQuery &);
    void save ();
    void create ();
    int create2 (QDateTime &, double);
    void moving (QDateTime &, double, int);
    void getIcon (QIcon &);
    int getHighLow (double &, double &);
    int inDateRange (QDateTime &, QDateTime &, DateBar &);
    
  protected:
    QDateTime date;
    double price;
    QColor color;
};

#endif
