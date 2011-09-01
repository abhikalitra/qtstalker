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
#include "Script.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandPlotLine::CommandPlotLine (QObject *p) : Command (p)
{
  _type = "PLOT_LINE";
}

int CommandPlotLine::runScript (Data *sg, Script *script)
{
  // input
  QString s = sg->get("INPUT");
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT not found" << s;
    return _ERROR;
  }

  // chart
  QString chart = sg->get("CHART");

  // style
  QString style = sg->get("STYLE");

  // label
  QString label = sg->get("NAME");
  if (label.isEmpty())
  {
    qDebug() << _type << "::runScript: invalid NAME" << label;
    return _ERROR;
  }

  Data *line = script->data(label);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate LABEL" << label;
    return _ERROR;
  }

  QList<Data *> list;
  list << in;
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
  {
    qDebug() << _type << "::runScript: invalid keys";
    return _ERROR;
  }

  // color
  QColor color = sg->getColor("COLOR");

  // Z
  int z = sg->getInteger("Z");

  // PEN
  int pen = sg->getInteger("PEN");

  line = new CurveData;
  line->set(CurveData::_TYPE, QString("Line"));
  line->set(CurveData::_Z, z);
  line->set(CurveData::_PEN, pen);
  line->set(CurveData::_LABEL, label);
  line->set(CurveData::_CHART, chart);
  line->set(CurveData::_STYLE, style);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *ibar = in->getData(keys.at(loop));
    if (! ibar)
      continue;

    Data *bar = new CurveBar;
    bar->set(CurveBar::_VALUE, ibar->getDouble(CurveBar::_VALUE));
    bar->set(CurveBar::_COLOR, color);
    line->set(keys.at(loop), bar);
  }

  script->setData(label, line);

  return _OK;
}

Data * CommandPlotLine::settings ()
{
  Data *sg = new Data;
  sg->set("CHART", QString());
  sg->set("NAME", QString("Line"));
  sg->set("INPUT", QString("close"));
  sg->set("STYLE", QString("Line"));
  sg->set("COLOR", QColor(Qt::red));
  sg->set("Z", -1);
  sg->set("PEN", 1);
  return sg;
}
