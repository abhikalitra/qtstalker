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
#include "PlotLineBar.h"
#include "PlotFactory.h"

#include <QtDebug>

SCIndicator::SCIndicator ()
{
  methodList << "NEW" << "GET_INDEX" << "GET_RANGE" << "PLUGIN" << "SET_INDEX" << "SET_COLOR";
}

int SCIndicator::calculate (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines,
			    BarData *data, QString &path)
{
  // format = INDICATOR,PLUGIN,*
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
    case NEW:
      rc = getNew(l, ba, tlines, data);
      break;
    case GET_INDEX:
      rc = getIndex(l, ba, tlines);
      break;
    case GET_RANGE:
      rc = getRange(l, ba, tlines);
      break;
    case PLUGIN:
      rc = getPlugin(l, ba, tlines, data, path);
      break;
    case SET_INDEX:
      rc = setIndex(l, ba, tlines);
      break;
    case SET_COLOR:
      rc = setColor(l, ba, tlines);
      break;
    default:
      qDebug() << "SCIndicator::calculate: invalid method" << l[1];
      break;
  }
  
  return rc;
}

int SCIndicator::getNew (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,NEW,<METHOD>,<NAME>,<LINE TYPE>,<COLOR>
  //     0      1     2        3        4         5

  if (l.count() != 6)
  {
    qDebug() << "SCIndicator::getNew: invalid parm count" << l.count();
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(l[4]);
  if (lineType == -1)
  {
    qDebug() << "SCIndicator::getNEW: invalid plot type" << l[4];
    return 1;
  }
  
  QColor color(l[5]);
  if (! color.isValid())
  {
    qDebug() << "SCIndicator::getNEW: invalid color" << l[5];
    return 1;
  }
  
  BarData bd;
  QStringList fl;
  bd.getInputFields(fl);
  fl.prepend("EMPTY");
  int method = fl.indexOf(l[2]);
  if (method == -1)
  {
    qDebug() << "SCIndicator::getNew: invalid method" << method << l[2];
    return 1;
  }
  
  PlotLine *out = 0;
  switch (method)
  {
    case 0:
    {
      // check if name already exists
      out = tlines.value(l[3]);
      if (out)
      {
        qDebug() << "SCIndicator::getNew: name already exists" << l[3];
        return 1;
      }

      out = fac.plot(lineType);
      break;
    }
    default:
      out = data->getInput(data->getInputType(l[2]), lineType, color);
      if (! out)
      {
        qDebug() << "SCIndicator::getNEW: input not found" << l[2];
        return 1;
      }
      break;
  }

  out->setLabel(l[3]);
  
  tlines.insert(l[3], out);

  ba.clear();
  ba.append("0\n");

  return 0;
}

int SCIndicator::getIndex (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines)
{
  // INDICATOR,GET_INDEX,<INPUT>,<INDEX>
  //     0         1        2       3

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
    qDebug() << "SCIndicator::getIndex: invalid index value" << l[3];
    return 1;
  }

  PlotLineBar *bar = line->data(index);
  if (! bar)
  {
    qDebug() << "SCIndicator::getIndex: invalid index value" << l[3];
    return 1;
  }
  
  ba.clear();

  ba.append(QString::number(bar->data()) + "\n");

  return 0;
}

int SCIndicator::setIndex (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines)
{
  // INDICATOR,SET_INDEX,<NAME>,<INDEX>,<VALUE>,<COLOR>
  //     0         1        2       3       4      5

  if (l.count() != 6)
  {
    qDebug() << "SCIndicator::setIndex: invalid parm count " << l.count();
    return 1;
  }

  PlotLine *line = tlines.value(l[2]);
  if (! line)
  {
    qDebug() << "SCIndicator::setIndex: name not found" << l[2];
    return 1;
  }

  bool ok;
  int index = l[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << "SCIndicator::setIndex: invalid index value" << l[3];
    return 1;
  }

  double value = l[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SCIndicator::setIndex: invalid value" << l[4];
    return 1;
  }

  QColor color(l[5]);
  if (! color.isValid())
  {
    qDebug() << "SCIndicator::setIndex: invalid color" << l[5];
    return 1;
  }
  
  line->setData(index, new PlotLineBar(color, value));

  ba.clear();
  ba.append("0\n");

  return 0;
}

int SCIndicator::getRange (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines)
{
  // INDICATOR,GET_RANGE,<INPUT>
  //      0       1         2

  if (l.count() != 3)
  {
    qDebug() << "SCIndicator::getRange: invalid parm count" << l.count();
    return 1;
  }

  PlotLine *line = tlines.value(l[2]);
  if (! line)
  {
    qDebug() << "SCIndicator::getRange: invalid input" << l[2];
    return 1;
  }

  int start = 0;
  int end = 0;
  line->keyRange(start, end);

  ba.clear();
  ba.append(QString::number(start) + "," + QString::number(end) + "\n");

  return 0;
}

int SCIndicator::setColor (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines)
{
  // INDICATOR,SET_COLOR,<NAME>,<INDEX>,<COLOR>
  //     0         1        2      3       4

  if (l.count() != 5)
  {
    qDebug() << "SCIndicator::setColor: invalid parm count " << l.count();
    return 1;
  }

  PlotLine *line = tlines.value(l[2]);
  if (! line)
  {
    qDebug() << "SCIndicator::setColor: name not found" << l[2];
    return 1;
  }

  bool ok;
  int index = l[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << "SCIndicator::setColor: invalid index value" << l[3];
    return 1;
  }

  QColor color(l[4]);
  if (! color.isValid())
  {
    qDebug() << "SCIndicator::setColor: invalid color" << l[4];
    return 1;
  }

  PlotLineBar *bar = line->data(index);
  if (! bar)
  {
    qDebug() << "SCIndicator::setColor: bar not found" << l[3];
    return 1;
  }
  
  bar->setColor(color);

  ba.clear();
  ba.append("0\n");

  return 0;
}

int SCIndicator::getPlugin (QStringList &l, QByteArray &ba, QHash<QString, PlotLine *> &tlines,
			    BarData *data, QString &path)
{
  // INDICATOR,PLUGIN,<PLUGIN>,*
  //     0       1       2

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

