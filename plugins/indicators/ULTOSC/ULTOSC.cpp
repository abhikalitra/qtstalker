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

#include "ULTOSC.h"
#include "ta_libc.h"
#include "PlotFactory.h"

#include <QtDebug>

ULTOSC::ULTOSC ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("ULTOSC::error on TA_Initialize");

  indicator = "ULTOSC";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(ShortPeriod, 7);
  settings.setData(MidPeriod, 14);
  settings.setData(LongPeriod, 28);
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
  settings.setData(Ref3Color, "white");
}

int ULTOSC::getIndicator (Indicator &ind, BarData *data)
{
  // 30 ref line
  QString s = "Horizontal";
  PlotFactory fac;
  PlotLine *line = fac.plot(s);
  if (! line)
    return 0;

  settings.getData(Ref1Color, s);
  QColor color(s);

  line->setData(0, new PlotLineBar(color, (double) 30));
  ind.addLine(line);

  // 50 ref line
  s = "Horizontal";
  line = fac.plot(s);
  if (! line)
    return 0;

  settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setData(0, new PlotLineBar(color, (double) 50));
  ind.addLine(line);

  // 70 ref line
  s = "Horizontal";
  line = fac.plot(s);
  if (! line)
    return 0;

  settings.getData(Ref3Color, s);
  color.setNamedColor(s);

  line->setData(0, new PlotLineBar(color, (double) 70));
  ind.addLine(line);

  // ultosc line
  int sp = settings.getInt(ShortPeriod);
  int mp = settings.getInt(MidPeriod);
  int lp = settings.getInt(LongPeriod);

  settings.getData(Color, s);
  color.setNamedColor(s);

  settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  line = getULTOSC(data, sp, mp, lp, lineType, color);
  if (! line)
    return 1;

  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  return 0;
}

int ULTOSC::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,ULTOSC,<NAME>,<SHORT PERIOD>,<MED PERIOD>,<LONG PERIOD>,<PLOT TYPE>,<COLOR>

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
  int sp = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid short period" << set[4];
    return 1;
  }

  int mp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid med period" << set[5];
    return 1;
  }

  int lp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid long period" << set[6];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[7]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid plot type" << set[7];
    return 1;
  }

  QColor color(set[8]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[8];
    return 1;
  }

  PlotLine *line = getULTOSC(data, sp, mp, lp, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * ULTOSC::getULTOSC (BarData *data, int sp, int mp, int lp, int lineType, QColor &color)
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

  TA_RetCode rc = TA_ULTOSC(0,
                            size - 1,
                            &high[0],
                            &low[0],
                            &close[0],
                            sp,
                            mp,
                            lp,
                            &outBeg,
                            &outNb,
                            &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::getULTOSC: TA-Lib error" << rc;
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

int ULTOSC::dialog (int)
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

  dialog->addIntItem(ShortPeriod, page, QObject::tr("Short Period"), settings.getInt(ShortPeriod), 1, 100000);

  dialog->addIntItem(MidPeriod, page, QObject::tr("Mid Period"), settings.getInt(MidPeriod), 1, 100000);

  dialog->addIntItem(LongPeriod, page, QObject::tr("Long Period"), settings.getInt(LongPeriod), 1, 100000);

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 30 Color"), d);

  settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 50 Color"), d);

  settings.getData(Ref3Color, d);
  dialog->addColorItem(Ref3Color, page, QObject::tr("Ref. 70 Color"), d);

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
  ULTOSC *o = new ULTOSC;
  return ((IndicatorPlugin *) o);
}

