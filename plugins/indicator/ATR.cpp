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

#include "ATR.h"

ATR::ATR ()
{
  pluginName = "ATR";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "14", Setting::Integer);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Average True Range\n";
}

ATR::~ATR ()
{
}

void ATR::calculate ()
{
  Output *line = getTR();

  SettingItem *set = getItem(tr("Period"));

  Output *data = getSMA(line, set->data.toInt());

  delete line;

  output.append(data);
}

QMemArray<int> ATR::getAlerts ()
{
  alerts.fill(0, data.count());

  if (! output.count())
    return alerts;

  Output *atr = output.at(0);
  int atrLoop;
  int listLoop = data.count() - atr->getSize() + 18;
  int status = 0;

  for (atrLoop = 18; atrLoop < (int) atr->getSize(); atrLoop++, listLoop++)
  {
    int loop;
    double h = 0;
    double l = 99999999;
    for (loop = 0; loop < 18; loop++)
    {
      if (atr->getData(atrLoop - loop) > h)
	  h = atr->getData(atrLoop - loop);

      if (atr->getData(atrLoop - loop) < l)
        l = atr->getData(atrLoop - loop);
    }

    switch (status)
    {
      case -1:
        if (atr->getData(atrLoop) <= l)
	  status = 1;
	break;
      case 1:
        if (atr->getData(atrLoop) >= h)
	  status = -1;
	break;
      default:
        if (atr->getData(atrLoop) <= l)
	  status = 1;
	else
	{
          if (atr->getData(atrLoop) >= h)
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
  ATR *o = new ATR;
  return ((Plugin *) o);
}




