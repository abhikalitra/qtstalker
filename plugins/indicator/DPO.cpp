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

#include "DPO.h"

DPO::DPO ()
{
  pluginName = "DPO";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "21", Setting::Integer);
  set(tr("MA Type"), "SMA", Setting::MAType);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Detrended Price Oscillator\n";
}

DPO::~DPO ()
{
}

void DPO::calculate ()
{
  int period = getInt(tr("Period"));

  PlotLine *c = getInput(tr("Close"));

  PlotLine *ma = getMA(c, getData(tr("MA Type")), period);

  PlotLine *dpo = new PlotLine();

  int maLoop = ma->getSize() - 1;
  int closeLoop = c->getSize() - 1;
  int t = (int) ((period / 2) + 1);

  while (maLoop >= t)
  {
    dpo->prepend(c->getData(closeLoop) - ma->getData(maLoop - t));
    closeLoop--;
    maLoop--;
  }

  delete c;
  delete ma;

  dpo->setColor(getData(tr("Color")));
  dpo->setType(getData(tr("Line Type")));
  dpo->setLabel(getData(tr("Label")));
  output.append(dpo);
}

Plugin * create ()
{
  DPO *o = new DPO;
  return ((Plugin *) o);
}

