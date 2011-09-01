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

#include "CommandPlotOHLC.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandPlotOHLC::CommandPlotOHLC (QObject *p) : Command (p)
{
  _type = "PLOT_OHLC";
}

int CommandPlotOHLC::runScript (Data *sg, Script *script)
{
  QString name = sg->get("NAME");
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate name" << name;
    return _ERROR;
  }

  QString s = sg->get("OPEN");
  Data *iopen = script->data(s);
  if (! iopen)
  {
    qDebug() << _type << "::runScript: invalid OPEN" << s;
    return _ERROR;
  }

  s = sg->get("HIGH");
  Data *ihigh = script->data(s);
  if (! ihigh)
  {
    qDebug() << _type << "::runScript: invalid HIGH" << s;
    return _ERROR;
  }

  s = sg->get("LOW");
  Data *ilow = script->data(s);
  if (! ilow)
  {
    qDebug() << _type << "::runScript: invalid LOW" << s;
    return _ERROR;
  }

  s = sg->get("CLOSE");
  Data *iclose = script->data(s);
  if (! iclose)
  {
    qDebug() << _type << "::runScript: invalid CLOSE" << s;
    return _ERROR;
  }

  QList<Data *> list;
  list << iopen << ihigh << ilow << iclose;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
  {
    qDebug() << _type << "::runScript: invalid keys";
    return _ERROR;
  }

//qDebug() << "CommandPlotOHLC::runScript:" << keys.count();

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
  line->set(CurveData::_TYPE, QString("OHLC"));
  line->set(CurveData::_Z, z);
  line->set(CurveData::_PEN, pen);
  line->set(CurveData::_LABEL, name);
  line->set(CurveData::_CHART, chart);
  line->set(CurveData::_STYLE, style);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *obar = iopen->getData(keys.at(loop));
    if (! obar)
      continue;

    Data *hbar = ihigh->getData(keys.at(loop));
    if (! hbar)
      continue;

    Data *lbar = ilow->getData(keys.at(loop));
    if (! lbar)
      continue;

    Data *cbar = iclose->getData(keys.at(loop));
    if (! cbar)
      continue;

    Data *bar = new CurveBar;
    bar->set(CurveBar::_OPEN, obar->getDouble(CurveBar::_VALUE));
    bar->set(CurveBar::_HIGH, hbar->getDouble(CurveBar::_VALUE));
    bar->set(CurveBar::_LOW, lbar->getDouble(CurveBar::_VALUE));
    bar->set(CurveBar::_CLOSE, cbar->getDouble(CurveBar::_VALUE));
    bar->set(CurveBar::_COLOR, color);
    line->set(keys.at(loop), bar);
  }

  script->setData(name, line);

  return _OK;
}

Data * CommandPlotOHLC::settings ()
{
  Data *sg = new Data;
  sg->set("CHART", QString());
  sg->set("NAME", QString("OHLC"));
  sg->set("STYLE", QString("OHLC"));
  sg->set("OPEN", QString("open"));
  sg->set("HIGH", QString("high"));
  sg->set("LOW", QString("low"));
  sg->set("CLOSE", QString("close"));
  sg->set("COLOR", QColor(Qt::red));
  sg->set("Z", -1);
  sg->set("PEN", 1);
  return sg;
}
