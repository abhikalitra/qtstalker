/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "VOL.h"

VOL::VOL ()
{
  pluginName = "VOL";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Color Bars"), tr("True"), Setting::Bool);
  set(tr("Line Type"), tr("Histogram Bar"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Volume\n";
}

VOL::~VOL ()
{
}

void VOL::calculate ()
{
  PlotLine *pl = getInput(tr("Volume"));
  pl->setColor(getData(tr("Color")));
  pl->setType(getData(tr("Line Type")));
  pl->setLabel(getData(tr("Label")));
  if (! getData(tr("Color Bars")).compare(tr("True")))
    pl->setColorBars(TRUE);
  output.append(pl);
}

Plugin * create ()
{
  VOL *o = new VOL;
  return ((Plugin *) o);
}

