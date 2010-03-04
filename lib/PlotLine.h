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

#ifndef PLOTLINE_HPP
#define PLOTLINE_HPP

#include <QString>
#include <QList>
#include <QColor>

#include "PlotLineBar.h"
#include "Setting.h"

class PlotLine
{
  public:
    PlotLine ();
    void setPlugin (QString &);
    void getPlugin (QString &);
    void setLabel (QString &);
    void getLabel (QString &);
    void append (double);
    void append (QColor &, double);
    void append (PlotLineBar &);
    void prepend (double);
    void prepend (QColor &, double);
    double getData (int, QColor &);
    double getData (int);
    void getData (int, PlotLineBar &);
    void setData (int, double);
    int count ();
    double getHigh ();
    void setHigh (double);
    double getLow ();
    void setLow (double);
    void checkHighLow (double);
    void setScaleFlag (bool);
    bool getScaleFlag ();
    void setPlotFlag (bool);
    bool getPlotFlag ();
    void getHighLowRange (int, int, double &, double &);
    void setOffset (int);
    int getOffset ();
    void getInfo (int, Setting &);
    void setColor (QColor &);
    void setColor (QString &);
    void setColorBar (int, QColor &);

  private:
    QList<PlotLineBar> data;
    QString label;
    QString plugin;
    double high;
    double low;
    bool scaleFlag;
    bool plotFlag;
    int offset;
};

#endif

