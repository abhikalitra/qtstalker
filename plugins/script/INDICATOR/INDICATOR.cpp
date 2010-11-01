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

#include "INDICATOR.h"
#include "IndicatorPluginFactory.h"
#include "Curve.h"

#include <QtDebug>

INDICATOR::INDICATOR ()
{
  _methodList << "NEW" << "GET_INDEX" << "GET_INDEX_DATE" << "GET_RANGE" << "PLUGIN" << "SET_INDEX";
  _methodList << "DELETE";
  _methodList << "SET_COLOR" << "SET_COLOR_ALL" << "SET_PLOT_STYLE" << "SET_PLOT";
}

int INDICATOR::command (QStringList &l, Indicator &ind, BarData &data, QByteArray &ba)
{
  // INDICATOR,METHOD,*
  //     0       1
  
  int rc = 1;
  ba.clear();
  ba.append("ERROR\n");

  if (l.count() < 2)
  {
    qDebug() << "INDICATOR::command: invalid parm count" << l.count();
    return rc;
  }
  
  switch ((Method) _methodList.indexOf(l[1]))
  {
    case __NEW:
      rc = getNew(l, ba, ind, data);
      break;
    case _GET_INDEX:
      rc = getIndex(l, ba, ind);
      break;
    case _GET_INDEX_DATE:
      rc = getIndexDate(l, ba, data);
      break;
    case _GET_RANGE:
      rc = getRange(l, ba, ind);
      break;
    case _PLUGIN:
      rc = getPlugin(l, ba, ind, data);
      break;
    case _SET_INDEX:
      rc = setIndex(l, ba, ind);
      break;
    case _DELETE:
      rc = setDelete(l, ba, ind);
      break;
    case _SET_COLOR:
      rc = setColor(l, ba, ind);
      break;
    case _SET_COLOR_ALL:
      rc = setColorAll(l, ba, ind);
      break;
    case _SET_PLOT_STYLE:
      rc = setPlotStyle(l, ba, ind);
      break;
    case _SET_PLOT:
      rc = setPlot(l, ba, ind);
      break;
    default:
      qDebug() << "INDICATOR::command: invalid method" << l[1];
      break;
  }
  
  return rc;
}

int INDICATOR::getNew (QStringList &l, QByteArray &ba, Indicator &ind, BarData &data)
{
  // INDICATOR,NEW,<METHOD>,<NAME>
  //     0      1     2        3

  if (l.count() != 4)
  {
    qDebug() << "INDICATOR::getNew: invalid parm count" << l.count();
    return 1;
  }

  BarData bd;
  QStringList fl;
  bd.getInputFields(fl);
  fl.prepend("EMPTY");
  int method = fl.indexOf(l[2]);
  if (method == -1)
  {
    qDebug() << "INDICATOR::getNew: invalid method" << method << l[2];
    return 1;
  }

  if (! data.count())
  {
    qDebug() << "INDICATOR::getNew: no bars available";
    return 1;
  }

  Curve *out = 0;
  switch (method)
  {
    case 0:
    {
      // check if name already exists
      out = ind.line(l.at(3));
      if (out)
      {
        qDebug() << "INDICATOR::getNew: name already exists" << l.at(3);
        return 1;
      }

      out = new Curve;
      break;
    }
    default:
      out = data.getInput(data.getInputType(l[2]));
      if (! out)
      {
        qDebug() << "INDICATOR::getNEW: input not found" << l[2];
        return 1;
      }
      break;
  }

  out->setLabel(l[3]);
  
  ind.setLine(l.at(3), out);

  ba.clear();
  ba.append("0\n");

  return 0;
}

