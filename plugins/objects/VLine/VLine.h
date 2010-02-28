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

#ifndef VLINE_HPP
#define VLINE_HPP

#include "COPlugin.h"

class VLine : public COPlugin
{
  public:
    VLine ();
    void draw (ChartObject *, QPixmap &, BarData *, int, int, int, Scaler &);
    void getInfo (ChartObject *, Setting &);
    void dialog (ChartObject *);
    void setSettings (ChartObject *, QSqlQuery &q);
    void getSettings (ChartObject *, QString &);
    void create (ChartObject *);
    int create2 (ChartObject *, QDateTime &, double);
    void moving (ChartObject *, QDateTime &, double, int);
    void getIcon (QIcon &);
};

extern "C"
{
  COPlugin * createCOPlugin ();
}

#endif
