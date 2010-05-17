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

#include "VAR.h"
#include "PlotFactory.h"

#include <QtDebug>

VAR::VAR ()
{
  indicator = "VAR";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Period, 5);
  settings.setData(Input, "Close");
}

int VAR::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);

  settings.getData(Color, s);
  QColor color(s);

  PlotFactory fac;
  settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  PlotLine *line = getVAR(in, period, lineType, color);
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

int VAR::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,VAR,<NAME>,<INPUT>,<PERIOD>,<PLOT TYPE>,<COLOR>
  //     0       1    2     3       4        5         6         7

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
    qDebug() << indicator << "::getCUS: invalid period settings" << set[5];
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

  PlotLine *line = getVAR(in, period, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * VAR::getVAR (PlotLine *in, int period, int lineType, QColor &color)
{
  if (in->count() < period)
    return 0;

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  QList<int> keys;
  in->keys(keys);
  
  int loop = period;
  for (; loop < keys.count(); loop++)
  {
    double mean = 0;
    int count = 0;
    for (; count < period; count++)
    {
      PlotLineBar *bar = in->data(keys.at(loop - count));
      mean += bar->data();
    }
    mean = mean / period;

    double total = 0;
    for (count = 0; count < period; count++)
    {
      PlotLineBar *bar = in->data(keys.at(loop - count));
      double t = bar->data() - mean;
      total += t * t;
    }

    line->setData(keys.at(loop), new PlotLineBar(color, total / (double) period));
  }

  return line;
}

int VAR::dialog (int)
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

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

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
  VAR *o = new VAR;
  return ((IndicatorPlugin *) o);
}

