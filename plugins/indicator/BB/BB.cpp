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

#include "BB.h"
#include <math.h>

BB::BB ()
{
  pluginName = "BB";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Deviation"), "2", Setting::Float);
  set(tr("MA Period"), "20", Setting::Integer);
  set(tr("MA Type"), tr("SMA"), Setting::MAType);
  set(tr("Plot"), tr("True"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Bollinger Bands\n";
}

BB::~BB ()
{
}

void BB::calculate ()
{
  int period = getInt(tr("MA Period"));

  double deviation = getFloat(tr("Deviation"));

  PlotLine *in = getTP();

  PlotLine *sma = getMA(in, getData(tr("MA Type")), period);
  int smaLoop = sma->getSize() - 1;

  if ((int) sma->getSize() < period * 2)
    return;

  PlotLine *bbu = new PlotLine();
  PlotLine *bbl = new PlotLine();

  int inputLoop = in->getSize() - 1;

  while (inputLoop >= period && smaLoop >= period)
  {
    int count;
    double t2 = 0;
    for (count = 0, t2 = 0; count < period; count++)
    {
      double t = in->getData(inputLoop - count) - sma->getData(smaLoop - count);
      t2 = t2 + (t * t);
    }

    double t = sqrt(t2 / period);

    bbu->prepend(sma->getData(smaLoop) + (deviation * t));
    bbl->prepend(sma->getData(smaLoop) - (deviation * t));

    inputLoop--;
    smaLoop--;
  }

  delete in;

  bbu->setColor(getData(tr("Color")));
  bbu->setType(getData(tr("Line Type")));
  bbu->setLabel(tr("BBU"));
  output.append(bbu);

  sma->setColor(getData(tr("Color")));
  sma->setType(getData(tr("Line Type")));
  sma->setLabel(tr("BBM"));
  output.append(sma);

  bbl->setColor(getData(tr("Color")));
  bbl->setType(getData(tr("Line Type")));
  bbl->setLabel(tr("BBL"));
  output.append(bbl);
}

QMemArray<int> BB::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 3)
    return alerts;

  PlotLine *bbu = output.at(0);
  PlotLine *bbl = output.at(2);

  int listLoop = data->count() - bbu->getSize() + 9;
  int bbLoop = 9;
  int status = 0;

  for (; listLoop < (int) data->count(); listLoop++, bbLoop++)
  {
    Setting *set = data->at(listLoop);
    double close = set->getFloat("Close");
    double t = close - bbl->getData(bbLoop);
    double t2 = bbu->getData(bbLoop) - bbl->getData(bbLoop);
    double t3 = t / t2;

    int loop;
    double h = 0;
    double l = 99999999;
    for (loop = 1; loop <= 9; loop++)
    {
      set = data->at(listLoop - loop);
      double high = set->getFloat("High");
      double low = set->getFloat("Low");

      if (high > h)
	h = high;

      if (low < l)
	l = low;
    }

    set = data->at(listLoop);
    double high = set->getFloat("High");
    double low = set->getFloat("Low");
    set = data->at(listLoop - 1);
    double yclose = set->getFloat("Close");

    switch (status)
    {
      case -1:
        if ((t3 < .5) && (low < l) && (close > yclose))
	  status = 1;
	break;
      case 1:
        if ((t3 > .5) && (high > h) && (close < yclose))
	  status = -1;
	break;
      default:
        if ((t3 < .5) && (low < l) && (close > yclose))
	  status = 1;
	else
	{
          if ((t3 > .5) && (high > h) && (close < yclose))
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
  BB *o = new BB;
  return ((Plugin *) o);
}


