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

#include "MAOSC.h"

MAOSC::MAOSC ()
{
  pluginName = "MAOSC";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Histogram"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("MA Type"), tr("SMA"), Setting::MAType);
  set(tr("MA Type 2"), tr("SMA"), Setting::MAType);
  set(tr("MA Period"), "9", Setting::Integer);
  set(tr("MA Period 2"), "18", Setting::Integer);
  set(tr("Input"), tr("Close"), Setting::InputField);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Moving Average Oscillator\n";
}

MAOSC::~MAOSC ()
{
}

void MAOSC::calculate ()
{
  int period = getInt(tr("MA Period"));

  int period2 = getInt(tr("MA Period 2"));

  PlotLine *in = getInput(getData(tr("Input")));

  QString type = getData(tr("MA Type"));

  QString type2 = getData(tr("MA Type 2"));

  PlotLine *ma = new PlotLine();

  PlotLine *fma = getMA(in, type, period);
  int fmaLoop = fma->getSize() - 1;

  PlotLine *sma = getMA(in, type2, period2);
  int smaLoop = sma->getSize() - 1;

  while (fmaLoop > -1 && smaLoop > -1)
  {
    ma->prepend(fma->getData(fmaLoop) - sma->getData(smaLoop));
    fmaLoop--;
    smaLoop--;
  }

  ma->setColor(getData(tr("Color")));
  ma->setType(getData(tr("Line Type")));
  ma->setLabel(getData(tr("Label")));
  output.append(ma);

  delete in;
  delete fma;
  delete sma;
}

QMemArray<int> MAOSC::getAlerts ()
{
  alerts.fill(0, data->count());

  if (! output.count())
    return alerts;

  PlotLine *line = output.at(0);
  int dataLoop = data->count() - line->getSize();
  int loop;
  int status = 0;
  for (loop = 0; loop < (int) line->getSize(); loop++, dataLoop)
  {
    switch (status)
    {
      case -1:
        if (line->getData(loop) > 0)
	  status = 1;
	break;
      case 1:
        if (line->getData(loop) < 0)
	  status = -1;
	break;
      default:
        if (line->getData(loop) > 0)
	  status = 1;
	else
	{
          if (line->getData(loop) < 0)
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
  MAOSC *o = new MAOSC;
  return ((Plugin *) o);
}

