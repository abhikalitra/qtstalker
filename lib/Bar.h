/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#ifndef BAR_HPP
#define BAR_HPP

#include <qstring.h>
#include "BarDate.h"

class Bar
{
  public:
    Bar ();
    ~Bar ();
    int setDate (BarDate);
    int setDate (QString);
    BarDate getDate ();
    void setOpen (double);
    double getOpen ();
    void setHigh (double);
    double getHigh ();
    void setLow (double);
    double getLow ();
    void setClose (double);
    double getClose ();
    void setVolume (double);
    double getVolume ();
    void setOI (int);
    int getOI ();
    QString getString ();
    
  protected:
    BarDate date;
    double open;
    double high;
    double low;
    double close;
    double volume;
    int oi;
};

#endif