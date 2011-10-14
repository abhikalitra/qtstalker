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
#include "VerifyDataInput.h"
#include "OHLCStyle.h"
#include "CurveData.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataDouble.h"
#include "DataColor.h"
#include "CurveBar.h"

#include <QtDebug>

CommandPlotOHLC::CommandPlotOHLC (QObject *p) : Command (p)
{
  _name = "PLOT_OHLC";
}

int CommandPlotOHLC::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandPlotOHLC::runScript invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // CHART
  QString chart;
  s = sg->value("CHART");
  if (vdi.toString(script, s, chart))
  {
    qDebug() << "CommandPlotOHLC::runScript invalid CHART" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // LABEL
  QString label;
  s = sg->value("LABEL");
  if (vdi.toString(script, s, label))
  {
    qDebug() << "CommandPlotOHLC::runScript invalid LABEL" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // STYLE
  OHLCStyle ls;
  QString style = sg->value("STYLE");
  if (ls.stringToStyle(style) == -1)
  {
    qDebug() << "CommandPlotOHLC::runScript invalid STYLE, using default" << style;
    style = "OHLC";
  }

  // COLOR
  QColor color;
  s = sg->value("COLOR");
  if (vdi.toColor(script, s, color))
  {
    qDebug() << "CommandPlotOHLC::runScript invalid COLOR, using default" << s;
    color = QColor(Qt::red);
  }

  // Z
  int z = 0;
  s = sg->value("Z");
  if (vdi.toInteger(script, s, z))
  {
    qDebug() << "CommandPlotOHLC::runScript invalid Z, using default" << s;
    z = 1;
  }

  // PEN
  int pen = 1;
  s = sg->value("PEN");
  if (vdi.toInteger(script, s, pen))
  {
    qDebug() << "CommandPlotOHLC::runScript invalid PEN, using default" << s;
    pen = 1;
  }

  s = sg->value("OPEN");
  Data *iopen = vdi.toCurve(script, s);
  if (! iopen)
  {
    qDebug() << "CommandPlotOHLC::runScript invalid OPEN" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("HIGH");
  Data *ihigh = vdi.toCurve(script, s);
  if (! ihigh)
  {
    qDebug() << "CommandPlotOHLC::runScript invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("LOW");
  Data *ilow = vdi.toCurve(script, s);
  if (! ilow)
  {
    qDebug() << "CommandPlotOHLC::runScript invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("CLOSE");
  Data *iclose = vdi.toCurve(script, s);
  if (! iclose)
  {
    qDebug() << "CommandPlotOHLC::runScript invalid CLOSE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QList<Data *> list;
  list << iopen << ihigh << ilow << iclose;

  QList<int> keys;
  if (vdi.curveKeys(list, keys))
  {
    qDebug() << "CommandPlotOHLC::runScript invalid keys";
    emit signalResume((void *) this);
    return _ERROR;
  }

  Data *line = new CurveData;
  line->set(CurveData::_TYPE, new DataString(QString("OHLC")));
  line->set(CurveData::_Z, new DataInteger(z));
  line->set(CurveData::_PEN, new DataInteger(pen));
  line->set(CurveData::_LABEL, new DataString(label));
  line->set(CurveData::_CHART, new DataString(chart));
  line->set(CurveData::_STYLE, new DataString(style));

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *obar = iopen->toData(keys.at(loop));
    if (! obar)
      continue;

    Data *hbar = ihigh->toData(keys.at(loop));
    if (! hbar)
      continue;

    Data *lbar = ilow->toData(keys.at(loop));
    if (! lbar)
      continue;

    Data *cbar = iclose->toData(keys.at(loop));
    if (! cbar)
      continue;

    Data *bar = new CurveBar;
    bar->set(CurveBar::_OPEN, new DataDouble(obar->toData(CurveBar::_VALUE)->toDouble()));
    bar->set(CurveBar::_HIGH, new DataDouble(hbar->toData(CurveBar::_VALUE)->toDouble()));
    bar->set(CurveBar::_LOW, new DataDouble(lbar->toData(CurveBar::_VALUE)->toDouble()));
    bar->set(CurveBar::_CLOSE, new DataDouble(cbar->toData(CurveBar::_VALUE)->toDouble()));
    bar->set(CurveBar::_COLOR, new DataColor(color));
    line->set(keys.at(loop), bar);
  }

  script->setData(name, line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
