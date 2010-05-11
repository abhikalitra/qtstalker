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
#include "MAUtils.h"
#include "PlotFactory.h"

#include <QtDebug>


MACD::MACD ()
{
  indicator = "MACD";

  settings.setData(MACDColor, "red");
  settings.setData(SignalColor, "yellow");
  settings.setData(HistColor, "blue");
  settings.setData(MACDPlot, "Line");
  settings.setData(SignalPlot, "Dot");
  settings.setData(HistPlot, "Histogram Bar");
  settings.setData(MACDLabel, "MACD");
  settings.setData(SignalLabel, "SIG");
  settings.setData(HistLabel, "HIST");
  settings.setData(FastPeriod, 12);
  settings.setData(SlowPeriod, 26);
  settings.setData(SignalPeriod, 9);
  settings.setData(FastMA, "SMA");
  settings.setData(SlowMA, "SMA");
  settings.setData(SignalMA, "EMA");
  settings.setData(Input, "Close");
}

int MACD::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int fast = settings.getInt(FastPeriod);
  int slow = settings.getInt(SlowPeriod);
  int signal = settings.getInt(SignalPeriod);

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  
  settings.getData(FastMA, s);
  int fastma = maList.indexOf(s);

  settings.getData(SlowMA, s);
  int slowma = maList.indexOf(s);

  settings.getData(SignalMA, s);
  int signalma = maList.indexOf(s);

  PlotLine *macd = getMACD(in, fast, fastma, slow, slowma);
  if (! macd)
  {
    delete in;
    return 1;
  }

  PlotLine *sigma = getSignal(macd, signal, signalma);
  if (! sigma)
  {
    delete in;
    delete macd;
    return 1;
  }

  PlotLine *hist = getHistogram(macd, sigma);
  if (! hist)
  {
    delete in;
    delete macd;
    delete sigma;
    return 1;
  }

  // hist line
  settings.getData(HistColor, s);
  hist->setColor(s);
  settings.getData(HistPlot, s);
  hist->setPlugin(s);
  settings.getData(HistLabel, s);
  hist->setLabel(s);
  ind.addLine(hist);

  // macd line
  settings.getData(MACDColor, s);
  macd->setColor(s);
  settings.getData(MACDPlot, s);
  macd->setPlugin(s);
  settings.getData(MACDLabel, s);
  macd->setLabel(s);
  ind.addLine(macd);

  // signal line
  settings.getData(SignalColor, s);
  sigma->setColor(s);
  settings.getData(SignalPlot, s);
  sigma->setPlugin(s);
  settings.getData(SignalLabel, s);
  sigma->setLabel(s);
  ind.addLine(sigma);

  delete in;

  return 0;
}

int MACD::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,MACD,<INPUT>,<NAME_MACD>,<NAME_SIGNAL>,<NAME_HIST>,<FAST_PERIOD>,<FAST_MA_TYPE>,
  // <SLOW_PERIOD>,<SLOW_MA_TYPE>,<SIGNAL_PERIOD>,<SIGNAL_MA_TYPE>

  if (set.count() != 13)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[4];
    return 1;
  }

  tl = tlines.value(set[5]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[5];
    return 1;
  }

  tl = tlines.value(set[6]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[6];
    return 1;
  }

  PlotLine *in = tlines.value(set[3]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[3]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[3];
      return 1;
    }

    tlines.insert(set[3], in);
  }

  bool ok;
  int fast = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fast period" << set[7];
    return 1;
  }

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  int fastma = maList.indexOf(set[8]);
  if (fastma == -1)
  {
    qDebug() << indicator << "::calculate: invalid fast ma" << set[8];
    return 1;
  }

  int slow = set[9].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid slow period" << set[9];
    return 1;
  }

  int slowma = maList.indexOf(set[10]);
  if (slowma == -1)
  {
    qDebug() << indicator << "::calculate: invalid slow ma" << set[10];
    return 1;
  }

  int signal = set[11].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid signal period" << set[11];
    return 1;
  }

  int signalma = maList.indexOf(set[12]);
  if (signalma == -1)
  {
    qDebug() << indicator << "::calculate: invalid fast ma" << set[12];
    return 1;
  }

  PlotLine *macd = getMACD(in, fast, fastma, slow, slowma);
  if (! macd)
    return 1;
  
  tlines.insert(set[4], macd);

  PlotLine *sigma = getSignal(macd, signal, signalma);
  if (! sigma)
    return 1;

  tlines.insert(set[5], sigma);

  PlotLine *hist = getHistogram(macd, sigma);
  if (! hist)
    return 1;
  
  tlines.insert(set[6], hist);

  return 0;
}

