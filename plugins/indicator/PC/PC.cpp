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

#include "PC.h"

PC::PC ()
{
  pluginName = "PC";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Histogram"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "1", Setting::Integer);
  set(tr("Input"), tr("Close"), Setting::InputField);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Percent Change\n";
}

PC::~PC ()
{
}

void PC::calculate ()
{
  int period = getInt(tr("Period"));

  PlotLine *in = getInput(getData(tr("Input")));

  PlotLine *pc = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
  {
    double t = in->getData(loop) - in->getData(loop - period);
    double t2 = (t / in->getData(loop - period)) * 100;
    pc->append(t2);
  }

  pc->setColor(getData(tr("Color")));
  pc->setType(getData(tr("Line Type")));
  pc->setLabel(getData(tr("Label")));
  output.append(pc);

  delete in;
}

Plugin * create ()
{
  PC *o = new PC;
  return ((Plugin *) o);
}





