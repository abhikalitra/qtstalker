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

#include "TRIX.h"

TRIX::TRIX ()
{
  pluginName = "TRIX";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "12", Setting::Integer);
  set(tr("Input"), tr("Close"), Setting::InputField);

  set(tr("Trigger Color"), "yellow", Setting::Color);
  set(tr("Trigger Line Type"), tr("Dash"), Setting::LineType);
  set(tr("Trigger Label"), tr("TRIX Trigger"), Setting::Text);
  set(tr("Trigger Period"), "9", Setting::Integer);
  set(tr("Trigger MA Type"), tr("SMA"), Setting::MAType);

  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "TRIX Oscillator\n";
}

TRIX::~TRIX ()
{
}

void TRIX::calculate ()
{
  int period = getInt(tr("Period"));

  int tperiod = getInt(tr("Trigger Period"));

  PlotLine *in = getInput(getData(tr("Input")));

  PlotLine *ema = getEMA(in, period);

  PlotLine *ema2 = getEMA(ema, period);

  PlotLine *ema3 = getEMA(ema2, period);
  int emaLoop = ema3->getSize() - 1;

  PlotLine *trix = new PlotLine();

  while (emaLoop > 0)
  {
    trix->prepend(((ema3->getData(emaLoop) - ema3->getData(emaLoop - 1)) / ema3->getData(emaLoop - 1)) * 100);
    emaLoop--;
  }

  PlotLine *trigger = getMA(trix, getData(tr("Trigger MA Type")), tperiod);
  trigger->setColor(getData(tr("Trigger Color")));
  trigger->setType(getData(tr("Trigger Line Type")));
  trigger->setLabel(getData(tr("Trigger Label")));

  trix->setColor(getData(tr("Color")));
  trix->setType(getData(tr("Line Type")));
  trix->setLabel(getData(tr("Label")));
  output.append(trix);

  output.append(trigger);

  delete in;
  delete ema;
  delete ema2;
  delete ema3;
}

QMemArray<int> TRIX::getAlerts ()
{
  alerts.fill(0, data.count());

  if (output.count() != 2)
    return alerts;

  PlotLine *trix = output.at(0);
  PlotLine *trig = output.at(1);

  int listLoop = data.count() - trig->getSize();
  int trixLoop = trix->getSize() - trig->getSize();
  int trigLoop;
  int status = 0;
  for (trigLoop = 0; trigLoop < (int) trig->getSize(); trigLoop++, trixLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if (trix->getData(trixLoop) > trig->getData(trigLoop))
          status = 1;
	break;
      case 1:
        if (trix->getData(trixLoop) < trig->getData(trigLoop))
	  status = -1;
	break;
      default:
        if (trix->getData(trixLoop) > trig->getData(trigLoop))
	  status = 1;
	else
	{
          if (trix->getData(trixLoop) < trig->getData(trigLoop))
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
  TRIX *o = new TRIX;
  return ((Plugin *) o);
}


