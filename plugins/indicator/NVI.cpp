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

#include "NVI.h"

NVI::NVI ()
{
  pluginName = "NVI";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Negative Volume Index\n";
}

NVI::~NVI ()
{
}

void NVI::calculate ()
{
  PlotLine *nvi = new PlotLine();

  int loop;
  double nv = 1000;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    Setting *set = data->at(loop);
    double volume = set->getFloat("Volume");
    double close = set->getFloat("Close");
    set = data->at(loop - 1);
    double yvolume = set->getFloat("Volume");
    double yclose = set->getFloat("Close");

    if (volume < yvolume)
      nv = nv + ((close - yclose) / yclose) * nv;

    nvi->append(nv);
  }

  nvi->setColor(getData(tr("Color")));
  nvi->setType(getData(tr("Line Type")));
  nvi->setLabel(getData(tr("Label")));
  output.append(nvi);
}

Plugin * create ()
{
  NVI *o = new NVI;
  return ((Plugin *) o);
}


