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

#include "HLC.h"

HLC::HLC ()
{
  pluginName = "HLC";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("High Color"), "red", Setting::Color);
  set(tr("Low Color"), "red", Setting::Color);
  set(tr("High Line Type"), tr("Line"), Setting::LineType);
  set(tr("Low Line Type"), tr("Line"), Setting::LineType);
  set(tr("High Label"), tr("HLCU"), Setting::Text);
  set(tr("Low Label"), tr("HLCL"), Setting::Text);
  set(tr("Period"), "20", Setting::Integer);
  set(tr("Plot"), tr("True"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "High Low Channel\n";
}

HLC::~HLC ()
{
}

void HLC::calculate ()
{
  int period = getInt(tr("Period"));

  PlotLine *ub = new PlotLine();
  PlotLine *lb = new PlotLine();

  int loop;
  for (loop = period; loop < (int) data->count(); loop++)
  {
    int loop2;
    double h = -99999999;
    double l = 99999999;
    for (loop2 = 1; loop2 <= period; loop2++)
    {
      Setting *set = data->at(loop - loop2);
      double high = set->getFloat("High");
      double low = set->getFloat("Low");

      if (high > h)
        h = high;

      if (low < l)
        l = low;
    }

    ub->append(h);
    lb->append(l);
  }

  ub->setColor(getData(tr("High Color")));
  ub->setType(getData(tr("High Line Type")));
  ub->setLabel(getData(tr("High Label")));
  output.append(ub);

  lb->setColor(getData(tr("Low Color")));
  lb->setType(getData(tr("Low Line Type")));
  lb->setLabel(getData(tr("Low Label")));
  output.append(lb);
}

QMemArray<int> HLC::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 2)
    return alerts;

  PlotLine *u = output.at(0);
  PlotLine *l = output.at(1);

  int listLoop = data->count() - u->getSize();

  int loop;
  int status = 0;
  for (loop = 0; loop < (int) u->getSize(); loop++, listLoop++)
  {
    Setting *set = data->at(listLoop);
    double close = set->getFloat("Close");

    switch (status)
    {
      case -1:
        if (close > u->getData(loop))
	  status = 1;
	break;
      case 1:
        if (close < l->getData(loop))
	  status = -1;
	break;
      default:
        if (close > u->getData(loop))
	  status = 1;
	else
	{
          if (close < l->getData(loop))
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
  HLC *o = new HLC;
  return ((Plugin *) o);
}


