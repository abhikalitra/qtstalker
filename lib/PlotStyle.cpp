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

#include "PlotStyle.h"
#include "Strip.h"

PlotStyle::PlotStyle ()
{
}

// virtual
void PlotStyle::draw (PlotData &, Scaler &, QMap<int, PlotLineBar *> &)
{
}

// virtual
void PlotStyle::info (int i, Setting *set, QMap<int, PlotLineBar *> &data, QString &label)
{
  PlotLineBar *bar = data.value(i);
  if (! bar)
    return;

  QString d;
  Strip strip;
  strip.strip(bar->data(), 4, d);
  set->setData(label, d);
}

