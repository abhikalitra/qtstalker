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

#ifndef BARDATA_HPP
#define BARDATA_HPP

#include <qlist.h>
#include <qdict.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include "Setting.h"

class BarData
{
  public:
  
    typedef struct
    {
      QDateTime date;
      double open;
      double high;
      double low;
      double close;
      double volume;
      int oi;
    } Bar;
    
    typedef struct
    {
      int x;
    } X;
  
    enum BarType
    {
      Bars,
      Other
    };
    
    enum BarCompression
    {
      Daily,
      Weekly,
      Monthly
    };
    
    BarData ();
    BarData (QStringList);
    ~BarData ();
    int count ();
    QDateTime getDate (int);
    double getOpen (int);
    double getHigh (int);
    double getLow (int);
    double getClose (int);
    double getVolume (int);
    int getOI (int);
    void prepend (Setting *);
    int getX (QDateTime);
    double getMax ();
    double getMin ();
    void createDateList ();
    double getOther (int, int);
    BarData::BarType getType ();
    QStringList getFormat ();
    void setBarCompression (BarData::BarCompression);
    BarData::BarCompression getBarCompression ();
    
  protected:
    QList<Bar> barList;
    QDict<X> dateList;
    double high;
    double low;
    BarType type;
    QStringList format;
    BarCompression compression;
};

#endif
