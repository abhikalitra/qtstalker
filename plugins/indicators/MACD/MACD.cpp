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
#include "MAFactory.h"
#include "PlotFactory.h"

#include <QtDebug>

MACD::MACD ()
{
  indicator = "MACD";

  settings.setData(MACDColor, "red");
  settings.setData(SignalColor, "yellow");
  settings.setData(HistColor, "blue");
  settings.setData(MACDPlot, "Line");
  settings.setData(SignalPlot, "Line");
  settings.setData(HistPlot, "Histogram");
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

  MAFactory mau;
  settings.getData(FastMA, s);
  int fastma = mau.typeFromString(s);

  settings.getData(SlowMA, s);
  int slowma = mau.typeFromString(s);

  settings.getData(SignalMA, s);
  int signalma = mau.typeFromString(s);

  // macd line
  settings.getData(MACDColor, s);
  QColor color(s);

  PlotFactory fac;
  settings.getData(MACDPlot, s);
  int lineType = fac.typeFromString(s);

  PlotLine *macd = getMACD(in, fast, fastma, slow, slowma, lineType, color);
  if (! macd)
  {
    delete in;
    return 1;
  }

  settings.getData(MACDLabel, s);
  macd->setLabel(s);

  // signal line
  settings.getData(SignalColor, s);
  color.setNamedColor(s);

  settings.getData(SignalPlot, s);
  lineType = fac.typeFromString(s);

  PlotLine *sigma = getSignal(macd, signal, signalma, lineType, color);
  if (! sigma)
  {
    delete in;
    return 1;
  }

  settings.getData(SignalLabel, s);
  sigma->setLabel(s);

  // hist line
  settings.getData(HistColor, s);
  color.setNamedColor(s);

  settings.getData(HistPlot, s);
  lineType = fac.typeFromString(s);

  PlotLine *hist = getHistogram(macd, sigma, lineType, color);
  if (! hist)
  {
    delete in;
    return 1;
  }

  settings.getData(HistLabel, s);
  hist->setLabel(s);

  ind.addLine(hist);
  ind.addLine(macd);
  ind.addLine(sigma);

  delete in;

  return 0;
}

int MACD::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,MACD,<INPUT>,<NAME_MACD>,<NAME_SIGNAL>,<NAME_HIST>,<FAST_PERIOD>,<FAST_MA_TYPE>,<SLOW_PERIOD>,<SLOW_MA_TYPE>,<SIGNAL_PERIOD>,<SIGNAL_MA_TYPE>,<MACD PLOT TYPE>,<SIGNAL PLOT TYPE>,<HIST PLOT TYPE>,<MACD COLOR>,<SIGNAL COLOR>,<HIST COLOR>
  //     0       1     2      3         4           5             6           7               8            9             10                11              12                13               14                 15           16             17           18

  if (set.count() != 19)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[4];
    return 1;
  }

  tl = tlines.value(set[5]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[5];
    return 1;
  }

  tl = tlines.value(set[6]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[6];
    return 1;
  }

  PlotLine *in = tlines.value(set[3]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[3]));
    if (! in)
    {
      qDebug() << indicator << "::getCUS: input not found" << set[3];
      return 1;
    }

    tlines.insert(set[3], in);
  }

  bool ok;
  int fast = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid fast period" << set[7];
    return 1;
  }

  MAFactory mau;
  int fastma = mau.typeFromString(set[8]);
  if (fastma == -1)
  {
    qDebug() << indicator << "::getCUS: invalid fast ma" << set[8];
    return 1;
  }

  int slow = set[9].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid slow period" << set[9];
    return 1;
  }

  int slowma = mau.typeFromString(set[10]);
  if (slowma == -1)
  {
    qDebug() << indicator << "::getCUS: invalid slow ma" << set[10];
    return 1;
  }

  int signal = set[11].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid signal period" << set[11];
    return 1;
  }

  int signalma = mau.typeFromString(set[12]);
  if (signalma == -1)
  {
    qDebug() << indicator << "::getCUS: invalid fast ma" << set[12];
    return 1;
  }

  PlotFactory fac;
  int mlineType = fac.typeFromString(set[13]);
  if (mlineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid macd plot type" << set[13];
    return 1;
  }

  int slineType = fac.typeFromString(set[14]);
  if (slineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid signal plot type" << set[14];
    return 1;
  }

  int hlineType = fac.typeFromString(set[15]);
  if (hlineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid macd plot type" << set[15];
    return 1;
  }

  QColor mcolor(set[16]);
  if (! mcolor.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid macd color" << set[16];
    return 1;
  }

  QColor scolor(set[17]);
  if (! scolor.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid signal color" << set[17];
    return 1;
  }

  QColor hcolor(set[18]);
  if (! hcolor.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid hist color" << set[18];
    return 1;
  }

  PlotLine *macd = getMACD(in, fast, fastma, slow, slowma, mlineType, mcolor);
  if (! macd)
    return 1;
  
  macd->setLabel(set[4]);

  tlines.insert(set[4], macd);

  PlotLine *sigma = getSignal(macd, signal, signalma, slineType, scolor);
  if (! sigma)
    return 1;

  sigma->setLabel(set[5]);

  tlines.insert(set[5], sigma);

  PlotLine *hist = getHistogram(macd, sigma, hlineType, hcolor);
  if (! hist)
    return 1;
  
  hist->setLabel(set[6]);

  tlines.insert(set[6], hist);

  return 0;
}

PlotLine * MACD::getMACD (PlotLine *in, int fast, int fmaType, int slow, int smaType, int lineType, QColor &color)
{
  if (in->count() < fast || in->count() < slow)
    return 0;
  
  MAFactory mau;
  PlotLine *fma = mau.ma(in, fast, fmaType, lineType, color);
  if (! fma)
    return 0;

  PlotLine *sma = mau.ma(in, slow, smaType, lineType, color);
  if (! sma)
  {
    delete fma;
    return 0;
  }

  QList<int> keys;
  sma->keys(keys);
  
  PlotFactory fac;
  PlotLine *macd = fac.plot(lineType);
  if (! macd)
  {
    delete fma;
    delete sma;
    return 0;
  }

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *fbar = fma->data(keys.at(loop));
    if (! fbar)
      continue;
    
    PlotLineBar *sbar = sma->data(keys.at(loop));
    
    macd->setData(keys.at(loop), new PlotLineBar(color, fbar->data() - sbar->data()));
  }

  delete fma;
  delete sma;

  return macd;
}

PlotLine * MACD::getSignal (PlotLine *macd, int period, int maType, int lineType, QColor &color)
{
  if (macd->count() < period)
    return 0;

  MAFactory mau;
  PlotLine *sig = mau.ma(macd, period, maType, lineType, color);

  return sig;
}

PlotLine * MACD::getHistogram (PlotLine *macd, PlotLine *sig, int lineType, QColor &color)
{
  PlotFactory fac;
  PlotLine *hist = fac.plot(lineType);
  if (! hist)
    return 0;

  QList<int> keys;
  sig->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *mbar = macd->data(keys.at(loop));
    if (! mbar)
      continue;

    PlotLineBar *sbar = sig->data(keys.at(loop));
    
    hist->setData(keys.at(loop), new PlotLineBar(color, mbar->data() - sbar->data()));
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
  MAFactory mau;
  mau.list(maList);
  
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


