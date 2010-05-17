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

#include "BETA.h"
#include "DBPlugin.h"
#include "ta_libc.h"
#include "PlotFactory.h"

#include <QtDebug>

BETA::BETA ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("TALIB::setDefaults:error on TA_Initialize");

  indicator = "BETA";

  settings.setData(Index, "SP500");
  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Input, "Close");
  settings.setData(Period, 5);
}

int BETA::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  settings.getData(Index, s);
  BarData bd;
  bd.setSymbol(s);
  bd.setBarLength(data->getBarLength());
  bd.setBarsRequested(data->getBarsRequested());

  DBPlugin db;
  db.getBars(bd);

  PlotLine *in2 = bd.getInput(BarData::Close);
  if (! in2)
  {
    qDebug() << indicator << "::calculate: index not found";
    delete in;
    return 1;
  }

  int period = settings.getInt(Period);

  settings.getData(Color, s);
  QColor color(s);

  settings.getData(Plot, s);
  PlotFactory fac;
  int lineType = fac.typeFromString(s);

  PlotLine *line = getBETA(in, in2, period, lineType, color);
  if (! line)
  {
    delete in;
    delete in2;
    return 1;
  }

  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  delete in;
  delete in2;

  return 0;
}

int BETA::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,BETA,<NAME>,<INPUT_1>,<INPUT_2>,<PERIOD>,<PLOT TYPE>,<COLOR>
  //     0       1      2     3       4         5        6          7         8

  if (set.count() != 9)
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

  PlotLine *in2 = tlines.value(set[5]);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(set[5]));
    if (! in2)
    {
      qDebug() << indicator << "::getCUS: input2 not found" << set[5];
      return 1;
    }

    tlines.insert(set[5], in2);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid period settings" << set[6];
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

  PlotLine *line = getBETA(in, in2, period, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * BETA::getBETA (PlotLine *in, PlotLine *in2, int period, int lineType, QColor &color)
{
  QList<int> keys;
  int size = in->count();
  if (in2->count() < size)
  {
    size = in2->count();
    in2->keys(keys);
  }
  else
    in->keys(keys);

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    if (! bar)
      continue;

    PlotLineBar *bar2 = in2->data(keys.at(loop));
    if (! bar2)
      continue;

    input[loop] = (TA_Real) bar->data();
    input2[loop] = (TA_Real) bar2->data();
  }

  TA_RetCode rc = TA_BETA(0, size - 1, &input[0], &input2[0], period, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
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

  return line;
}

int BETA::dialog (int)
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

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 1, 100000);

  settings.getData(Index, d);
  dialog->addTextItem(Index, page, QObject::tr("Index"), d);

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
  BETA *o = new BETA;
  return ((IndicatorPlugin *) o);
}


