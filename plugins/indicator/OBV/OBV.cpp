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

#include "OBV.h"

OBV::OBV ()
{
  pluginName = "OBV";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "On Balance Volume\n";
}

OBV::~OBV ()
{
}

void OBV::calculate ()
{
  PlotLine *obv = new PlotLine();

  int loop;
  double t = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    Setting *set = data->at(loop);
    double close = set->getFloat("Close");
    double volume = set->getFloat("Volume");
    set = data->at(loop - 1);
    double yclose = set->getFloat("Close");

    if (close > yclose)
      t = t + volume;
    else
    {
      if (close < yclose)
      	t = t - volume;
    }

    obv->append(t);
  }

  obv->setColor(getData(tr("Color")));
  obv->setType(getData(tr("Line Type")));
  obv->setLabel(getData(tr("Label")));
  output.append(obv);
}

Plugin * create ()
{
  OBV *o = new OBV;
  return ((Plugin *) o);
}

