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

#include "CCI.h"
#include <math.h>

CCI::CCI ()
{
  pluginName = "CCI";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "20", Setting::Integer);
  set(tr("Buy Line"), "0", Setting::Integer);
  set(tr("Sell Line"), "0", Setting::Integer);

  set(tr("MA Color"), "yellow", Setting::Color);
  set(tr("MA Line Type"), tr("Line"), Setting::LineType);
  set(tr("MA Label"), tr("MA CCI"), Setting::Text);
  set(tr("MA Period"), "20", Setting::Integer);
  set(tr("MA Type"), tr("SMA"), Setting::MAType);

  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Commodity Channel Index with optional MA\n";
}

CCI::~CCI ()
{
}

void CCI::calculate ()
{
  SettingItem *set = getItem(tr("Period"));
  int period = set->data.toInt();

  Output *tp = getTP();
  int tpLoop = tp->getSize() - 1;

  Output *sma = getSMA(tp, period);
  int smaLoop = sma->getSize() - 1;

  Output *cci = new Output();

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

  output.append(cci);

  set = getItem(tr("MA Period"));
  period = set->data.toInt();
  if (period)
  {
    set = getItem(tr("MA Type"));
    Output *ma = getMA(cci, set->data, period);
    ma->setColor(tr("MA Color"));
    ma->setType(tr("MA Line Type"));
    ma->setLabel(tr("MA Label"));
    output.append(ma);
  }

  delete tp;
  delete sma;
}

QMemArray<int> CCI::getAlerts ()
{
  alerts.fill(0, data.count());

  if (output.count() == 0)
    return alerts;

  SettingItem *set = getItem(tr("Buy Line"));
  int buy = set->data.toInt();

  set = getItem(tr("Sell Line"));
  int sell = set->data.toInt();

  if (output.count() == 1)
  {
    Output *cci = output.at(0);

    int listLoop = data.count() - cci->getSize();
    int cciLoop;
    int status = 0;
    for (cciLoop = 0; cciLoop < (int) cci->getSize(); cciLoop++, listLoop++)
    {
      switch (status)
      {
        case -1:
          if (cci->getData(cciLoop) > buy)
	    status = 1;
	  break;
        case 1:
          if (cci->getData(cciLoop) < sell)
	    status = -1;
	  break;
        default:
          if (cci->getData(cciLoop) > buy)
	    status = 1;
	  else
	  {
            if (cci->getData(cciLoop) < sell)
	      status = -1;
	  }
	  break;
      }

      alerts[listLoop] = status;
    }
  }
  else
  {
    if (output.count() == 2)
    {
      Output *cci = output.at(0);
      Output *ma = output.at(1);

      int maLoop;
      int cciLoop = cci->getSize() - ma->getSize();
      int listLoop = data.count() - ma->getSize();
      int status = 0;
      for (maLoop = 0; maLoop < (int) ma->getSize(); maLoop++, cciLoop++, listLoop++)
      {
        switch (status)
        {
          case -1:
            if ((cci->getData(cciLoop) > ma->getData(maLoop)) && (cci->getData(cciLoop) > buy))
	      status = 1;
	    break;
          case 1:
            if ((cci->getData(cciLoop) < ma->getData(maLoop)) && (cci->getData(cciLoop) < sell))
	      status = -1;
	    break;
          default:
            if ((cci->getData(cciLoop) > ma->getData(maLoop)) && (cci->getData(cciLoop) > buy))
	      status = 1;
	    else
	    {
              if ((cci->getData(cciLoop) < ma->getData(maLoop)) && (cci->getData(cciLoop) < sell))
	        status = -1;
	    }
	    break;
        }
	
        alerts[listLoop] = status;
      }
    }
  }

  return alerts;
}

Plugin * create ()
{
  CCI *o = new CCI;
  return ((Plugin *) o);
}





