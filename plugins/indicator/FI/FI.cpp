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

#include "FI.h"

FI::FI ()
{
  pluginName = "FI";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "orange", Setting::Color);
  set(tr("Line Type"), tr("Histogram Bar"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Smoothing"), "2", Setting::Integer);
  set(tr("Smoothing Type"), tr("EMA"), Setting::MAType);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Force Index\n";
}

FI::~FI ()
{
}

void FI::calculate ()
{
  PlotLine *fi = new PlotLine();

  int loop;
  double force = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    Setting *r = data->at(loop);
    Setting *pr = data->at(loop - 1);
    double cdiff = r->getFloat("Close") - pr->getFloat("Close");
    force = r->getFloat("Volume") * cdiff;
    fi->append(force);
  }

  if (getInt(tr("Smoothing")) > 1)
  {
    PlotLine *ma = getMA(fi, getData(tr("Smoothing Type")), getInt(tr("Smoothing")));
    ma->setColor(getData(tr("Color")));
    ma->setType(getData(tr("Line Type")));
    ma->setLabel(getData(tr("Label")));
    output.append(ma);
    delete fi;
  }
  else
  {
    fi->setColor(getData(tr("Color")));
    fi->setType(getData(tr("Line Type")));
    fi->setLabel(getData(tr("Label")));
    output.append(fi);
  }
}

Plugin * create ()
{
  FI *o = new FI;
  return ((Plugin *) o);
}



