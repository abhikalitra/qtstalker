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

#include "SAR.h"

SAR::SAR ()
{
  pluginName = "SAR";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "white", Setting::Color);
  set(tr("Line Type"), tr("Dot"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Initial"), "0.02", Setting::Float);
  set(tr("Add"), "0.02", Setting::Float);
  set(tr("Limit"), "0.2", Setting::Float);
  set(tr("Plot"), tr("True"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Parabolic Time/Price SAR\n";
}

SAR::~SAR ()
{
}

void SAR::calculate ()
{
  double initial = getFloat(tr("Initial"));

  double add = getFloat(tr("Add"));

  double limit = getFloat(tr("Limit"));

  PlotLine *d = new PlotLine();

  Setting *set = data->at(1);
  double high = set->getFloat("High");
  double low = set->getFloat("Low");
  set = data->at(0);
  double yhigh = set->getFloat("High");
  double ylow = set->getFloat("Low");

  int flag = 0;
  double ep = 0;
  double sar = 0;
  double psar = 0;
  double a = initial;
  double t = yhigh;
  double t2 = high;
  if (t2 > t)
  {
    // we are long
    flag = 0;
    t = ylow;
    t2 = low;
    if (t < t2)
      ep = t;
    else
      ep = t2;

    sar = ep;
    psar = ep;
  }
  else
  {
    // we are short
    flag = 1;
    t = yhigh;
    t2 = high;
    if (t > t2)
      ep = t;
    else
      ep = t2;

    sar = ep;
    psar = ep;
  }

  d->append(sar);

  int loop;
  for (loop = 2; loop < (int) data->count(); loop++)
  {
    set = data->at(loop);
    high = set->getFloat("High");
    low = set->getFloat("Low");
    set = data->at(loop - 1);
    yhigh = set->getFloat("High");
    ylow = set->getFloat("Low");

    // are we short?
    if (flag)
    {
      // check for a switch
      t = high;
      if (t >= sar)
      {
        sar = ep;
        psar = sar;
        ep = t;
        flag = 0;
        a = initial;
      }
      else
      {
        t = low;
        if (t  < ep)
        {
          ep = t;
          a = a + add;
          if (a > limit)
            a = limit;
        }

        t = psar + (a * (ep - psar));
        t2 = high;
        if (t < t2)
        {
          double t3 = yhigh;
          if (t3 > t2)
            t = t3;
          else
            t = t2;
        }
        psar = sar;
        sar = t;
      }
    }
    else
    {
      // we are long
      // check for a switch

      t = low;
      if (t <= sar)
      {
        sar = ep;
        psar = sar;
        ep = t;
        flag = 1;
        a = initial;
      }
      else
      {
        t = high;
        if (t  > ep)
        {
          ep = t;
          a = a + add;
          if (a > limit)
            a = limit;
        }

        t = psar + (a * (ep - psar));
        t2 = low;
        if (t > t2)
        {
          double t3 = ylow;
          if (t3 < t2)
            t = t3;
          else
            t = t2;
        }
        psar = sar;
        sar = t;
      }
    }

    d->append(sar);
  }

  d->setColor(getData(tr("Color")));
  d->setType(getData(tr("Line Type")));
  d->setLabel(getData(tr("Label")));
  output.append(d);
}

QMemArray<int> SAR::getAlerts ()
{
  alerts.fill(0, data->count());

  if (! output.count())
    return alerts;

  PlotLine *line = output.at(0);
  int lineLoop;
  int listLoop = data->count() - line->getSize();
  int status = 0;
  for (lineLoop = 0; lineLoop < (int) line->getSize(); lineLoop++, listLoop++)
  {
    Setting *set = data->at(listLoop);
    double high = set->getFloat("High");
    double low = set->getFloat("Low");

    switch (status)
    {
      case -1:
        if (line->getData(lineLoop) > high)
	  status = 1;
	break;
      case 1:
        if (line->getData(lineLoop) < low)
	  status = -1;
	break;
      default:
        if (line->getData(lineLoop) > high)
	  status = 1;
	else
	{
          if (line->getData(lineLoop) < low)
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
  SAR *o = new SAR;
  return ((Plugin *) o);
}





