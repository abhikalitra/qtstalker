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

#include "CommandPlotLine.h"
#include "VerifyDataInput.h"
#include "LineStyle.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataColor.h"
#include "DataDouble.h"

#include <QtDebug>

CommandPlotLine::CommandPlotLine (QObject *p) : Command (p)
{
  _name = "PLOT_LINE";
}

int CommandPlotLine::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandPlotLine::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INPUT
  s = sg->value("INPUT");
  Data *in = vdi.toCurve(script, s);
  if (! in)
  {
    qDebug() << "CommandPlotLine::runScript: INPUT not found" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // CHART
  QString chart;
  s = sg->value("CHART");
  if (vdi.toString(script, s, chart))
  {
    qDebug() << "CommandPlotLine::runScript: invalid CHART" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // LABEL
  QString label;
  s = sg->value("LABEL");
  if (vdi.toString(script, s, label))
  {
    qDebug() << "CommandPlotLine::runScript invalid LABEL" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // STYLE
  LineStyle ls;
  QString style = sg->value("STYLE");
  if (ls.stringToStyle(style) == -1)
  {
    qDebug() << "CommandPlotLine::runScript: invalid STYLE, using default" << style;
    style = "Line";
  }

  // COLOR
  QColor color;
  s = sg->value("COLOR");
  if (vdi.toColor(script, s, color))
  {
    qDebug() << "CommandPlotLine::runScript: invalid COLOR, using default" << s;
    color = QColor(Qt::red);
  }

  // Z
  int z = 0;
  s = sg->value("Z");
  if (vdi.toInteger(script, s, z))
  {
    qDebug() << "CommandPlotLine::runScript: invalid Z, using default" << s;
    z = 0;
  }

  // PEN
  int pen = 1;
  s = sg->value("PEN");
  if (vdi.toInteger(script, s, pen))
  {
    qDebug() << "CommandPlotLine::runScript: invalid PEN, using default" << s;
    pen = 1;
  }

  QList<Data *> list;
  list << in;

  QList<int> keys;
  if (vdi.curveKeys(list, keys))
  {
    qDebug() << "CommandPlotLine::runScript: invalid keys";
    emit signalResume((void *) this);
    return _ERROR;
  }

  Data *line = new CurveData;
  line->set(CurveData::_TYPE, new DataString(QString("Line")));
  line->set(CurveData::_Z, new DataInteger(z));
  line->set(CurveData::_PEN, new DataInteger(pen));
  line->set(CurveData::_LABEL, new DataString(label));
  line->set(CurveData::_CHART, new DataString(chart));
  line->set(CurveData::_STYLE, new DataString(style));

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *ibar = in->toData(keys.at(loop));
    if (! ibar)
      continue;

    Data *bar = new CurveBar;
    bar->set(CurveBar::_VALUE, new DataDouble(ibar->toData(CurveBar::_VALUE)->toDouble()));
    bar->set(CurveBar::_COLOR, new DataColor(color));
    line->set(keys.at(loop), bar);
  }

  script->setData(name, line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
