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

#include "MA3.h"

MA3::MA3 ()
{
  pluginName = "MA3";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Fast Color"), "red", Setting::Color);
  set(tr("Fast Line Type"), tr("Line"), Setting::LineType);
  set(tr("Fast Label"), tr("MA3F"), Setting::Text);
  set(tr("Fast Period"), "5", Setting::Integer);
  set(tr("Fast Type"), "SMA", Setting::MAType);
  set(tr("Fast Input"), tr("Close"), Setting::InputField);
  set(tr("Middle Color"), "red", Setting::Color);
  set(tr("Middle Line Type"), tr("Line"), Setting::LineType);
  set(tr("Middle Label"), tr("MA3M"), Setting::Text);
  set(tr("Middle Period"), "20", Setting::Integer);
  set(tr("Middle Type"), "SMA", Setting::MAType);
  set(tr("Middle Input"), tr("Close"), Setting::InputField);
  set(tr("Slow Color"), "red", Setting::Color);
  set(tr("Slow Line Type"), tr("Line"), Setting::LineType);
  set(tr("Slow Label"), tr("MA3S"), Setting::Text);
  set(tr("Slow Period"), "40", Setting::Integer);
  set(tr("Slow Type"), "SMA", Setting::MAType);
  set(tr("Slow Input"), tr("Close"), Setting::InputField);
  set(tr("Plot"), tr("True"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "3 Plot Moving Average\n";
}

MA3::~MA3 ()
{
}

void MA3::calculate ()
{
  PlotLine *fin = getInput(getData(tr("Fast Input")));
  PlotLine *min = getInput(getData(tr("Middle Input")));
  PlotLine *sin = getInput(getData(tr("Slow Input")));

  int fperiod = getInt(tr("Fast Period"));
  int mperiod = getInt(tr("Middle Period"));
  int speriod = getInt(tr("Slow Period"));

  PlotLine *fma = getMA(fin, getData(tr("Fast Type")), fperiod);
  fma->setColor(getData(tr("Fast Color")));
  fma->setType(getData(tr("Fast Line Type")));
  fma->setLabel(getData(tr("Fast Label")));

  PlotLine *mma = getMA(min, getData(tr("Middle Type")), mperiod);
  mma->setColor(getData(tr("Middle Color")));
  mma->setType(getData(tr("Middle Line Type")));
  mma->setLabel(getData(tr("Middle Label")));

  PlotLine *sma = getMA(sin, getData(tr("Slow Type")), speriod);
  sma->setColor(getData(tr("Slow Color")));
  sma->setType(getData(tr("Slow Line Type")));
  sma->setLabel(getData(tr("Slow Label")));

  delete fin;
  delete min;
  delete sin;

  if (fma->getSize())
    output.append(fma);
  else
    delete fma;

  if (mma->getSize())
    output.append(mma);
  else
    delete mma;

  if (sma->getSize())
    output.append(sma);
  else
    delete sma;
}

QMemArray<int> MA3::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 3)
    return alerts;

  PlotLine *fma = output.at(0);
  PlotLine *mma = output.at(1);
  PlotLine *sma = output.at(2);

  int listLoop = data->count();
  int fmaLoop = fma->getSize();
  int mmaLoop = mma->getSize();
  int smaLoop = sma->getSize();
  while (listLoop != 0 && fmaLoop != 0 && mmaLoop != 0 &&  smaLoop != 0)
  {
    listLoop--;
    fmaLoop--;
    mmaLoop--;
    smaLoop--;
  }

  int status = 0;
  for (; listLoop < (int) data->count(); fmaLoop++, mmaLoop++, smaLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if (fma->getData(fmaLoop) > mma->getData(mmaLoop))
	  status = 0;
	break;
      case 1:
        if (fma->getData(fmaLoop) < mma->getData(mmaLoop))
	  status = 0;
	break;
      default:
        if ((fma->getData(fmaLoop) > mma->getData(mmaLoop)) && (mma->getData(mmaLoop) > sma->getData(smaLoop)))
	  status = 1;
	else
	{
          if ((fma->getData(fmaLoop) < mma->getData(mmaLoop)) && (mma->getData(mmaLoop) < sma->getData(smaLoop)))
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
  MA3 *o = new MA3;
  return ((Plugin *) o);
}

