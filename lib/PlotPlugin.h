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

#ifndef PLOT_PLUGIN_HPP
#define PLOT_PLUGIN_HPP

#include <QPixmap>
#include <QPainter>
#include <QString>

#include "PlotLine.h"
#include "Scaler.h"
#include "BarData.h"

class PlotPlugin
{
  public:
    PlotPlugin ();
    virtual ~PlotPlugin ();
    // ints are startX, pixelspace, pos
    virtual void draw (PlotLine *, BarData *, QPixmap &, int, int, int, Scaler &);
    void strip (double, int, QString &);
};

#endif

