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

#include "RSI.h"
#include "MAFactory.h"
#include "ta_libc.h"
#include "PlotFactory.h"

#include <QtDebug>

RSI::RSI ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("RSI::error on TA_Initialize");

  indicator = "RSI";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Period, 14);
  settings.setData(Smoothing, 1);
  settings.setData(SmoothingType, "SMA");
  settings.setData(Input, "Close");
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
  settings.setData(Ref1, 30);
  settings.setData(Ref2, 70);
}

int RSI::getIndicator (Indicator &ind, BarData *data)
{
  // ref1 line
  QString s = "Horizontal";
  PlotFactory fac;
  PlotLine *line = fac.plot(s);
  if (! line)
    return 0;

  settings.getData(Ref1Color, s);
  QColor color(s);

  line->setData(0, new PlotLineBar(color, (double) settings.getInt(Ref1)));
  ind.addLine(line);

  // ref2 line
  s = "Horizontal";
  line = fac.plot(s);
  if (! line)
    return 0;

  settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setData(0, new PlotLineBar(color, (double) settings.getInt(Ref2)));
  ind.addLine(line);

  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);
  int smoothing = settings.getInt(Smoothing);

  MAFactory mau;
  settings.getData(SmoothingType, s);
  int type = mau.typeFromString(s);

  settings.getData(Color, s);
  color.setNamedColor(s);

  settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  line = getRSI(in, period, smoothing, type, lineType, color);
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

int RSI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,RSI,<NAME>,<INPUT>,<PERIOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>,<PLOT TYPE>,<COLOR>
  //     0        1    2     3      4       5              6                 7             8         9

  if (set.count() != 10)
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
    qDebug() << indicator << "::getCUS: invalid period parm" << set[5];
    return 1;
  }

  int smoothing = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid smoothing" << set[6];
    return 1;
  }

  MAFactory mau;
  int type = mau.typeFromString(set[7]);
  if (type == -1)
  {
    qDebug() << indicator << "::getCUS: invalid smoothing type" << set[7];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[8]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid plot type" << set[8];
    return 1;
  }

  QColor color(set[9]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[9];
    return 1;
  }

  PlotLine *line = getRSI(in, period, smoothing, type, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * RSI::getRSI (PlotLine *in, int period, int smoothing, int type, int lineType, QColor &color)
{
  QList<int> keys;
  in->keys(keys);
  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_RSI(0,
                         size - 1,
                         &input[0],
                         period,
                         &outBeg,
                         &outNb,
                         &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::getRSI: TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setData(keys.at(keyLoop), new PlotLineBar(color, out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  if (smoothing > 1)
  {
    MAFactory mau;
    PlotLine *ma = mau.ma(line, smoothing, type, lineType, color);
    delete line;
    line = ma;
  }

  return line;
}

int RSI::dialog (int)
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

  dialog->addIntItem(Smoothing, page, QObject::tr("Smoothing"), settings.getInt(Smoothing), 1, 100000);

  QStringList maList;
  MAFactory mau;
  mau.list(maList);

  settings.getData(SmoothingType, d);
  dialog->addComboItem(Smoothing, page, QObject::tr("Smoothing Type"), maList, d);

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

  dialog->addIntItem(Ref1, page, QObject::tr("Ref. 1"), settings.getInt(Ref1), 0, 100);

  dialog->addIntItem(Ref2, page, QObject::tr("Ref. 2"), settings.getInt(Ref2), 0, 100);

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
  RSI *o = new RSI;
  return ((IndicatorPlugin *) o);
}


