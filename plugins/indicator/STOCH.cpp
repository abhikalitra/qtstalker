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

#include "STOCH.h"

STOCH::STOCH ()
{
  pluginName = "STOCH";
  version = 0.2;

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
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Stochastic\n";
}

STOCH::~STOCH ()
{
}

void STOCH::calculate ()
{
  SettingItem *set = getItem(tr("Period"));
  int period = set->data.toInt();

  set = getItem(tr("%K Smoothing"));
  int kperiod = set->data.toInt();

  set = getItem(tr("%D Smoothing"));
  int dperiod = set->data.toInt();

  Output *k = new Output();
  k->setColor(tr("%K Color"));
  k->setType(tr("%K Line Type"));
  k->setLabel(tr("%K Label"));

  int loop;
  for (loop = period; loop < (int) data.count(); loop++)
  {
    int loop2;
    double l;
    double h;
    for (loop2 = 0, l = 9999999, h = 0; loop2 < period; loop2++)
    {
      Setting *set = data.at(loop - loop2);
      double high = set->getFloat("High");
      double low = set->getFloat("Low");

      double t = high;
      if (t > h)
        h = t;

      t = low;
      if (t < l)
        l = t;
    }

    Setting *set = data.at(loop);
    double close = set->getFloat("Close");
    double t = ((close - l) / (h - l)) * 100;
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    k->append(t);
  }

  if (kperiod)
  {
    Output *k2 = getSMA(k, kperiod);
    k2->setColor(tr("%K Color"));
    k2->setType(tr("%K Line Type"));
    k2->setLabel(tr("%K Label"));
    output.append(k2);
    delete k;
  }
  else
    output.append(k);

  k = output.at(0);

  Output *d;

  if (dperiod)
    d = getSMA(k, dperiod);
  else
  {
    d = new Output();

    int loop;
    for (loop = 0; loop < (int) k->getSize(); loop++)
      d->append(k->getData(loop));
  }

  d->setColor(tr("%D Color"));
  d->setType(tr("%D Line Type"));
  d->setLabel(tr("%D Label"));

  output.append(d);
}

QMemArray<int> STOCH::getAlerts ()
{
  alerts.fill(0, data.count());

  if (output.count() != 2)
    return alerts;

  SettingItem *set = getItem(tr("Buy Line"));
  int buy = set->data.toInt();

  set = getItem(tr("Sell Line"));
  int sell = set->data.toInt();

  Output *line = output.at(1);

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
  STOCH *o = new STOCH;
  return ((Plugin *) o);
}