int INDICATOR::getIndex (QStringList &l, QByteArray &ba, Indicator &ind)
{
  // INDICATOR,GET_INDEX,<INPUT>,<INDEX>
  //     0         1        2       3

  if (l.count() != 4)
  {
    qDebug() << "INDICATOR::getIndex: invalid parm count" << l.count();
    return 1;
  }

  Curve *line = ind.line(l.at(2));
  if (! line)
  {
    qDebug() << "INDICATOR::getIndex: invalid input" << l.at(2);
    return 1;
  }

  bool ok;
  int index = l[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR::getIndex: invalid index value" << l[3];
    return 1;
  }

  CurveBar *bar = line->bar(index);
  if (! bar)
  {
    qDebug() << "INDICATOR::getIndex: invalid index value" << l[3];
    return 1;
  }
  
  ba.clear();

  ba.append(QString::number(bar->data()) + "\n");

  return 0;
}

int INDICATOR::getIndexDate (QStringList &l, QByteArray &ba, BarData &data)
{
  // INDICATOR,GET_INDEX_DATE,<INDEX>
  //     0           1           2

  if (l.count() != 3)
  {
    qDebug() << "INDICATOR::getIndexDate: invalid parm count" << l.count();
    return 1;
  }

  bool ok;
  int index = l[2].toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR::getIndexDate: invalid index value" << l[2];
    return 1;
  }

  if (! data.count())
  {
    qDebug() << "INDICATOR::getIndexDate: no bars available";
    return 1;
  }

  Bar bar = data.getBar(index);
  if (! bar.count())
  {
    qDebug() << "INDICATOR::getIndexDate: invalid index value" << l[2];
    return 1;
  }

  ba.clear();

  QString s;
  bar.getDateTimeString(s);
  
  ba.append(s + "\n");

  return 0;
}

int INDICATOR::setIndex (QStringList &l, QByteArray &ba, Indicator &ind)
{
  // INDICATOR,SET_INDEX,<NAME>,<INDEX>,<VALUE>,<COLOR>
  //     0         1        2       3       4      5

  if (l.count() != 6)
  {
    qDebug() << "INDICATOR::setIndex: invalid parm count " << l.count();
    return 1;
  }

  Curve *line = ind.line(l.at(2));
  if (! line)
  {
    qDebug() << "INDICATOR::setIndex: name not found" << l.at(2);
    return 1;
  }

  bool ok;
  int index = l[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR::setIndex: invalid index value" << l[3];
    return 1;
  }

  double value = l[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR::setIndex: invalid value" << l[4];
    return 1;
  }

  QColor color(l[5]);
  if (! color.isValid())
  {
    qDebug() << "INDICATOR::setIndex: invalid color" << l[5];
    return 1;
  }

  line->setBar(index, new CurveBar(color, value));

  ba.clear();
  ba.append("0\n");

  return 0;
}

int INDICATOR::getRange (QStringList &l, QByteArray &ba, Indicator &ind)
{
  // INDICATOR,GET_RANGE,<INPUT>
  //      0       1         2

  if (l.count() != 3)
  {
    qDebug() << "INDICATOR::getRange: invalid parm count" << l.count();
    return 1;
  }

  Curve *line = ind.line(l.at(2));
  if (! line)
  {
    qDebug() << "INDICATOR::getRange: invalid input" << l.at(2);
    return 1;
  }

  int start = 0;
  int end = 0;
  line->keyRange(start, end);

  ba.clear();
  ba.append(QString::number(start) + "," + QString::number(end) + "\n");

  return 0;
}

int INDICATOR::getPlugin (QStringList &l, QByteArray &ba, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,<PLUGIN>,*
  //     0       1       2

  if (l.count() < 3)
  {
    qDebug() << "INDICATOR::getPlugin: invalid parm count" << l.count();
    return 1;
  }

  if (! data.count())
  {
    qDebug() << "INDICATOR::getPlugin: no bars available";
    return 1;
  }

  IndicatorPluginFactory fac;
  IndicatorPlugin *ip = fac.plugin(l[2]);
  if (! ip)
    return 1;
  
  int rc = ip->getCUS(l, ind, data);
  if (ip->deleteFlag())
    delete ip;

  ba.clear();
  ba.append(QString::number(rc) + '\n');

  return 0;
}

