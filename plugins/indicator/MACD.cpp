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

#include "MACD.h"

MACD::MACD ()
{
  pluginName = "MACD";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("MACD Color"), "red", Setting::Color);
  set(tr("Trigger Color"), "yellow", Setting::Color);
  set(tr("Oscillator Color"), "blue", Setting::Color);
  set(tr("MACD Line Type"), tr("Line"), Setting::LineType);
  set(tr("Trigger Line Type"), tr("Dash"), Setting::LineType);
  set(tr("Oscillator Line Type"), tr("Histogram"), Setting::LineType);
  set(tr("MACD Label"), pluginName, Setting::Text);
  set(tr("Trigger Label"), tr("MACD Trigger"), Setting::Text);
  set(tr("Oscillator Label"), tr("MACD Oscillator"), Setting::Text);
  set(tr("Input"), tr("Close"), Setting::InputField);
  set(tr("Period"), "9", Setting::Integer);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Moving Average Convergence Divergence\n";
}

MACD::~MACD ()
{
}

void MACD::calculate ()
{
  SettingItem *set = getItem(tr("Input"));
  Output *d = getInput(set->data);

  Output *slow = getEMA(d, 26);
  if (slow->getSize() == 0)
    return;

  Output *fast = getEMA(d, 12);

  Output *macd = new Output();
  macd->setColor(tr("MACD Color"));
  macd->setType(tr("MACD Line Type"));
  macd->setLabel(tr("MACD Label"));

  int floop = fast->getSize() - 1;
  int sloop = slow->getSize() - 1;

  while (floop > -1 && sloop > -1)
  {
    macd->prepend(fast->getData(floop) - slow->getData(sloop));
    floop--;
    sloop--;
  }

  set = getItem(tr("Period"));
  int period = set->data.toInt();

  Output *trigger = getEMA(macd, period);
  trigger->setColor(tr("Trigger Color"));
  trigger->setType(tr("Trigger Line Type"));
  trigger->setLabel(tr("Trigger Label"));

  Output *osc = new Output();
  osc->setColor(tr("Oscillator Color"));
  osc->setType(tr("Oscillator Line Type"));
  osc->setLabel(tr("Oscillator Label"));

  floop = macd->getSize() - 1;
  sloop = trigger->getSize() - 1;

  while (floop > -1 && sloop > -1)
  {
    osc->prepend(macd->getData(floop) - trigger->getData(sloop));
    floop--;
    sloop--;
  }

  output.append(osc);
  output.append(macd);
  output.append(trigger);

  delete d;
  delete slow;
  delete fast;
}

QMemArray<int> MACD::getAlerts ()
{
  alerts.fill(0, data.count());

  if (output.count() != 3)
    return alerts;

  Output *macd = output.at(1);
  Output *trig = output.at(2);

  int listLoop = data.count() - trig->getSize();
  int macdLoop = macd->getSize() - trig->getSize();
  int trigLoop;
  int status = 0;
  for (trigLoop = 0; trigLoop < (int) trig->getSize(); trigLoop++, macdLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if (macd->getData(macdLoop) > trig->getData(trigLoop))
	  status = 1;
	break;
      case 1:
        if (macd->getData(macdLoop) < trig->getData(trigLoop))
	  status = -1;
	break;
      default:
        if (macd->getData(macdLoop) > trig->getData(trigLoop))
	  status = 1;
	else
	{
          if (macd->getData(macdLoop) < trig->getData(trigLoop))
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
  MACD *o = new MACD;
  return ((Plugin *) o);
}

