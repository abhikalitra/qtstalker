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

#include "PlotCursor.h"

PlotCursor::PlotCursor ()
{
}

PlotCursor::~PlotCursor ()
{
}

// virtual
int PlotCursor::getCursor ()
{
  return 0;
}

// virtual
void PlotCursor::draw (QPainter &, PlotData &, DateBar &, Scaler &)
{
}

// virtual
void PlotCursor::mousePress (PlotData &, DateBar &, Scaler &, Indicator &)
{
}

// virtual
void PlotCursor::mouseDoubleClick (PlotData &, DateBar &, Scaler &)
{
}

// virtual
void PlotCursor::mouseMove (PlotData &, DateBar &, Scaler &, Indicator &)
{
}

//********************************************************
//************ NON VIRTUAL FUNCTIONS *********************
//********************************************************

QString & PlotCursor::type ()
{
  return _type;
}

int PlotCursor::convertXToDataIndex (int x, PlotData &pd, DateBar &dateBars)
{
  int i = (x / pd.barSpacing) + pd.startIndex;
  if (i >= (int) dateBars.count())
    i = dateBars.count() - 1;
  if (i < pd.startIndex)
    i = pd.startIndex;

  return i;
}

