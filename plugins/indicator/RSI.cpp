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

#include "RSI.h"
#include <math.h>

RSI::RSI ()
{
  pluginName = "RSI";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("RSI Color"), "red", Setting::Color);
  set(tr("MA Color"), "yellow", Setting::Color);
  set(tr("RSI Line Type"), tr("Line"), Setting::LineType);
  set(tr("MA Line Type"), tr("Dash"), Setting::LineType);
  set(tr("RSI Label"), pluginName, Setting::Text);
  set(tr("MA Label"), tr("RSI MA"), Setting::Text);
  set(tr("RSI Period"), "14", Setting::Integer);
  set(tr("MA Period"), "14", Setting::Integer);
  set(tr("MA Type"), tr("SMA"), Setting::MAType);
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
  int period = getInt(tr("RSI Period"));

  PlotLine *in = getInput(getData(tr("Input")));

  PlotLine *rsi = new PlotLine();
  rsi->setColor(getData(tr("RSI Color")));
  rsi->setType(getData(tr("RSI Line Type")));
  rsi->setLabel(getData(tr("RSI Label")));

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

  output.append(rsi);

  period = getInt(tr("MA Period"));
  if (period)
  {
    PlotLine *ma = getMA(rsi, getData(tr("MA Type")), period);
    ma->setColor(getData(tr("MA Color")));
    ma->setType(getData(tr("MA Line Type")));
    ma->setLabel(getData(tr("MA Label")));
    output.append(ma);
  }

  delete in;
}

QMemArray<int> RSI::getAlerts ()
{
  alerts.fill(0, data.count());

  if (! output.count())
    return alerts;

  int buy = getInt(tr("Buy Line"));

  int sell = getInt(tr("Sell Line"));

  PlotLine *line;
  if (output.count() == 1)
    line = output.at(0);
  else
    line = output.at(1);

  int lineLoop;
  int listLoop = data.count() - line->getSize() + 1;
  int status = 0;
  for (lineLoop = 1; lineLoop < (int) line->getSize(); lineLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if ((line->getData(lineLoop) <= buy) && (line->getData(lineLoop) > line->getData(lineLoop - 1)))
	  status = 1;
	break;
      case 1:
        if ((line->getData(lineLoop) >= sell) && (line->getData(lineLoop) < line->getData(lineLoop - 1)))
	  status = -1;
	break;
      default:
        if ((line->getData(lineLoop) <= buy) && (line->getData(lineLoop) > line->getData(lineLoop - 1)))
	  status = 1;
	else
	{
          if ((line->getData(lineLoop) >= sell) && (line->getData(lineLoop) < line->getData(lineLoop - 1)))
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
  RSI *o = new RSI;
  return ((Plugin *) o);
}



