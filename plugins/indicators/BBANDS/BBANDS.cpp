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

#include "BBANDS.h"
#include "FunctionMA.h"
#include "BARSUtils.h"
#include "PlotStyleFactory.h"
#include "FunctionBBANDS.h"

#include <QtDebug>
#include <QColor>

BBANDS::BBANDS ()
{
  _indicator = "BBANDS";

  _settings.setData(UpColor, "red");
  _settings.setData(MidColor, "red");
  _settings.setData(DownColor, "red");
  _settings.setData(UpPlot, "Line");
  _settings.setData(MidPlot, "Line");
  _settings.setData(DownPlot, "Line");
  _settings.setData(UpLabel, "BBU");
  _settings.setData(MidLabel, "BBM");
  _settings.setData(DownLabel, "BBL");
  _settings.setData(UpDeviation, 2);
  _settings.setData(DownDeviation, 2);
  _settings.setData(Input, "Close");
  _settings.setData(Period, 20);
  _settings.setData(MAType, "SMA");
}

int BBANDS::getIndicator (Indicator &ind, BarData &data)
{
  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  BARSUtils b;
  PlotLine *bars = b.getBARS(data, up, down, neutral);
  if (! bars)
    return 1;

  QString s = "0";
  ind.setLine(s, bars);
  ind.addPlotOrder(s);

  _settings.getData(Input, s);
  PlotLine *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = _settings.getInt(Period);
  double udev = _settings.getDouble(UpDeviation);
  double ldev = _settings.getDouble(DownDeviation);

  FunctionMA mau;
  _settings.getData(MAType, s);
  int maType = mau.typeFromString(s);

  QList<PlotLine *> pl;
  FunctionBBANDS f;
  if (f.calculate(in, period, udev, ldev, maType, pl))
  {
    delete in;
    return 1;
  }

  PlotLine *line = pl.at(0);

  _settings.getData(UpPlot, s);
  line->setType(s);

  _settings.getData(UpColor, s);
  line->setColor(s);

  _settings.getData(UpLabel, s);
  line->setLabel(s);
  
  s = "1";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  line = pl.at(1);

  _settings.getData(MidPlot, s);
  line->setType(s);

  _settings.getData(MidColor, s);
  line->setColor(s);

  _settings.getData(MidLabel, s);
  line->setLabel(s);
  
  s = "2";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  line = pl.at(2);

  _settings.getData(DownPlot, s);
  line->setType(s);

  _settings.getData(DownColor, s);
  line->setColor(s);

  _settings.getData(DownLabel, s);
  line->setLabel(s);
  
  s = "3";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  delete in;

  return 0;
}

int BBANDS::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionBBANDS f;
  return f.script(set, ind, data);
}

int BBANDS::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);
  
  _settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addIntItem(Period, page, QObject::tr("Period"), _settings.getInt(Period), 2, 100000);

  dialog->addDoubleItem(UpDeviation, page, QObject::tr("Upper Deviation"), _settings.getDouble(UpDeviation), -100000, 100000);

  dialog->addDoubleItem(DownDeviation, page, QObject::tr("Lower Deviation"), _settings.getDouble(DownDeviation), -100000, 100000);

  FunctionMA mau;
  QStringList maList = mau.list();
  
  _settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("MA Type"), maList, d);

  PlotStyleFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  page++;
  k = QObject::tr("Upper");
  dialog->addPage(page, k);

  _settings.getData(UpColor, d);
  dialog->addColorItem(UpColor, page, QObject::tr("Color"), d);

  _settings.getData(UpPlot, d);
  dialog->addComboItem(UpPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(UpLabel, d);
  dialog->addTextItem(UpLabel, page, QObject::tr("Label"), d, QString());

  page++;
  k = QObject::tr("Mid");
  dialog->addPage(page, k);

  _settings.getData(MidColor, d);
  dialog->addColorItem(MidColor, page, QObject::tr("Color"), d);

  _settings.getData(MidPlot, d);
  dialog->addComboItem(MidPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MidLabel, d);
  dialog->addTextItem(MidLabel, page, QObject::tr("Label"), d, QString());

  page++;
  k = QObject::tr("Lower");
  dialog->addPage(page, k);

  _settings.getData(DownColor, d);
  dialog->addColorItem(DownColor, page, QObject::tr("Color"), d);

  _settings.getData(DownPlot, d);
  dialog->addComboItem(DownPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(DownLabel, d);
  dialog->addTextItem(DownLabel, page, QObject::tr("Label"), d, QString());

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
  BBANDS *o = new BBANDS;
  return ((IndicatorPlugin *) o);
}


