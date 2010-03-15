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

#include "SCTest.h"

#include <QtDebug>


SCTest::SCTest ()
{
}

PlotLine * SCTest::getSig (QStringList &l, QHash<QString, PlotLine *> &tlines, QByteArray &ba)
{
  // format TEST_ENTER_LONG,INPUT
  // format TEST_EXIT_LONG,INPUT
  // format TEST_ENTER_SHORT,INPUT
  // format TEST_EXIT_SHORT,INPUT

  ba.clear();
  ba.append("1\n");

  if (l.count() != 2)
  {
    qDebug() << "SCTest::enterLong: invalid parm count" << l.count();
    return 0;
  }

  PlotLine *line = tlines[l[1]];
  if (! line)
  {
    qDebug() << "SCTest::enterLong: invalid input" << l[1];
    return 0;
  }

  ba.clear();
  ba.append("0\n");

  return line;
}

