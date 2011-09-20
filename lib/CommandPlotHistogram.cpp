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
#include "CurveData.h"
#include "CurveBar.h"
#include "HistogramStyle.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingDouble.h"
#include "SettingColor.h"
#include "DataFactory.h"
#include "DataSetting.h"
#include "VerifyDataInput.h"
#include "SettingFactory.h"

#include <QtDebug>

CommandPlotHistogram::CommandPlotHistogram (QObject *p) : Command (p)
{
  _type = "PLOT_HISTOGRAM";
}

int CommandPlotHistogram::runScript (Message *sg, Script *script)
{
  // OUTPUT
  QString name = sg->value("OUTPUT");
  if (name.isEmpty())
  {
    _message << "invalid OUTPUT";
    return _ERROR;
  }

  // CHART
  QString chart = sg->value("CHART");
  if (chart.isEmpty())
  {
    _message << "invalid CHART";
    return _ERROR;
  }

  // style
  HistogramStyle ls;
  QString style = sg->value("STYLE");
  if (ls.stringToStyle(style) == -1)
  {
    _message << "invalid STYLE " + style;
    return _ERROR;
  }

  // color
  VerifyDataInput vdi;
  QString s = sg->value("COLOR");
  Setting *color = vdi.setting(SettingFactory::_COLOR, script, s);
  if (! color)
  {
    _message << "invalid COLOR " + s;
    return _ERROR;
  }

  // Z
  s = sg->value("Z");
  Setting *z = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! z)
  {
    _message << "invalid Z " + s;
    return _ERROR;
  }

  // PEN
  s = sg->value("PEN");
  Setting *pen = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! pen)
  {
    _message << "invalid PEN " + s;
    return _ERROR;
  }

  // HIGH
  s = sg->value("HIGH");
  Data *ihigh = vdi.curveAll(script, s);
  if (! ihigh)
  {
    _message << "invalid HIGH";
    return _ERROR;
  }

  // LOW
  s = sg->value("LOW");
  Data *ilow = vdi.curveAll(script, s);
  if (! ilow)
  {
    _message << "invalid LOW";
    return _ERROR;
  }

  // keys
  QList<Data *> list;
  list << ihigh << ilow;

  QList<int> keys;
  if (vdi.curveKeys(list, keys))
  {
    _message << "invalid keys";
    return _ERROR;
  }

  Data *line = new CurveData;
  line->set(CurveData::_TYPE, new SettingString(QString("Histogram")));
  line->set(CurveData::_Z, new SettingInteger(z->toInteger()));
  line->set(CurveData::_PEN, new SettingInteger(pen->toInteger()));
  line->set(CurveData::_LABEL, new SettingString(name));
  line->set(CurveData::_CHART, new SettingString(chart));
  line->set(CurveData::_STYLE, new SettingString(style));

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    double v = 0;
    if (vdi.curveValue(ihigh, keys, loop, 0, v))
      continue;

    double v2 = 0;
    if (vdi.curveValue(ilow, keys, loop, 0, v2))
      continue;

    Data *bar = new CurveBar;
    bar->set(CurveBar::_HIGH, new SettingDouble(v));
    bar->set(CurveBar::_LOW, new SettingDouble(v2));
    bar->set(CurveBar::_COLOR, new SettingColor(color->toColor()));
    line->set(keys.at(loop), bar);
  }

  script->setData(name, line);

  return _OK;
}
