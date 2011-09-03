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

  _types << "Histogram" << "Histogram Bar";
}

int CommandPlotHistogram::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT").toString();
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  // verify HIGH
  int valueFlag = FALSE;
  double value = 0;
  Data *ihigh = 0;
  QString s = sg->get("HIGH").toString();
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
  s = sg->get("LOW").toString();
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

  // style
  s = sg->get("STYLE").toString();
  if (_types.indexOf(s) == -1)
  {
    qDebug() << _type << "::runScript: invalid STYLE" << s;
    return _ERROR;
  }

  // color
  s = sg->get("COLOR").toString();
  QColor c(s);
  if (! c.isValid())
  {
    qDebug() << _type << "::runScript: invalid COLOR" << s;
    return _ERROR;
  }

  line = new CurveData;
  line->set(CurveData::_TYPE, QString("Histogram"));
  line->set(CurveData::_Z, sg->get("Z"));
  line->set(CurveData::_PEN, sg->get("PEN"));
  line->set(CurveData::_LABEL, name);
  line->set(CurveData::_CHART, sg->get("CHART"));
  line->set(CurveData::_STYLE, sg->get("STYLE"));

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

      v = hbar->get(CurveBar::_VALUE).toDouble();
    }

    double v2 = 0;
    if (valueFlag2)
      v2 = value2;
    else
    {
      Data *lbar = ilow->getData(keys.at(loop));
      if (! lbar)
        continue;

      v2 = lbar->get(CurveBar::_VALUE).toDouble();
    }

    Data *bar = new CurveBar;
    bar->set(CurveBar::_HIGH, QVariant(v));
    bar->set(CurveBar::_LOW, QVariant(v2));
    bar->set(CurveBar::_COLOR, sg->get("COLOR"));
    line->set(keys.at(loop), bar);
  }

  script->setData(name, line);

  return _OK;
}

Data * CommandPlotHistogram::settings ()
{
  Data *sg = new Data;
  sg->set("CHART", QVariant(QString()));
  sg->set("OUTPUT", QVariant(QString()));
  sg->set("STYLE", QVariant(QString("Histogram Bar")));
  sg->set("HIGH", QVariant(QString("high")));
  sg->set("LOW", QVariant(QString("low")));
  sg->set("COLOR", QVariant(QString("red")));
  sg->set("Z", QVariant(-1));
  sg->set("PEN", QVariant(1));
  return sg;
}