PlotLine * MACD::getMACD (PlotLine *in, int fast, int fmaType, int slow, int smaType)
{
  if (in->count() < fast || in->count() < slow)
    return 0;
  
  MAUtils mau;
  PlotLine *fma = mau.getMA(in, fast, fmaType);
  if (! fma)
    return 0;

  PlotLine *sma = mau.getMA(in, slow, smaType);
  if (! sma)
  {
    delete fma;
    return 0;
  }

  int smaLoop = sma->count() - 1;
  int fmaLoop = fma->count() - 1;

  PlotLine *macd = new PlotLine;
  while (fmaLoop > -1 && smaLoop > -1)
  {
    macd->prepend(fma->getData(fmaLoop) - sma->getData(smaLoop));
    fmaLoop--;
    smaLoop--;
  }

  delete fma;
  delete sma;

  return macd;
}

PlotLine * MACD::getSignal (PlotLine *macd, int period, int maType)
{
  if (macd->count() < period)
    return 0;

  MAUtils mau;
  PlotLine *sig = mau.getMA(macd, period, maType);

  return sig;
}

PlotLine * MACD::getHistogram (PlotLine *macd, PlotLine *sig)
{
  PlotLine *hist = new PlotLine;
  int sigLoop = sig->count() - 1;
  int macdLoop = macd->count() - 1;

  while (sigLoop > -1 && macdLoop > -1)
  {
    hist->prepend(macd->getData(macdLoop) - sig->getData(sigLoop));
    sigLoop--;
    macdLoop--;
  }

  return hist;
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

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addIntItem(FastPeriod, page, QObject::tr("Fast Period"), settings.getInt(FastPeriod), 2, 100000);

  dialog->addIntItem(SlowPeriod, page, QObject::tr("Slow Period"), settings.getInt(SlowPeriod), 2, 100000);

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  
  settings.getData(FastMA, d);
  dialog->addComboItem(FastMA, page, QObject::tr("Fast MA"), maList, d);

  settings.getData(SlowMA, d);
  dialog->addComboItem(SlowMA, page, QObject::tr("Slow MA"), maList, d);

  page++;
  k = QObject::tr("MACD");
  dialog->addPage(page, k);

  settings.getData(MACDColor, d);
  dialog->addColorItem(MACDColor, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  settings.getData(MACDPlot, d);
  dialog->addComboItem(MACDPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MACDLabel, d);
  dialog->addTextItem(MACDLabel, page, QObject::tr("Label"), d);

  page++;
  k = QObject::tr("Signal");
  dialog->addPage(page, k);

  settings.getData(SignalColor, d);
  dialog->addColorItem(SignalColor, page, QObject::tr("Color"), d);

  settings.getData(SignalPlot, d);
  dialog->addComboItem(SignalPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(SignalLabel, d);
  dialog->addTextItem(SignalLabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(SignalPeriod, page, QObject::tr("Period"), settings.getInt(SignalPeriod), 1, 100000);

  settings.getData(SignalMA, d);
  dialog->addComboItem(SignalMA, page, QObject::tr("MA Type"), maList, d);

  page++;
  k = QObject::tr("Hist");
  dialog->addPage(page, k);

  settings.getData(HistColor, d);
  dialog->addColorItem(HistColor, page, QObject::tr("Color"), d);

  settings.getData(HistPlot, d);
  dialog->addComboItem(HistPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(HistLabel, d);
  dialog->addTextItem(HistLabel, page, QObject::tr("Label"), d);

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


