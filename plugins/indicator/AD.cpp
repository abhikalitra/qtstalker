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

#include "AD.h"

AD::AD ()
{
  pluginName = "AD";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Acumulation Distribution\n";
}

AD::~AD ()
{
}

void AD::calculate ()
{
  PlotLine *line = new PlotLine;
  line->setColor(getData(tr("Color")));
  line->setType(getData(tr("Line Type")));
  line->setLabel(getData(tr("Label")));

  int loop;
  double accum = 0;
  for (loop = 0; loop < (int) data.count(); loop++)
  {
    Setting *set = data.at(loop);
    double volume = set->getFloat("Volume");
    if (volume > 0)
    {
      double high = set->getFloat("High");
      double low = set->getFloat("Low");

      double t = high - low;

      if (t != 0)
      {
        double close = set->getFloat("Close");
        double t2 = (close - low) - (high - close);
        accum = accum + ((t2 / t) * volume);
      }
    }

    line->append(accum);
  }

  output.append(line);
}

Plugin * create ()
{
  AD *o = new AD;
  return ((Plugin *) o);
}



