/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "TestSignal.h"

#include <QDebug>

TestSignal::TestSignal ()
{
}

int TestSignal::createSignals (BarData *data, PlotLine *line)
{
  int barLoop = data->count() - line->getSize();
  if (barLoop < 0)
  {
    qDebug() << "IndicatorBase::createSignals" << line->getSize() << "size > bars size";
    return 1;
  }

  int status = 0;
  int lineLoop;
  for (lineLoop = 0; lineLoop < line->getSize(); lineLoop++, barLoop++)
  {
    switch (status)
    {
      case 1: // we are inside a signal
        if (line->getData(lineLoop) == 0)
          status = 0;
	break;
      default: // we are outside a signal
        if (line->getData(lineLoop) == 1)
        {
          status = 1;
	  sigs.insert(barLoop, barLoop);
	}
	break;
    }
  }

  return 0;
}

int TestSignal::getBar (int i)
{
  int rc = 0;
  if (sigs.contains(i))
    rc = sigs.value(i);
  return rc;
}

