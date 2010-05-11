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

#ifndef TLINE_HPP
#define TLINE_HPP

#include <QStringList>

#include "COPlugin.h"

class TLine : public COPlugin
{
  public:
    TLine ();
    void draw (PlotData &);
    void getInfo (Setting *);
    void dialog ();
    void load (QSqlQuery &q);
    void save ();
    void create ();
    int create2 (QDateTime &, double);
    int create3 (QDateTime &, double);
    void moving (QDateTime &, double, int);
    void getIcon (QIcon &);
    int getHighLow (double &, double &);
    int inDateRange (PlotData &, QDateTime &, QDateTime &);

  protected:
    QStringList fieldList;
    QColor color;
    double price;
    double price2;
    QDateTime date;
    QDateTime date2;
    int extend;
};

#endif