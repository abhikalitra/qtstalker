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

#include "ENV.h"

ENV::ENV ()
{
  pluginName = "ENV";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Upper Color"), "red", Setting::Color);
  set(tr("Lower Color"), "red", Setting::Color);
  set(tr("Upper Line Type"), tr("Line"), Setting::LineType);
  set(tr("Lower Line Type"), tr("Line"), Setting::LineType);
  set(tr("Upper Label"), tr("ENVU"), Setting::Text);
  set(tr("Lower Label"), tr("ENVL"), Setting::Text);
  set(tr("Period"), "10", Setting::Integer);
  set(tr("Upper Percent"), "1.02", Setting::Float);
  set(tr("Lower Percent"), "0.98", Setting::Float);
  set(tr("Input"), tr("Close"), Setting::InputField);
  set(tr("MA Type"), tr("SMA"), Setting::MAType);
  set(tr("Plot"), tr("True"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Moving Average Envelope\n";
}

ENV::~ENV ()
{
}

void ENV::calculate ()
{
  int period = getInt(tr("Period"));

  double up = getFloat(tr("Upper Percent"));

  double lp = getFloat(tr("Lower Percent"));

  PlotLine *in = getInput(getData(tr("Input")));

  PlotLine *uma = getMA(in, getData(tr("MA Type")), period);

  PlotLine *lma = getMA(in, getData(tr("MA Type")), period);

  int maLoop = uma->getSize() - 1;

  while (maLoop > -1)
  {
    double t = uma->getData(maLoop);
    uma->setData(maLoop, t * up);

    t = lma->getData(maLoop);
    lma->setData(maLoop, t * lp);

    maLoop--;
  }

  delete in;

  uma->setColor(getData(tr("Upper Color")));
  uma->setType(getData(tr("Upper Line Type")));
  uma->setLabel(getData(tr("Upper Label")));
  output.append(uma);

  lma->setColor(getData(tr("Lower Color")));
  lma->setType(getData(tr("Lower Line Type")));
  lma->setLabel(getData(tr("Lower Label")));
  output.append(lma);
}

QMemArray<int> ENV::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 2)
    return alerts;

  PlotLine *uma = output.at(0);
  PlotLine *lma = output.at(1);

  int listLoop = data->count() - uma->getSize();
  int maLoop;
  int status = 0;
  for (maLoop = 0; maLoop < (int) uma->getSize(); maLoop++, listLoop++)
  {
    double close = data->getClose(listLoop);

    double t = (close - lma->getData(maLoop)) / (uma->getData(maLoop) - lma->getData(maLoop));

    switch (status)
    {
      case -1:
        if ((t < .2) || (close < lma->getData(maLoop)))
          status = 1;
        break;
      case 1:
        if ((t > .8) || (close > uma->getData(maLoop)))
	  status = -1;
	break;
      default:
        if ((t < .2) || (close < lma->getData(maLoop)))
	  status = 1;
	else
	{
          if ((t > .8) || (close > uma->getData(maLoop)))
	    status = -1;
	}
	break;
    }
    
    alerts[listLoop] = status;
  }

  return alerts;
}

Plugin * create ()
{
  ENV *o = new ENV;
  return ((Plugin *) o);
}


