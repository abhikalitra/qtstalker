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

#include "CCI.h"
#include <math.h>

CCI::CCI ()
{
  pluginName = "CCI";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "20", Setting::Integer);
  set(tr("Smoothing"), "3", Setting::Integer);
  set(tr("Smoothing Type"), tr("SMA"), Setting::MAType);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  QStringList l;
  l.append(tr("100 Rule"));
  l.append(tr("0 Rule"));
  l.sort();
  set(tr("Alert Type"), tr("100 Rule"), Setting::List);
  setList(tr("Alert Type"), l);

  about = "Commodity Channel Index with optional MA\n";
}

CCI::~CCI ()
{
}

void CCI::calculate ()
{
  int period = getInt(tr("Period"));

  PlotLine *tp = getTP();
  int tpLoop = tp->getSize() - 1;

  PlotLine *sma = getSMA(tp, period);
  int smaLoop = sma->getSize() - 1;

  PlotLine *cci = new PlotLine();

  while (tpLoop >= period && smaLoop >= period)
  {
    double md = 0;
    int loop;
    for (loop = 0; loop < period; loop++)
      md = md + fabs(tp->getData(tpLoop - loop) - sma->getData(smaLoop - loop));
    md = md / period;

    double t = (tp->getData(tpLoop) - sma->getData(smaLoop)) / (0.015 * md);
    cci->prepend(t);

    tpLoop--;
    smaLoop--;
  }

  period = getInt(tr("Smoothing"));
  if (period > 1)
  {
    PlotLine *ma = getMA(cci, getData(tr("Smoothing Type")), period);
    ma->setColor(getData(tr("Color")));
    ma->setType(getData(tr("Line Type")));
    ma->setLabel(getData(tr("Label")));
    output.append(ma);
    delete cci;
  }
  else
  {
    cci->setColor(getData(tr("Color")));
    cci->setType(getData(tr("Line Type")));
    cci->setLabel(getData(tr("Label")));
    output.append(cci);
  }

  delete tp;
  delete sma;
}

QMemArray<int> CCI::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() == 0)
    return alerts;

  QString s = getData(tr("Alert Type"));
  if (! s.compare(tr("100 Rule")))
    alertHundred();

  if (! s.compare(tr("0 Rule")))
    alertZero();

  return alerts;
}

void CCI::alertHundred ()
{
  PlotLine *cci = output.at(0);

  int dataLoop = data->count() - cci->getSize();
  int loop;
  int status = 0;
  for (loop = 0; loop < (int) cci->getSize(); loop++, dataLoop++)
  {
    switch (status)
    {
      case -1:
        if (cci->getData(loop) > -100)
          status = 0;
        break;
      case 1:
        if (cci->getData(loop) < 100)
	  status = 0;
	break;
      default:
        if (cci->getData(loop) > 100)
	  status = 1;
	else
	{
          if (cci->getData(loop) < -100)
	    status = -1;
	}
	break;
    }

    alerts[dataLoop] = status;
  }
}

void CCI::alertZero ()
{
  PlotLine *cci = output.at(0);

  int dataLoop = data->count() - cci->getSize();
  int status = 0;
  int loop;
  for (loop = 0; loop < (int) cci->getSize(); loop++, dataLoop++)
  {
    switch (status)
    {
      case -1:
        if (cci->getData(loop) > 0)
          status = 0;
        break;
      case 1:
        if (cci->getData(loop) < 0)
	  status = 0;
	break;
      default:
        if (cci->getData(loop) > 0)
	  status = 1;
	else
	{
          if (cci->getData(loop) < 0)
	    status = -1;
	}
	break;
    }

    alerts[dataLoop] = status;
  }
}

Plugin * create ()
{
  CCI *o = new CCI;
  return ((Plugin *) o);
}





