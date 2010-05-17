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

#include "CCI.h"
#include "MAFactory.h"
#include "PlotFactory.h"
#include "ta_libc.h"

#include <QtDebug>
#include <cmath>

CCI::CCI ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CCI::error on TA_Initialize");

  indicator = "CCI";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Smoothing, 10);
  settings.setData(SmoothingType, "SMA");
  settings.setData(Period, 20);
  settings.setData(Ref1, 100);
  settings.setData(Ref2, -100);
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
}

int CCI::getIndicator (Indicator &ind, BarData *data)
{
  // create the ref1 line
  PlotFactory fac;
  QString s = "Horizontal";
  PlotLine *rline = fac.plot(s);
  if (! rline)
    return 0;

  settings.getData(Ref1Color, s);
  QColor color(s);

  int ref = settings.getInt(Ref1);
  rline->setData(0, new PlotLineBar(color, ref));
  
  ind.addLine(rline);

  // create the ref2 line
  s = "Horizontal";
  rline = fac.plot(s);
  if (! rline)
    return 0;

  settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  ref = settings.getInt(Ref2);
  rline->setData(0, new PlotLineBar(color, ref));

  ind.addLine(rline);

  // get the CCI plot
  int period = settings.getInt(Period);
  int smoothing = settings.getInt(Smoothing);

  MAFactory mau;
  settings.getData(SmoothingType, s);
  int type = mau.typeFromString(s);

  settings.getData(Color, s);
  color.setNamedColor(s);

  settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  PlotLine *line = getCCI(data, period, smoothing, type, lineType, color);
  if (! line)
    return 1;

  settings.getData(Label, s);
  line->setLabel(s);

  ind.addLine(line);

  return 0;
}

int CCI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,CCI,<NAME>,<PERIOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>,<PLOT TYPE>,<COLOR>
  //     0       1     2    3       4             5                  6              7        8

  if (set.count() != 9)
  {
    qDebug() << indicator << "::getCUS: invalid parm count" << set.count();
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
    qDebug() << indicator << "::getCUS: invalid period parm" << set[4];
    return 1;
  }

  int smoothing = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid smoothing period" << set[5];
    return 1;
  }

  MAFactory mau;
  int ma = mau.typeFromString(set[6]);
  if (ma == -1)
  {
    qDebug() << indicator << "::getCUS: invalid smoothing type" << set[6];
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

  PlotLine *line = getCCI(data, period, smoothing, ma, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * CCI::getCCI (BarData *data, int period, int smoothing, int type, int lineType, QColor &color)
{
  int size = data->count();
  if (size < period)
    return 0;

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

  TA_RetCode rc = TA_CCI(0,
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
    qDebug() << indicator << "::getCCI: TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *cci = fac.plot(lineType);
  if (! cci)
    return 0;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (dataLoop > -1 && outLoop > -1)
  {
    cci->setData(dataLoop, new PlotLineBar(color, out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  if (smoothing > 1)
  {
    MAFactory maf;
    PlotLine *ma = maf.ma(cci, smoothing, type, lineType, color);
    delete cci;
    cci = ma;
  }

  return cci;
}

int CCI::dialog (int)
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

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 2, 100000);

  dialog->addIntItem(Smoothing, page, QObject::tr("Smoothing"), settings.getInt(Smoothing), 1, 100000);

  QStringList maList;
  MAFactory mau;
  mau.list(maList);

  settings.getData(SmoothingType, d);
  dialog->addComboItem(SmoothingType, page, QObject::tr("Smoothing Type"), maList, d);

  page++;
  k = QObject::tr("Ref Lines");
  dialog->addPage(page, k);

  settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref 1 Color"), d);

  settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref 2 Color"), d);

  dialog->addIntItem(Ref1, page, QObject::tr("Ref 1"), settings.getInt(Ref1), -100000, 100000);

  dialog->addIntItem(Ref2, page, QObject::tr("Ref 2"), settings.getInt(Ref2), -100000, 100000);

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
  CCI *o = new CCI;
  return ((IndicatorPlugin *) o);
}


