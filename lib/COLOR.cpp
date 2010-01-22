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

#include "COLOR.h"

#include <QtDebug>


COLOR::COLOR ()
{
}

int COLOR::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,COLOR,<INPUT>,<INPUT_2>,<VALUE>,<COLOR>

  if (set.count() != 6)
  {
    qDebug() << "COLOR::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *in = tlines.value(set[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[2]));
    if (! in)
    {
      qDebug() << set[1] << "::calculate: input not found" << set[2];
      return 1;
    }

    tlines.insert(set[2], in);
  }

  PlotLine *in2 = tlines.value(set[3]);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(set[3]));
    if (! in2)
    {
      qDebug() << set[1] << "::calculate: input2 not found" << set[3];
      return 1;
    }

    tlines.insert(set[3], in2);
  }

  bool ok = FALSE;
  double value = set[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "COLOR::calculate: invalid value" << set[4];
    return 1;
  }

  QColor c(set[5]);
  if (! c.isValid())
  {
    qDebug() << "COLOR::calculate: invalid color" << set[4];
    return 1;
  }

  int inboolLoop = in->getSize() - 1;
  in2->setColorFlag(TRUE);
  int incolLoop = in2->getSize() - 1;

  while (inboolLoop > -1 && incolLoop > -1)
  {
    if (in->getData(inboolLoop) == value)
      in2->setColorBar(incolLoop, c);

    inboolLoop--;
    incolLoop--;
  }

  return 0;
}

