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

#include "MA.h"

MA::MA ()
{
  pluginName = "MA";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "10", Setting::Integer);
  set(tr("Displace"), "0", Setting::Integer);
  set(tr("Input"), tr("Close"), Setting::InputField);
  set(tr("MA Type"), "SMA", Setting::MAType);
  set(tr("Plot"), tr("True"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Moving Average\n";
}

MA::~MA ()
{
}

void MA::calculate ()
{
  PlotLine *in = getInput(getData(tr("Input")));

  PlotLine *data = getMA(in, getData(tr("MA Type")), getInt(tr("Period")), getInt(tr("Displace")));

  delete in;

  data->setColor(getData(tr("Color")));
  data->setType(getData(tr("Line Type")));
  data->setLabel(getData(tr("Label")));
  output.append(data);
}

QMemArray<int> MA::getAlerts ()
{
  alerts.fill(0, data->count());

  if (! output.count())
    return alerts;

  PlotLine *line = output.at(0);

  PlotLine *in = getInput(getData(tr("Input")));

  int listLoop = data->count() - line->getSize() + 1;
  int lineLoop;
  int status = 0;
  for (lineLoop = 1; listLoop < (int) data->count(); lineLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if ((in->getData(listLoop) > line->getData(lineLoop)) && (in->getData(listLoop - 1) >= line->getData(lineLoop - 1)))
	  status = 1;
	break;
      case 1:
        if ((in->getData(listLoop) < line->getData(lineLoop)) && (in->getData(listLoop - 1) <= line->getData(lineLoop - 1)))
	  status = -1;
	break;
      default:
        if ((in->getData(listLoop) > line->getData(lineLoop)) && (in->getData(listLoop - 1) >= line->getData(lineLoop - 1)))
	  status = 1;
	else
	{
          if ((in->getData(listLoop) < line->getData(lineLoop)) && (in->getData(listLoop - 1) <= line->getData(lineLoop - 1)))
	    status = -1;
	}
	break;
    }
    
    alerts[listLoop] = status;
  }

  delete in;

  return alerts;
}

Plugin * create ()
{
  MA *o = new MA;
  return ((Plugin *) o);
}


