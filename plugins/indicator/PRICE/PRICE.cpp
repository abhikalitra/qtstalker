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

#include "PRICE.h"

PRICE::PRICE ()
{
  pluginName = "PRICE";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "green", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Plot"), tr("True"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);
  set(tr("Smoothing Period"), "0", Setting::Integer);
  set(tr("Smoothing Displacement"), "0", Setting::Integer);
  set(tr("Input"), tr("Close"), Setting::InputField);
  set(tr("Smoothing Type"), "SMA", Setting::MAType);

  about = "Price\n";
}

PRICE::~PRICE ()
{
}

void PRICE::calculate ()
{
  PlotLine *line = getInput(getData(tr("Input")));
  line->setColor(getData(tr("Color")));
  line->setType(getData(tr("Line Type")));
  line->setLabel(getData(tr("Label")));

  if (getInt(tr("Smoothing Period")) < 1)
  {
    output.append(line);
    return;
  }

  PlotLine *ma = getMA(line,
  		       getData(tr("Smoothing Type")),
		       getInt(tr("Smoothing Period")),
		       getInt(tr("Smoothing Displacement")));
  ma->setColor(getData(tr("Color")));
  ma->setType(getData(tr("Line Type")));
  ma->setLabel(getData(tr("Label")));
  delete line;
  output.append(ma);
}

Plugin * create ()
{
  PRICE *o = new PRICE;
  return ((Plugin *) o);
}

