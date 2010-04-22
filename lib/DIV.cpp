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

#include "DIV.h"

#include <QtDebug>

DIV::DIV ()
{
  indicator = "DIV";
  deleteFlag = TRUE;
}

int DIV::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,DIV,<NAME>,<INPUT>,<INPUT2>
  //     0       1     2    3      4        5

  if (set.count() != 6)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  PlotLine *in = tlines.value(set[4]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[4]));
    if (! in)
    {
      qDebug() << indicator << "::getCUS: input not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in);
  }

  PlotLine *in2 = tlines.value(set[5]);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(set[5]));
    if (! in2)
    {
      qDebug() << indicator << "::getCUS: input2 not found" << set[5];
      return 1;
    }

    tlines.insert(set[5], in2);
  }

  PlotLine *line = getDIV(in, in2);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * DIV::getDIV (PlotLine *in, PlotLine *in2)
{
  if (in->count() < 1 || in2->count() < 1)
    return 0;
  
  PlotLine *line = new PlotLine;

  int inLoop = in->count() - 1;
  int in2Loop = in2->count() - 1;

  while (inLoop > -1 && in2Loop > -1)
  {
    line->prepend(in->getData(inLoop) / in2->getData(in2Loop));
    inLoop--;
    in2Loop--;
  }
  
  return line;
}

