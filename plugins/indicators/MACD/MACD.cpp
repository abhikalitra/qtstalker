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

#include "MACD.h"
#include "FunctionMA.h"
#include "PlotStyleFactory.h"
#include "FunctionMACD.h"

#include <QtDebug>

MACD::MACD ()
{
  _indicator = "MACD";

  _settings.setData(MACDColor, "red");
  _settings.setData(SignalColor, "yellow");
  _settings.setData(HistColor, "blue");
  _settings.setData(MACDPlot, "Line");
  _settings.setData(SignalPlot, "Line");
  _settings.setData(HistPlot, "Histogram");
  _settings.setData(MACDLabel, "MACD");
  _settings.setData(SignalLabel, "SIG");
  _settings.setData(HistLabel, "HIST");
  _settings.setData(FastPeriod, 12);
  _settings.setData(SlowPeriod, 26);
  _settings.setData(SignalPeriod, 9);
  _settings.setData(FastMA, "SMA");
  _settings.setData(SlowMA, "SMA");
  _settings.setData(SignalMA, "EMA");
  _settings.setData(Input, "Close");
}

int MACD::getIndicator (Indicator &ind, BarData &data)
{
  QString s;
  _settings.getData(Input, s);
  PlotLine *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::calculate: input not found" << s;
    return 1;
  }

  int fast = _settings.getInt(FastPeriod);
  int slow = _settings.getInt(SlowPeriod);
  int signal = _settings.getInt(SignalPeriod);

  FunctionMA mau;
  _settings.getData(FastMA, s);
  int fastma = mau.typeFromString(s);

  _settings.getData(SlowMA, s);
  int slowma = mau.typeFromString(s);

  _settings.getData(SignalMA, s);
  int signalma = mau.typeFromString(s);

  FunctionMACD f;
  QList<PlotLine *> pl;
  if (f.calculate(in, fast, fastma, slow, slowma, signal, signalma, pl))
  {
    delete in;
    return 1;
  }

  // plot hist 
  PlotLine *line = pl.at(2);

  _settings.getData(HistPlot, s);
  line->setType(s);

  _settings.getData(HistColor, s);
  line->setColor(s);

  _settings.getData(HistLabel, s);
  line->setLabel(s);
  
  s = "0";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  // plot macd
  line = pl.at(0);

  _settings.getData(MACDPlot, s);
  line->setType(s);

  _settings.getData(MACDColor, s);
  line->setColor(s);

  _settings.getData(MACDLabel, s);
  line->setLabel(s);

  s = "1";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  // plot signal
  line = pl.at(1);

  _settings.getData(SignalPlot, s);
  line->setType(s);

  _settings.getData(SignalColor, s);
  line->setColor(s);

  _settings.getData(SignalLabel, s);
  line->setLabel(s);
  
  s = "2";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  delete in;

  return 0;
}

int MACD::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionMACD f;
  return f.script(set, ind, data);
}

int MACD::dialog (int)
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

  dialog->addIntItem(FastPeriod, page, QObject::tr("Fast Period"), _settings.getInt(FastPeriod), 2, 100000);

  dialog->addIntItem(SlowPeriod, page, QObject::tr("Slow Period"), _settings.getInt(SlowPeriod), 2, 100000);

  FunctionMA mau;
  QStringList maList = mau.list();
  
  _settings.getData(FastMA, d);
  dialog->addComboItem(FastMA, page, QObject::tr("Fast MA"), maList, d);

  _settings.getData(SlowMA, d);
  dialog->addComboItem(SlowMA, page, QObject::tr("Slow MA"), maList, d);

  page++;
  k = QObject::tr("MACD");
  dialog->addPage(page, k);

  _settings.getData(MACDColor, d);
  dialog->addColorItem(MACDColor, page, QObject::tr("Color"), d);

  PlotStyleFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(MACDPlot, d);
  dialog->addComboItem(MACDPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MACDLabel, d);
  dialog->addTextItem(MACDLabel, page, QObject::tr("Label"), d, QString());

  page++;
  k = QObject::tr("Signal");
  dialog->addPage(page, k);

  _settings.getData(SignalColor, d);
  dialog->addColorItem(SignalColor, page, QObject::tr("Color"), d);

  _settings.getData(SignalPlot, d);
  dialog->addComboItem(SignalPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(SignalLabel, d);
  dialog->addTextItem(SignalLabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(SignalPeriod, page, QObject::tr("Period"), _settings.getInt(SignalPeriod), 1, 100000);

  _settings.getData(SignalMA, d);
  dialog->addComboItem(SignalMA, page, QObject::tr("MA Type"), maList, d);

  page++;
  k = QObject::tr("Hist");
  dialog->addPage(page, k);

  _settings.getData(HistColor, d);
  dialog->addColorItem(HistColor, page, QObject::tr("Color"), d);

  _settings.getData(HistPlot, d);
  dialog->addComboItem(HistPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(HistLabel, d);
  dialog->addTextItem(HistLabel, page, QObject::tr("Label"), d, QString());

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
  MACD *o = new MACD;
  return ((IndicatorPlugin *) o);
}


