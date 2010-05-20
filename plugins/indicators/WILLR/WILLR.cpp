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

#include "WILLR.h"
#include "PlotFactory.h"
#include "ta_libc.h"

#include <QtDebug>

WILLR::WILLR ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("WILLR::error on TA_Initialize");

  indicator = "WILLR";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Period, 14);
}

int WILLR::getIndicator (Indicator &ind, BarData *data)
{
  int period = settings.getInt(Period);

  QString s;
  settings.getData(Color, s);
  QColor color(s);

  PlotFactory fac;
  settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  PlotLine *line = getWILLR(data, period, lineType, color);
  if (! line)
    return 1;

  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  return 0;
}

int WILLR::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,WILLR,<NAME>,<PERIOD>,<PLOT TYPE>,<COLOR>
  //     0       1      2     3       4          5         6

  if (set.count() != 7)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid period settings" << set[4];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[5]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid plot type" << set[5];
    return 1;
  }

  QColor color(set[6]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[6];
    return 1;
  }

  PlotLine *line = getWILLR(data, period, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * WILLR::getWILLR (BarData *data, int period, int lineType, QColor &color)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    high[loop] = (TA_Real) bar->getHigh();
    low[loop] = (TA_Real) bar->getLow();
    close[loop] = (TA_Real) bar->getClose();
  }

  TA_RetCode rc = TA_WILLR(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           &close[0],
                           period,
                           &outBeg,
                           &outNb,
                           &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::getWILLR: TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setData(dataLoop, new PlotLineBar(color, out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  return line;
}

int WILLR::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 1, 100000);

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
  WILLR *o = new WILLR;
  return ((IndicatorPlugin *) o);
}

