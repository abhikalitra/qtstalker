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

#include "CommandPlotHistogram.h"
#include "Script.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandPlotHistogram::CommandPlotHistogram (QObject *p) : Command (p)
{
  _type = "PLOT_HISTOGRAM";
}

int CommandPlotHistogram::runScript (Data *sg, Script *script)
{
  QString name = sg->get("NAME");
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate name" << name;
    return _ERROR;
  }

  // verify HIGH
  int valueFlag = FALSE;
  double value = 0;
  Data *ihigh = 0;
  QString s = sg->get("HIGH");
  bool ok;
  value = s.toDouble(&ok);
  if (ok)
    valueFlag++;
  else
  {
    ihigh = script->data(s);
    if (! ihigh)
    {
      qDebug() << _type << "::runScript: invalid HIGH" << s;
      return _ERROR;
    }
  }

  // verify LOW
  int valueFlag2 = FALSE;
  double value2 = 0;
  Data *ilow = 0;
  s = sg->get("LOW");
  value2 = s.toDouble(&ok);
  if (ok)
    valueFlag2++;
  else
  {
    ilow = script->data(s);
    if (! ilow)
    {
      qDebug() << _type << "::runScript: invalid LOW" << s;
      return _ERROR;
    }
  }

  QList<Data *> list;
  if (! valueFlag)
    list << ihigh;
  if (! valueFlag2)
    list << ilow;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
  {
    qDebug() << _type << "::runScript: invalid keys";
    return _ERROR;
  }

  // chart
  QString chart = sg->get("CHART");

  // style
  QString style = sg->get("STYLE");

  // color
  QColor color = sg->getColor("COLOR");

  // Z
  int z = sg->getInteger("Z");

  // PEN
  int pen = sg->getInteger("PEN");

  line = new CurveData;
  line->set(CurveData::_TYPE, QString("Histogram"));
  line->set(CurveData::_Z, z);
  line->set(CurveData::_PEN, pen);
  line->set(CurveData::_LABEL, name);
  line->set(CurveData::_CHART, chart);
  line->set(CurveData::_STYLE, style);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    double v = 0;
    if (valueFlag)
      v = value;
    else
    {
      Data *hbar = ihigh->getData(keys.at(loop));
      if (! hbar)
        continue;
      v = hbar->getDouble(CurveBar::_VALUE);
    }

    double v2 = 0;
    if (valueFlag2)
      v2 = value2;
    else
    {
      Data *lbar = ilow->getData(keys.at(loop));
      if (! lbar)
        continue;
      v2 = lbar->getDouble(CurveBar::_VALUE);
    }

    Data *bar = new CurveBar;
    bar->set(CurveBar::_HIGH, v);
    bar->set(CurveBar::_LOW, v2);
    bar->set(CurveBar::_COLOR, color);
    line->set(keys.at(loop), bar);
  }

  script->setData(name, line);

  return _OK;
}

Data * CommandPlotHistogram::settings ()
{
  Data *sg = new Data;
  sg->set("CHART", QString());
  sg->set("NAME", QString());
  sg->set("STYLE", QString("Histogram Bar"));
  sg->set("HIGH", QString("high"));
  sg->set("LOW", QString("low"));
  sg->set("COLOR", QColor(Qt::red));
  sg->set("Z", -1);
  sg->set("PEN", 1);
  return sg;
}
