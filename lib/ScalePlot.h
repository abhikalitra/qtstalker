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

#ifndef SCALEPLOT_HPP
#define SCALEPLOT_HPP

#include <QList>

#include "PlotData.h"
#include "Setting.h"
#include "Scaler.h"
#include "Indicator.h"

class ScalePlot
{
  public:
    ScalePlot ();
    void draw (PlotData &, Scaler &, Indicator &);
    void drawPoints (PlotData &, Scaler &, Indicator &);
    void points (PlotData &, QList<Setting> &, Indicator &);
};

#endif


