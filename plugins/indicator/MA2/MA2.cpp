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

#include "MA2.h"

MA2::MA2 ()
{
  pluginName = "MA2";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Fast Color"), "red", Setting::Color);
  set(tr("Fast Line Type"), tr("Line"), Setting::LineType);
  set(tr("Fast Label"), tr("MA2F"), Setting::Text);
  set(tr("Fast Period"), "5", Setting::Integer);
  set(tr("Fast Displace"), "0", Setting::Integer);
  set(tr("Fast Type"), "SMA", Setting::MAType);
  set(tr("Fast Input"), tr("Close"), Setting::InputField);
  set(tr("Slow Color"), "red", Setting::Color);
  set(tr("Slow Line Type"), tr("Line"), Setting::LineType);
  set(tr("Slow Label"), tr("MA2S"), Setting::Text);
  set(tr("Slow Period"), "20", Setting::Integer);
  set(tr("Slow Displace"), "0", Setting::Integer);
  set(tr("Slow Type"), "SMA", Setting::MAType);
  set(tr("Slow Input"), tr("Close"), Setting::InputField);
  set(tr("Plot"), tr("True"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "2 Plot Moving Average\n";
}

MA2::~MA2 ()
{
}

void MA2::calculate ()
{
  PlotLine *fin = getInput(getData(tr("Fast Input")));
  PlotLine *sin = getInput(getData(tr("Slow Input")));

  PlotLine *fma = getMA(fin, getData(tr("Fast Type")), getInt(tr("Fast Period")), getInt(tr("Fast Displace")));
  fma->setColor(getData(tr("Fast Color")));
  fma->setType(getData(tr("Fast Line Type")));
  fma->setLabel(getData(tr("Fast Label")));

  PlotLine *sma = getMA(sin, getData(tr("Slow Type")), getInt(tr("Slow Period")), getInt(tr("Slow Displace")));
  sma->setColor(getData(tr("Slow Color")));
  sma->setType(getData(tr("Slow Line Type")));
  sma->setLabel(getData(tr("Slow Label")));

  delete fin;
  delete sin;

  if (fma->getSize())
    output.append(fma);
  else
    delete fma;

  if (sma->getSize())
    output.append(sma);
  else
    delete sma;
}

QMemArray<int> MA2::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 2)
    return alerts;

  PlotLine *fma = output.at(0);
  PlotLine *sma = output.at(1);

  int listLoop = data->count();
  int fmaLoop = fma->getSize();
  int smaLoop = sma->getSize();
  while (listLoop != 0 && fmaLoop != 0 && smaLoop != 0)
  {
    listLoop--;
    fmaLoop--;
    smaLoop--;
  }

  int status = 0;
  for (; listLoop < (int) data->count(); fmaLoop++, smaLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if (fma->getData(fmaLoop) > sma->getData(smaLoop))
	  status = 1;
	break;
      case 1:
        if (fma->getData(fmaLoop) < sma->getData(smaLoop))
	  status = -1;
	break;
      default:
        if (fma->getData(fmaLoop) > sma->getData(smaLoop))
	  status = 1;
	else
	{
          if (fma->getData(fmaLoop) < sma->getData(smaLoop))
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
  MA2 *o = new MA2;
  return ((Plugin *) o);
}


