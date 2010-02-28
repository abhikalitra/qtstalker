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

#include "SCIndicator.h"

#include <QtDebug>


SCIndicator::SCIndicator ()
{
}

int SCIndicator::getIndicator (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines)
{
  // format 'INDICATOR_GET,VARIABLE,BARS' returns the requested data in a CSV string

  ba.clear();
  ba.append("ERROR\n");

  if (l.count() != 3)
  {
    qDebug() << "SCIndicator::getIndicator: invalid parm count" << l.count();
    return 1;
  }

  PlotLine *in = tlines.value(l[1]);
  if (! in)
  {
    qDebug() << "SCIndicator::getIndicator: input parm error" << l[1];
    return 1;
  }

  bool ok;
  int bars = l[2].toInt(&ok);
  if (! ok)
  {
    qDebug() << "SCIndicator::getIndicator: bars parm error" << l[2];
    return 1;
  }

  int loop;
  QStringList l2;
  if (bars == 0)
    loop = 0;
  else
    loop = in->count() - bars;
  for (; loop < in->count(); loop++)
    l2.append(QString::number(in->getData(loop)));

  ba.clear();
  ba.append(l2.join(","));
  ba.append('\n');

  return 0;
}

int SCIndicator::getIndex (QStringList &l, QHash<QString, PlotLine *> &tlines, QByteArray &ba)
{
  // format = INDICATOR_GET_INDEX,INPUT_ARRAY,OFFSET

  ba.clear();
  ba.append("ERROR\n");

  if (l.count() != 3)
  {
    qDebug() << "SCIndicator::getIndex: invalid parm count" << l.count();
    return 1;
  }

  PlotLine *line = tlines.value(l[1]);
  if (! line)
  {
    qDebug() << "SCIndicator::getIndex: invalid input" << l[1];
    return 1;
  }

  bool ok;
  int index = l[2].toInt(&ok);
  if (! ok)
  {
    qDebug() << "SCIndicator::getIndex: invalid index" << l[2];
    return 1;
  }

  int offset = line->count() - 1 - index;
  if (offset < 0)
  {
    qDebug() << "SCIndicator::getIndex: offset greater than" << l[1] << "size";
    return 1;
  }

  ba.clear();
  ba.append(QString::number(line->getData(offset)));
  ba.append('\n');

  return 0;
}

int SCIndicator::setIndicator (QStringList &l, QHash<QString, PlotLine *> &tlines, QByteArray &ba)
{
  // format 'INDICATOR_SET,VARIABLE,CSV,DATA,FROM,NOW,ON' - will create a new line using the provided data

  ba.clear();
  ba.append("1\n");

  if (l.count() < 3)
  {
    qDebug() << "SCIndicator::setIndicator: invalid parm count " << l.count();
    return 1;
  }

  PlotLine *line = tlines.value(l[1]);
  if (line)
  {
    // variable already used, abort
    qDebug() << "SCIndicator::setIndicator: duplicate variable name" << l[1];
    return 1;
  }

  line = new PlotLine;
  int loop;
  QColor color("red");
  for (loop = 2; loop < l.count(); loop++)
    line->append(color, l[loop].toDouble());

  tlines.insert(l[1], line);

  ba.clear();
  ba.append("0\n");

  return 0;
}

int SCIndicator::getSize (QStringList &l, QHash<QString, PlotLine *> &tlines, QByteArray &ba)
{
  // format = INDICATOR_GET_SIZE,INPUT_ARRAY

  ba.clear();
  ba.append("ERROR\n");

  if (l.count() != 2)
  {
    qDebug() << "SCIndicator::getSize: invalid parm count" << l.count();
    return 1;
  }

  PlotLine *line = tlines.value(l[1]);
  if (! line)
  {
    qDebug() << "SCIndicator::getSize: invalid input" << l[1];
    return 1;
  }

  int size = line->count();

  ba.clear();
  ba.append(QString::number(size));
  ba.append('\n');

  return 0;
}

