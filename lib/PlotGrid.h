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

#ifndef PLOT_GRID_HPP
#define PLOT_GRID_HPP

#include <QPixmap>
#include <QColor>
#include <QVector>

#include "Scaler.h"

class PlotGrid
{
  public:
    PlotGrid ();
    void draw (QPixmap &, int startX, int startIndex, int pixelspace, Scaler &);
    void setGridFlag (int);
    void setXGrid (QVector<int> &);
    void setGridColor (QColor &);
    void drawXGrid (QPixmap &, int startX, int startIndex, int pixelspace);
    void drawYGrid (QPixmap &, Scaler &, int startX);

  private:
    QColor gridColor;
    int gridFlag;
    QVector<int> xGrid;
};

#endif


