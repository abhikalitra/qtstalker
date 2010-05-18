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

#ifndef PLOT_LINE_BAR_HPP
#define PLOT_LINE_BAR_HPP

#include <QColor>
#include <QHash>

class PlotLineBar
{
  public:
    PlotLineBar ();
    PlotLineBar (double);
    PlotLineBar (QColor &, double);
    double data (int);
    double data ();
    void setData (int, double);
    void setData (double);
    void setColor (QColor &);
    QColor & color ();
    int count ();
    int highLow (double &, double &);

  protected:
    QHash<int, double> _data;
    QColor _color;
};

#endif
