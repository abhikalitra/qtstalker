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

#include "STOCH.h"

STOCH::STOCH ()
{
  pluginName = "STOCH";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("%D Color"), "yellow", Setting::Color);
  set(tr("%D Line Type"), tr("Dash"), Setting::LineType);
  set(tr("%D Label"), "%D", Setting::Text);
  set(tr("%D Smoothing"), "3", Setting::Integer);
  set(tr("%K Color"), "red", Setting::Color);
  set(tr("%K Line Type"), tr("Line"), Setting::LineType);
  set(tr("%K Label"), "%K", Setting::Text);
  set(tr("%K Smoothing"), "3", Setting::Integer);
  set(tr("Period"), "14", Setting::Integer);
  set(tr("Buy Line"), "20", Setting::Integer);
  set(tr("Sell Line"), "80", Setting::Integer);
  set(tr("Smoothing Type"), tr("SMA"), Setting::MAType);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Stochastic\n";
}

STOCH::~STOCH ()
{
}

void STOCH::calculate ()
{
  int period = getInt(tr("Period"));

  int kperiod = getInt(tr("%K Smoothing"));

  int dperiod = getInt(tr("%D Smoothing"));

  PlotLine *k = new PlotLine();
  k->setColor(getData(tr("%K Color")));
  k->setType(getData(tr("%K Line Type")));
  k->setLabel(getData(tr("%K Label")));

  int loop;
  for (loop = period; loop < (int) data->count(); loop++)
  {
    int loop2;
    double l;
    double h;
    for (loop2 = 0, l = 9999999, h = 0; loop2 < period; loop2++)
    {
      double high = data->getHigh(loop - loop2);
      double low = data->getLow(loop - loop2);

      double t = high;
      if (t > h)
        h = t;

      t = low;
      if (t < l)
        l = t;
    }

    double close = data->getClose(loop);
    double t = ((close - l) / (h - l)) * 100;
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    k->append(t);
  }

  if (kperiod > 1)
  {
    PlotLine *k2 = getMA(k, getData(tr("Smoothing Type")), kperiod);
    k2->setColor(getData(tr("%K Color")));
    k2->setType(getData(tr("%K Line Type")));
    k2->setLabel(getData(tr("%K Label")));
    output.append(k2);
    delete k;
  }
  else
    output.append(k);

  k = output.at(0);

  PlotLine *d;

  if (dperiod)
    d = getMA(k, getData(tr("Smoothing Type")), dperiod);
  else
  {
    d = new PlotLine();

    int loop;
    for (loop = 0; loop < (int) k->getSize(); loop++)
      d->append(k->getData(loop));
  }

  d->setColor(getData(tr("%D Color")));
  d->setType(getData(tr("%D Line Type")));
  d->setLabel(getData(tr("%D Label")));

  output.append(d);
}

QMemArray<int> STOCH::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 2)
    return alerts;

  int buy = getInt(tr("Buy Line"));

  int sell = getInt(tr("Sell Line"));

  PlotLine *line = output.at(1);

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
  STOCH *o = new STOCH;
  return ((Plugin *) o);
}




