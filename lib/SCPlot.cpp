/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "SCPlot.h"

#include <QtDebug>


SCPlot::SCPlot ()
{
}

int SCPlot::calculate (QStringList &l, QStringList &plotOrder, QHash<QString, PlotLine *> &tlines,
		       QByteArray &ba)
{
  // format 'PLOT,NAME'
  
  ba.clear();
  ba.append("ERROR\n");

  if (l.count() != 2)
  {
    qDebug() << "SCPlot::calculate: invalid parm count" << l.count();
    return 1;
  }

  PlotLine *line = tlines[l[1]];
  if (! line)
  {
    qDebug() << "SCPlot::calculate: invalid line name" << l[1];
    return 1;
  }
  
  line->setPlotFlag(TRUE);

  plotOrder.append(l[1]);

  ba.clear();
  ba.append("0\n");

  return 0;
}