int INDICATOR::setDelete (QStringList &l, QByteArray &ba, Indicator &ind)
{
  // INDICATOR,DELETE,<NAME>
  //     0       1      2

  if (l.count() != 3)
  {
    qDebug() << "INDICATOR::getDelete: invalid parm count " << l.count();
    return 1;
  }

  if (ind.deleteLine(l.at(2)))
  {
    qDebug() << "INDICATOR::getDelete: name not found" << l.at(2);
    return 1;
  }

  ba.clear();
  ba.append("0\n");

  return 0;
}

int INDICATOR::setColor (QStringList &l, QByteArray &ba, Indicator &ind)
{
  // INDICATOR,SET_COLOR,<NAME>,<INDEX>,<COLOR>
  //  0      1        2      3       4

  if (l.count() != 5)
  {
    qDebug() << "INDICATOR::setColor: invalid parm count " << l.count();
    return 1;
  }

  Curve *line = ind.line(l.at(2));
  if (! line)
  {
    qDebug() << "INDICATOR::setColor: name not found" << l.at(2);
    return 1;
  }

  bool ok;
  int index = l[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR::setColor: invalid index value" << l[3];
    return 1;
  }

  QColor color(l[4]);
  if (! color.isValid())
  {
    qDebug() << "INDICATOR::setColor: invalid color" << l[4];
    return 1;
  }

  CurveBar *bar = line->bar(index);
  if (! bar)
  {
    qDebug() << "INDICATOR::setColor: bar not found" << l[3];
    return 1;
  }

  bar->setColor(color);

  ba.clear();
  ba.append("0\n");

  return 0;
}

int INDICATOR::setColorAll (QStringList &l, QByteArray &ba, Indicator &ind)
{
  // INDICATOR,SET_COLOR_ALL,<NAME>,<COLOR>
  //  0        1          2       3

  if (l.count() != 4)
  {
    qDebug() << "INDICATOR::setColorAll: invalid parm count " << l.count();
    return 1;
  }

  Curve *line = ind.line(l.at(2));
  if (! line)
  {
    qDebug() << "INDICATOR::setColorAll: name not found" << l.at(2);
    return 1;
  }

  QColor color(l[3]);
  if (! color.isValid())
  {
    qDebug() << "INDICATOR::setColorAll: invalid color" << l[3];
    return 1;
  }

  line->setAllColor(color);

  ba.clear();
  ba.append("0\n");

  return 0;
}

int INDICATOR::setPlotStyle (QStringList &l, QByteArray &ba, Indicator &ind)
{
  // INDICATOR,SET_PLOT_STYLE,<NAME>,<STYLE>
  //  0        1           2       3

  if (l.count() != 4)
  {
    qDebug() << "INDICATOR::setPlotStyle: invalid parm count " << l.count();
    return 1;
  }

  Curve *line = ind.line(l.at(2));
  if (! line)
  {
    qDebug() << "INDICATOR::setPlotStyle: name not found" << l.at(2);
    return 1;
  }

  int style = line->typeFromString(l[3]);
  if (style == -1)
  {
    qDebug() << "INDICATOR::setPlotStyle: invalid style" << l.at(3);
    return 1;
  }

  line->setType((Curve::Type) style);

  ba.clear();
  ba.append("0\n");

  return 0;
}

int INDICATOR::setPlot (QStringList &l, QByteArray &ba, Indicator &ind)
{
  // INDICATOR,SET_PLOT,<NAME>,<Z>
  //  0      1       2     3

  if (l.count() != 4)
  {
    qDebug() << "INDICATOR::setPlot: invalid parm count " << l.count();
    return 1;
  }

  Curve *line = ind.line(l.at(2));
  if (! line)
  {
    qDebug() << "INDICATOR::setPlot: name not found" << l.at(2);
    return 1;
  }

  bool ok;
  int z = l.at(3).toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR::setPlot: invalid z value" << l.at(3);
    return 1;
  }

  line->setZ(z);

  ba.clear();
  ba.append("0\n");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR *o = new INDICATOR;
  return ((ScriptPlugin *) o);
}

