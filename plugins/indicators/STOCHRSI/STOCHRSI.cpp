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

#include "STOCHRSI.h"
#include "MAFactory.h"
#include "ta_libc.h"
#include "PlotFactory.h"

#include <QtDebug>

STOCHRSI::STOCHRSI ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("STOCHRSI::error on TA_Initialize");

  indicator = "STOCHRSI";

  settings.setData(Color, "red");
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
  settings.setData(Plot, "Line");
  settings.setData(Label, "STOCHRSI");
  settings.setData(Ref1, 0.2);
  settings.setData(Ref2, 0.8);
  settings.setData(Input, "Close");
  settings.setData(Period, 14);
}

int STOCHRSI::getIndicator (Indicator &ind, BarData *data)
{
  // create first ref line
  QString s = "Horizontal";
  PlotFactory fac;
  PlotLine *line = fac.plot(s);
  if (! line)
    return 1;

  settings.getData(Ref1Color, s);
  QColor color(s);

  line->setData(0, new PlotLineBar(color, (double) settings.getInt(Ref1)));
  ind.addLine(line);

  // create second ref line
  s = "Horizontal";
  line = fac.plot(s);
  if (! line)
    return 1;

  settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setData(0, new PlotLineBar(color, (double) settings.getInt(Ref2)));
  ind.addLine(line);

  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);

  settings.getData(Color, s);
  color.setNamedColor(s);

  settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  line = getSTOCHRSI(in, period, lineType, color);
  if (! line)
  {
    delete in;
    return 1;
  }
  
  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  delete in;

  return 0;
}

int STOCHRSI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,STOCHRSI,<NAME>,<INPUT>,<PERIOD>,<PLOT TYPE>,<COLOR>
  //    0        1       2       3       4       5          6         7

  if (set.count() != 8)
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

  PlotLine *in = tlines.value(set[4]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[4]));
    if (! in)
    {
      qDebug() << indicator << "::getCUS: input not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid period" << set[5];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[6]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid plot type" << set[6];
    return 1;
  }

  QColor color(set[7]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[7];
    return 1;
  }

  PlotLine *line = getSTOCHRSI(in, period, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * STOCHRSI::getSTOCHRSI (PlotLine *in, int period, int lineType, QColor &color)
{
  QList<int> keys;
  in->keys(keys);
  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_STOCHRSI(0,
                              size - 1,
                              &input[0],
                              period,
                              period,
                              0,
                              (TA_MAType) 0,
                              &outBeg,
                              &outNb,
                              &out[0],
                              &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::getSTOCHRSI: TA-Lib error" << rc;
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

int STOCHRSI::dialog (int)
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

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 1 Color"), d);

  settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 2 Color"), d);

  dialog->addDoubleItem(Ref1, page, QObject::tr("Ref. 1"), settings.getDouble(Ref1), 0.0, 1.0);

  dialog->addDoubleItem(Ref2, page, QObject::tr("Ref. 2"), settings.getDouble(Ref2), 0.0, 1.0);

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
  STOCHRSI *o = new STOCHRSI;
  return ((IndicatorPlugin *) o);
}


