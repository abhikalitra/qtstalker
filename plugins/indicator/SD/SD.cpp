/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "SD.h"
#include <math.h>

SD::SD ()
{
  pluginName = "SD";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "21", Setting::Integer);
  set(tr("Input"), tr("Close"), Setting::InputField);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Standard Deviation\n";
}

SD::~SD ()
{
}

void SD::calculate ()
{
  int period = getInt(tr("Period"));

  PlotLine *in = getInput(getData(tr("Input")));

  PlotLine *sd = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
  {
    double mean = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
      mean = mean + in->getData(loop - loop2);
    mean = mean / period;

    double ds = 0;
    for (loop2 = 0; loop2 < period; loop2++)
    {
      double t = in->getData(loop - loop2) - mean;
      ds = ds + (t * t);
    }
    ds = ds / period;

    sd->append(ds);
  }

  sd->setColor(getData(tr("Color")));
  sd->setType(getData(tr("Line Type")));
  sd->setLabel(getData(tr("Label")));
  output.append(sd);

  delete in;
}

Plugin * create ()
{
  SD *o = new SD;
  return ((Plugin *) o);
}


