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

#include "OI.h"

OI::OI ()
{
  pluginName = "OI";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "yellow", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Open Interest\n";
}

OI::~OI ()
{
}

void OI::calculate ()
{
  PlotLine *pl = getInput(tr("Open Interest"));
  pl->setColor(getData(tr("Color")));
  pl->setType(getData(tr("Line Type")));
  pl->setLabel(getData(tr("Label")));
  output.append(pl);
}

Plugin * create ()
{
  OI *o = new OI;
  return ((Plugin *) o);
}

