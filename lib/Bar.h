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
#include <qstringlist.h>
#include <qdict.h>
#include "BarDate.h"

typedef struct
{
  double v;

} BarItem;

class Bar
{
  public:
  
    Bar ();
    ~Bar ();
    int setDate (BarDate &);
    int setDate (QString &);
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
    double getOI ();
    void getString (QString &);
    void getFields (QStringList &);
    void setData (QString &, double);
    double getData (QString &);
    void copy (Bar *);
    double getMin ();
    double getMax ();
    bool getTickFlag ();
    void setTickFlag (bool);
    int count ();
    
  protected:
    BarDate date;
    QDict<BarItem> data;
    double min;
    double max;
};

#endif
