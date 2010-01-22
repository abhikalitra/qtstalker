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

#include "SCSetIndicator.h"

#include <QtDebug>


SCSetIndicator::SCSetIndicator ()
{
}

int SCSetIndicator::calculate (QStringList &l, QHash<QString, PlotLine *> &tlines)
{
  // format 'SET_INDICATOR,VARIABLE,CSV,DATA,FROM,NOW,ON' - will create a new line using the provided data

  if (l.count() < 3)
  {
    qDebug() << "SCSetIndicator::calculate: invalid parm count " << l.count();
    return 1;
  }

  PlotLine *line = tlines.value(l[1]);
  if (line)
  {
    // variable already used, abort
    qDebug() << "SCSetIndicator::calculate: duplicate variable name" << l[1];
    return 1;
  }

  line = new PlotLine;
  int loop;
  for (loop = 2; loop < l.count(); loop++)
    line->append(l[loop].toDouble());

  tlines.insert(l[1], line);

  return 0;
}

