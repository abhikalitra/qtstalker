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
  set(tr("Fast Label"), tr("MA2 Fast"), Setting::Text);
  set(tr("Fast Period"), "5", Setting::Integer);
  set(tr("Fast Type"), "SMA", Setting::MAType);
  set(tr("Middle Color"), "red", Setting::Color);
  set(tr("Middle Line Type"), tr("Line"), Setting::LineType);
  set(tr("Middle Label"), tr("MA2 Middle"), Setting::Text);
  set(tr("Middle Period"), "20", Setting::Integer);
  set(tr("Middle Type"), "SMA", Setting::MAType);
  set(tr("Slow Color"), "red", Setting::Color);
  set(tr("Slow Line Type"), tr("Line"), Setting::LineType);
  set(tr("Slow Label"), tr("MA2 Slow"), Setting::Text);
  set(tr("Slow Period"), "40", Setting::Integer);
  set(tr("Slow Type"), "SMA", Setting::MAType);
  set(tr("Input"), tr("Close"), Setting::InputField);
  set(tr("Plot"), tr("True"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "3 Plot Moving Average\n";
}

MA3::~MA3 ()
{
}

void MA3::calculate ()
{
  SettingItem *set = getItem(tr("Input"));
  Output *in = getInput(set->data);

  set = getItem(tr("Fast Period"));
  int fperiod = set->data.toInt();

  set = getItem(tr("Middle Period"));
  int mperiod = set->data.toInt();

  set = getItem(tr("Slow Period"));
  int speriod = set->data.toInt();

  set = getItem(tr("Fast Type"));
  Output *fma = getMA(in, set->data, fperiod);
  fma->setColor(tr("Fast Color"));
  fma->setType(tr("Fast Line Type"));
  fma->setLabel(tr("Fast Label"));

  set = getItem(tr("Middle Type"));
  Output *mma = getMA(in, set->data, mperiod);
  mma->setColor(tr("Middle Color"));
  mma->setType(tr("Middle Line Type"));
  mma->setLabel(tr("Middle Label"));

  set = getItem(tr("Slow Type"));
  Output *sma = getMA(in, set->data, speriod);
  sma->setColor(tr("Slow Color"));
  sma->setType(tr("Slow Line Type"));
  sma->setLabel(tr("Slow Label"));

  delete in;

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
  alerts.fill(0, data.count());

  if (output.count() != 3)
    return alerts;

  Output *fma = output.at(0);
  Output *mma = output.at(1);
  Output *sma = output.at(2);

  int listLoop = data.count();
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
  for (; listLoop < (int) data.count(); fmaLoop++, mmaLoop++, smaLoop++, listLoop++)
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


