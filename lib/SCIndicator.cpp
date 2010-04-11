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

#include "SCIndicator.h"
#include "PluginFactory.h"
#include "IndicatorPlugin.h"

#include <QtDebug>


SCIndicator::SCIndicator ()
{
  methodList << "GET" << "GET_INDEX" << "GET_SIZE" << "PLUGIN" << "SET";
}

int SCIndicator::calculate (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines,
			    BarData *data, QString &path)
{
  // format = INDICATOR,METHOD,*
  int rc = 1;
  ba.clear();
  ba.append("ERROR\n");

  if (l.count() < 2)
  {
    qDebug() << "SCIndicator::calculate: invalid parm count" << l.count();
    return rc;
  }
  
  switch ((Method) methodList.indexOf(l[1]))
  {
    case GET:
      rc = getIndicator(l, ba, tlines);
      break;
    case GET_INDEX:
      rc = getIndex(l, ba, tlines);
      break;
    case GET_SIZE:
      rc = getSize(l, ba, tlines);
      break;
    case PLUGIN:
      rc = getPlugin(l, ba, tlines, data, path);
      break;
    case SET:
      rc = setIndicator(l, ba, tlines);
      break;
    default:
      qDebug() << "SCIndicator::calculate: invalid method" << l[1];
      break;
  }
  
  return rc;
}

int SCIndicator::getIndicator (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines)
{
  // format 'INDICATOR,GET,VARIABLE,BARS' returns the requested data in a CSV string

  if (l.count() != 4)
  {
    qDebug() << "SCIndicator::getIndicator: invalid parm count" << l.count();
    return 1;
  }

  PlotLine *in = tlines.value(l[2]);
  if (! in)
  {
    qDebug() << "SCIndicator::getIndicator: input parm error" << l[2];
    return 1;
  }

  bool ok;
  int bars = l[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << "SCIndicator::getIndicator: bars parm error" << l[3];
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
  ba.append(l2.join(",") + "\n");

  return 0;
}

int SCIndicator::getIndex (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines)
{
  // format = INDICATOR,GET_INDEX,INPUT_ARRAY,OFFSET

  if (l.count() != 4)
  {
    qDebug() << "SCIndicator::getIndex: invalid parm count" << l.count();
    return 1;
  }

  PlotLine *line = tlines.value(l[2]);
  if (! line)
  {
    qDebug() << "SCIndicator::getIndex: invalid input" << l[2];
    return 1;
  }

  bool ok;
  int index = l[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << "SCIndicator::getIndex: invalid index" << l[3];
    return 1;
  }

  int offset = line->count() - 1 - index;
  if (offset < 0)
  {
    qDebug() << "SCIndicator::getIndex: offset greater than" << l[2] << "size";
    return 1;
  }

  ba.clear();
  ba.append(QString::number(line->getData(offset)) + "\n");

  return 0;
}

int SCIndicator::setIndicator (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines)
{
  // format 'INDICATOR,SET,VARIABLE,*' - will create a new line using the provided data

  if (l.count() < 3)
  {
    qDebug() << "SCIndicator::setIndicator: invalid parm count " << l.count();
    return 1;
  }

  PlotLine *line = tlines.value(l[2]);
  if (line)
  {
    // variable already used, abort
    qDebug() << "SCIndicator::setIndicator: duplicate variable name" << l[2];
    return 1;
  }

  line = new PlotLine;
  int loop;
  QColor color("red");
  for (loop = 3; loop < l.count(); loop++)
    line->append(color, l[loop].toDouble());

  tlines.insert(l[2], line);

  ba.clear();
  ba.append("0\n");

  return 0;
}

int SCIndicator::getSize (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines)
{
  // format = INDICATOR,GET_SIZE,INPUT_ARRAY

  if (l.count() != 3)
  {
    qDebug() << "SCIndicator::getSize: invalid parm count" << l.count();
    return 1;
  }

  PlotLine *line = tlines.value(l[2]);
  if (! line)
  {
    qDebug() << "SCIndicator::getSize: invalid input" << l[2];
    return 1;
  }

  int size = line->count();

  ba.clear();
  ba.append(QString::number(size) + "\n");

  return 0;
}

int SCIndicator::getPlugin (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines,
			    BarData *data, QString &path)
{
  // format 'INDICATOR,PLUGIN,PLUGIN,*

  if (l.count() < 3)
  {
    qDebug() << "SCIndicator::getPlugin: invalid parm count" << l.count();
    return 1;
  }

  PluginFactory fac;
  IndicatorPlugin *ip = fac.getIndicator(path, l[2]);
  if (! ip)
    return 1;

  int rc = ip->getCUS(l, tlines, data);
  if (ip->getDeleteFlag())
    delete ip;

  ba.clear();
  ba.append(QString::number(rc) + '\n');

  return 0;
}

