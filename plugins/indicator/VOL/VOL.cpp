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

#include "VOL.h"

VOL::VOL ()
{
  pluginName = "VOL";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color Up"), "green", Setting::Color);
  set(tr("Color Down"), "red", Setting::Color);
  set(tr("Line Type"), tr("Histogram Bar"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  set(tr("MA Color"), "yellow", Setting::Color);
  set(tr("MA Line Type"), tr("Line"), Setting::LineType);
  set(tr("MA Label"), tr("MAVol"), Setting::Text);
  set(tr("MA Period"), "0", Setting::Integer);
  set(tr("MA Displace"), "0", Setting::Integer);
  set(tr("MA Type"), "SMA", Setting::MAType);

  about = "Volume\n";
}

VOL::~VOL ()
{
}

void VOL::calculate ()
{
  PlotLine *pl = getInput(tr("Volume"));
  pl->setType(getData(tr("Line Type")));
  pl->setLabel(getData(tr("Label")));
  pl->setColorFlag(TRUE);

  QString upColor = getData(tr("Color Up"));
  QString downColor = getData(tr("Color Down"));

  // set the first bar color
  pl->appendColorBar(upColor);

  int loop;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    Setting *r = data->at(loop);
    Setting *pr = data->at(loop - 1);;

    if (r->getFloat("Close") > pr->getFloat("Close"))
      pl->appendColorBar(upColor);
    else
      pl->appendColorBar(downColor);
  }

  output.append(pl);

  if (getInt(tr("MA Period")) < 1)
    return;

  PlotLine *ma = getMA(pl, getData(tr("MA Type")), getInt(tr("MA Period")), getInt(tr("MA Displace")));
  ma->setColor(getData(tr("MA Color")));
  ma->setType(getData(tr("MA Line Type")));
  ma->setLabel(getData(tr("MA Label")));
  output.append(ma);
}

Plugin * create ()
{
  VOL *o = new VOL;
  return ((Plugin *) o);
}

