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

#include "COLOR.h"

#include <QtDebug>


COLOR::COLOR ()
{
  indicator = "COLOR";
  deleteFlag = TRUE;
  
  methodList << "All" << "Compare";
}

int COLOR::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *)
{
  // INDICATOR,PLUGIN,COLOR,METHOD,*

  if (set.count() < 4)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }
  
  int rc = 1;
  switch (methodList.indexOf(set[3]))
  {
    case 0:
      rc = getAll(set, tlines);
      break;
    case 1:
      rc = getCompare(set, tlines);
      break;
    default:
      break;
  }
  
  return rc;
}

int COLOR::getCompare (QStringList &set, QHash<QString, PlotLine *> &tlines)
{
  // INDICATOR,PLUGIN,COLOR,METHOD,<INPUT>,<INPUT_2>,<VALUE>,<COLOR>

  if (set.count() != 8)
  {
    qDebug() << indicator << "::getCompare: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *in = tlines.value(set[4]);
  if (! in)
  {
    qDebug() << indicator << "::getCompare: input not found" << set[4];
    return 1;
  }

  PlotLine *in2 = tlines.value(set[5]);
  if (! in2)
  {
    qDebug() << indicator << "::getCompare: input2 not found" << set[5];
    return 1;
  }

  bool ok = FALSE;
  double value = set[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCompare: invalid value" << set[6];
    return 1;
  }

  QColor c(set[7]);
  if (! c.isValid())
  {
    qDebug() << indicator << "::getCompare: invalid color" << set[7];
    return 1;
  }

  int inboolLoop = in->count() - 1;
  int incolLoop = in2->count() - 1;

  while (inboolLoop > -1 && incolLoop > -1)
  {
    if (in->getData(inboolLoop) == value)
      in2->setColorBar(incolLoop, c);

    inboolLoop--;
    incolLoop--;
  }

  return 0;
}

int COLOR::getAll (QStringList &set, QHash<QString, PlotLine *> &tlines)
{
  // INDICATOR,PLUGIN,COLOR,METHOD,<INPUT>,<COLOR>

  if (set.count() != 6)
  {
    qDebug() << indicator << "::getAll: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *in = tlines.value(set[4]);
  if (! in)
  {
    qDebug() << indicator << "::getAll: input not found" << set[4];
    return 1;
  }

  QColor c(set[5]);
  if (! c.isValid())
  {
    qDebug() << indicator << "::getAll: invalid color" << set[5];
    return 1;
  }

  in->setColor(c);

  return 0;
}

