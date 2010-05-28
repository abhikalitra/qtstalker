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

#include "SAR.h"
#include "ta_libc.h"
#include "BARSUtils.h"
#include "PlotFactory.h"

#include <QtDebug>

SAR::SAR ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("SAR::error on TA_Initialize");

  _indicator = "SAR";

  _settings.setData(Color, "yellow");
  _settings.setData(Plot, "Dot");
  _settings.setData(Label, _indicator);
  _settings.setData(Init, 0.02);
  _settings.setData(Max, 0.2);
}

int SAR::getIndicator (Indicator &ind, BarData *data)
{
  double tinit = _settings.getDouble(Init);
  double tmax = _settings.getDouble(Max);

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  BARSUtils b;
  PlotLine *bars = b.getBARS(data, up, down, neutral);
  if (bars)
  {
    QString s = "0";
    ind.setLine(s, bars);
    ind.addPlotOrder(s);
  }

  QString s;
  _settings.getData(Color, s);
  QColor color(s);

  PlotFactory fac;
  _settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  PlotLine *line = getSAR(data, tinit, tmax, lineType, color);
  if (! line)
    return 1;

  _settings.getData(Label, s);
  line->setLabel(s);
  
  s = "1";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  return 0;
}

int SAR::getCUS (QStringList &set, Indicator &ind, BarData *data)
{
  // INDICATOR,PLUGIN,SAR,<NAME>,<INITIAL_STEP>,<MAX_STEP>,<PLOT TYPE>,<COLOR>
  //      0       1    2     3         4            5           6         7

  if (set.count() != 8)
  {
    qDebug() << _indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  double init = set[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid init" << set[4];
    return 1;
  }

  double max = set[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid max" << set[5];
    return 1;
  }

   PlotFactory fac;
  int lineType = fac.typeFromString(set[6]);
  if (lineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid plot type" << set[6];
    return 1;
  }

  QColor color(set[7]);
  if (! color.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid color" << set[7];
    return 1;
  }

  PlotLine *line = getSAR(data, init, max, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

PlotLine * SAR::getSAR (BarData *data, double _init, double _max, int lineType, QColor &color)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    high[loop] = (TA_Real) bar->getHigh();
    low[loop] = (TA_Real) bar->getLow();
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_SAR(0, size - 1, &high[0], &low[0], _init, _max, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getSAR: TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  for (loop = 0; loop < outNb; loop++)
    line->setData(loop + 1, new PlotLineBar(color, out[loop]));

  return line;
}

int SAR::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  _settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  _settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  dialog->addDoubleItem(Init, page, QObject::tr("Initial"), _settings.getDouble(Init), 0.0, 100000.0);

  dialog->addDoubleItem(Max, page, QObject::tr("Max"), _settings.getDouble(Max), 0.0, 100000.0);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  getDialogSettings(dialog);

  delete dialog;
  return rc;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  SAR *o = new SAR;
  return ((IndicatorPlugin *) o);
}


