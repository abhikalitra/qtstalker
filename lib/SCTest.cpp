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

#include "SCTest.h"

#include <QtDebug>


SCTest::SCTest ()
{
}

void SCTest::getSig (QStringList &l, BarData *data, QHash<QString, PlotLine *> &tlines, QByteArray &ba,
			QHash<int, int> &sig)
{
  // format TEST_ENTER_LONG,INPUT
  // format TEST_EXIT_LONG,INPUT
  // format TEST_ENTER_SHORT,INPUT
  // format TEST_EXIT_SHORT,INPUT

  ba.clear();
  ba.append("1\n");
  sig.clear();

  if (l.count() != 2)
  {
    qDebug() << "SCTest::enterLong: invalid parm count" << l.count();
    return;
  }

  PlotLine *line = tlines[l[1]];
  if (! line)
  {
    qDebug() << "SCTest::enterLong: invalid input" << l[1];
    return;
  }

  int barLoop = data->count() - line->getSize() - 1;
  if (barLoop < 0)
  {
    qDebug() << "SCTest::enterLong:" << line->getSize() << "size > bars size";
    return;
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
	  sig.insert(barLoop, lineLoop);
	}
	break;
    }
  }

  ba.clear();
  ba.append("0\n");
}

