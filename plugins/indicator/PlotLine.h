/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#ifndef PLOTLINE_HPP
#define PLOTLINE_HPP

#include <qstring.h>
#include <qlist.h>
#include <qmemarray.h>

typedef struct
{
  double v;

} Val;

class PlotLine
{
  public:
    PlotLine ();
    ~PlotLine ();
    void setColor (QString);
    QString getColor ();
    void setType (QString);
    QString getType ();
    void setLabel (QString);
    QString getLabel ();
    void append (double);
    void prepend (double);
    double getData (int);
    void setData (int, double);
    int getSize ();
    double getHigh ();
    double getLow ();
    void checkHighLow (double);

  private:
    QList<Val> data;
    QString color;
    QString lineType;
    QString label;
    double high;
    double low;
};

#endif

