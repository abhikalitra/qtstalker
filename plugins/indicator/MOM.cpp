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

#include "MOM.h"

MOM::MOM ()
{
  pluginName = "MOM";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Histogram"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "14", Setting::Integer);
  set(tr("Input"), tr("Close"), Setting::InputField);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Momentum\n";
}

MOM::~MOM ()
{
}

void MOM::calculate ()
{
  SettingItem *set = getItem(tr("Period"));
  int period = set->data.toInt();

  set = getItem(tr("Input"));
  Output *in = getInput(set->data);

  Output *mom = new Output();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
    mom->append(in->getData(loop) - in->getData(loop - period));

  output.append(mom);

  delete in;
}

QMemArray<int> MOM::getAlerts ()
{
  alerts.fill(0, data.count());

  if (! output.count())
    return alerts;

  Output *line = output.at(0);

  int lineLoop;
  int listLoop = data.count() - line->getSize();
  int status = 0;
  for (lineLoop = 0; lineLoop < (int) line->getSize(); lineLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if (line->getData(lineLoop) > 0)
	  status = 1;
	break;
      case 1:
        if (line->getData(lineLoop) < 0)
	  status = -1;
	break;
      default:
        if (line->getData(lineLoop) > 0)
	  status = 1;
	else
	{
          if (line->getData(lineLoop) < 0)
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
  MOM *o = new MOM;
  return ((Plugin *) o);
}

