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

#include "SCGetIndicator.h"

#include <QtDebug>


SCGetIndicator::SCGetIndicator ()
{
}

int SCGetIndicator::calculate (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines)
{
  // format 'GET_INDICATOR,VARIABLE,BARS' returns the requested data in a CSV string

  ba.clear();

  if (l.count() != 3)
  {
    qDebug() << "SCGetIndicator::calculate: invalid parm count" << l.count();
    return 1;
  }

  PlotLine *in = tlines.value(l[1]);
  if (! in)
  {
    qDebug() << "SCGetIndicator::calculate: input parm error" << l[1];
    return 1;
  }

  bool ok;
  int bars = l[2].toInt(&ok);
  if (! ok)
  {
    qDebug() << "SCGetIndicator::calculate: bars parm error" << l[2];
    return 1;
  }

  int loop;
  QStringList l2;
  if (bars == 0)
    loop = 0;
  else
    loop = in->getSize() - bars;
  for (; loop < in->getSize(); loop++)
    l2.append(QString::number(in->getData(loop)));

  ba.append(l2.join(","));
  ba.append('\n');

  return 0;
}

int SCGetIndicator::getIndex (QStringList &l, QHash<QString, PlotLine *> &tlines, QByteArray &ba)
{
  // format = INDEX,INPUT_ARRAY,OFFSET

  ba.clear();

  if (l.count() != 3)
  {
    qDebug() << "SCGetIndicator::getIndex: invalid parm count" << l.count();
    return 1;
  }

  PlotLine *line = tlines.value(l[1]);
  if (! line)
  {
    qDebug() << "SCGetIndicator::getIndex: invalid input" << l[1];
    return 1;
  }

  bool ok;
  int index = l[2].toInt(&ok);
  if (! ok)
  {
    qDebug() << "SCGetIndicator::getIndex: invalid index" << l[2];
    return 1;
  }

  int offset = line->getSize() - 1 - index;
  if (offset < 0)
  {
    qDebug() << "SCGetIndicator::getIndex: offset greater than" << l[1] << "size";
    return 1;
  }

  ba.append(QString::number(line->getData(offset)));

  ba.append('\n');

  return 0;
}

