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

#include "RSI.h"
#include <math.h>

RSI::RSI ()
{
  pluginName = "RSI";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "14", Setting::Integer);
  set(tr("Smoothing"), "10", Setting::Integer);
  set(tr("Smoothing Type"), tr("SMA"), Setting::MAType);
  set(tr("Input"), tr("Close"), Setting::InputField);
  set(tr("Buy Line"), "25", Setting::Integer);
  set(tr("Sell Line"), "75", Setting::Integer);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Relative Strength Index\n";
}

RSI::~RSI ()
{
}

void RSI::calculate ()
{
  int period = getInt(tr("Period"));

  PlotLine *in = getInput(getData(tr("Input")));

  PlotLine *rsi = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
  {
    double loss = 0;
    double gain = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
    {
      double t = in->getData(loop - loop2) - in->getData(loop - loop2 - 1);
      if (t > 0)
        gain = gain + t;
      if (t < 0)
        loss = loss + fabs(t);
    }

    double again = gain / period;
    double aloss = loss / period;
    double rs = again / aloss;
    double t = 100 - (100 / (1 + rs));
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    rsi->append(t);
  }

  if (getInt(tr("Smoothing")) > 1)
  {
    PlotLine *ma = getMA(rsi, getData(tr("Smoothing Type")), getInt(tr("Smoothing")));
    ma->setColor(getData(tr("Color")));
    ma->setType(getData(tr("Line Type")));
    ma->setLabel(getData(tr("Label")));
    output.append(ma);
    delete rsi;
  }
  else
  {
    rsi->setColor(getData(tr("Color")));
    rsi->setType(getData(tr("Line Type")));
    rsi->setLabel(getData(tr("Label")));
    output.append(rsi);
  }

  delete in;
}

QMemArray<int> RSI::getAlerts ()
{
  alerts.fill(0, data->count());

  if (! output.count())
    return alerts;

  int buy = getInt(tr("Buy Line"));

  int sell = getInt(tr("Sell Line"));

  PlotLine *line = output.at(0);

  int dataLoop = data->count() - line->getSize() + 1;
  int loop;
  int status = 0;
  for (loop = 1; loop < (int) line->getSize(); loop++, dataLoop++)
  {
    switch (status)
    {
      case -1:
        if ((line->getData(loop) <= buy) && (line->getData(loop) > line->getData(loop - 1)))
	  status = 1;
	break;
      case 1:
        if ((line->getData(loop) >= sell) && (line->getData(loop) < line->getData(loop - 1)))
	  status = -1;
	break;
      default:
        if ((line->getData(loop) <= buy) && (line->getData(loop) > line->getData(loop - 1)))
	  status = 1;
	else
	{
          if ((line->getData(loop) >= sell) && (line->getData(loop) < line->getData(loop - 1)))
	    status = -1;
	}
	break;
    }

    alerts[dataLoop] = status;
  }

  return alerts;
}

Plugin * create ()
{
  RSI *o = new RSI;
  return ((Plugin *) o);
}



