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
#include "ta_libc.h"

#include <QtDebug>

MACD::MACD ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("MACD::error on TA_Initialize");

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

  MAFactory mau;
  _settings.getData(FastMA, s);
  int fastma = mau.typeFromString(s);

  _settings.getData(SlowMA, s);
  int slowma = mau.typeFromString(s);

  _settings.getData(SignalMA, s);
  int signalma = mau.typeFromString(s);

  // macd line
  _settings.getData(MACDColor, s);
  QColor macdColor(s);

  PlotFactory fac;
  _settings.getData(MACDPlot, s);
  int macdPlot = fac.typeFromString(s);

  _settings.getData(SignalColor, s);
  QColor signalColor(s);

  _settings.getData(SignalPlot, s);
  int signalPlot = fac.typeFromString(s);

  _settings.getData(HistColor, s);
  QColor histColor(s);

  _settings.getData(HistPlot, s);
  int histPlot = fac.typeFromString(s);

  QList<PlotLine *> pl;
  if (getMACD(in,
              fast,
              fastma,
              slow,
              slowma,
              signal,
              signalma,
              macdPlot,
              macdColor,
              signalPlot,
              signalColor,
              histPlot,
              histColor,
              pl))
  {
    delete in;
    return 1;
  }

  // plot hist 
  PlotLine *line = pl.at(2);
  _settings.getData(HistLabel, s);
  line->setLabel(s);
  
  s = "0";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  // plot macd
  line = pl.at(0);
  _settings.getData(MACDLabel, s);
  line->setLabel(s);

  s = "1";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  // plot signal
  line = pl.at(1);
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
  // INDICATOR,PLUGIN,MACD,<INPUT>,<NAME_MACD>,<NAME_SIGNAL>,<NAME_HIST>,<FAST_PERIOD>,<FAST_MA_TYPE>,<SLOW_PERIOD>,<SLOW_MA_TYPE>,<SIGNAL_PERIOD>,<SIGNAL_MA_TYPE>,<MACD PLOT TYPE>,<SIGNAL PLOT TYPE>,<HIST PLOT TYPE>,<MACD COLOR>,<SIGNAL COLOR>,<HIST COLOR>
  //     0       1     2      3         4           5             6           7               8            9             10                11              12                13               14                 15           16             17           18

  if (set.count() != 19)
  {
    qDebug() << _indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[4];
    return 1;
  }

  tl = ind.line(set[5]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[5];
    return 1;
  }

  tl = ind.line(set[6]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[6];
    return 1;
  }

  PlotLine *in = ind.line(set[3]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[3]));
    if (! in)
    {
      qDebug() << _indicator << "::getCUS: input not found" << set[3];
      return 1;
    }

    ind.setLine(set[3], in);
  }

  bool ok;
  int fast = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid fast period" << set[7];
    return 1;
  }

  MAFactory mau;
  int fastma = mau.typeFromString(set[8]);
  if (fastma == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid fast ma" << set[8];
    return 1;
  }

  int slow = set[9].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid slow period" << set[9];
    return 1;
  }

  int slowma = mau.typeFromString(set[10]);
  if (slowma == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid slow ma" << set[10];
    return 1;
  }

  int signal = set[11].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid signal period" << set[11];
    return 1;
  }

  int signalma = mau.typeFromString(set[12]);
  if (signalma == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid fast ma" << set[12];
    return 1;
  }

  PlotFactory fac;
  int mlineType = fac.typeFromString(set[13]);
  if (mlineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid macd plot type" << set[13];
    return 1;
  }

  int slineType = fac.typeFromString(set[14]);
  if (slineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid signal plot type" << set[14];
    return 1;
  }

  int hlineType = fac.typeFromString(set[15]);
  if (hlineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid macd plot type" << set[15];
    return 1;
  }

  QColor mcolor(set[16]);
  if (! mcolor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid macd color" << set[16];
    return 1;
  }

  QColor scolor(set[17]);
  if (! scolor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid signal color" << set[17];
    return 1;
  }

  QColor hcolor(set[18]);
  if (! hcolor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid hist color" << set[18];
    return 1;
  }

  QList<PlotLine *> pl;
  if (getMACD(in,
              fast,
              fastma,
              slow,
              slowma,
              signal,
              signalma,
              mlineType,
              mcolor,
              slineType,
              scolor,
              hlineType,
              hcolor,
              pl))
    return 1;

  PlotLine *line = pl.at(0);
  line->setLabel(set[4]);
  ind.setLine(set[4], line);
  
  line = pl.at(1);
  line->setLabel(set[5]);
  ind.setLine(set[5], line);

  line = pl.at(2);
  line->setLabel(set[6]);
  ind.setLine(set[6], line);

  return 0;
}

int MACD::getMACD (PlotLine *in, int fastPeriod, int fastMA, int slowPeriod, int slowMA, int signalPeriod,
                   int signalMA, int macdPlot, QColor &macdColor, int signalPlot, QColor &signalColor,
                   int histPlot, QColor &histColor, QList<PlotLine *> &pl)
{
  if (in->count() < fastPeriod || in->count() < slowPeriod || in->count() < signalPeriod)
    return 1;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar bar;
    in->data(keys.at(loop), bar);
    input[loop] = (TA_Real) bar.data();
  }

  TA_RetCode rc = TA_MACDEXT(0,
                             size - 1,
                             &input[0],
                             fastPeriod,
                             (TA_MAType) fastMA,
                             slowPeriod,
                             (TA_MAType) slowMA,
                             signalPeriod,
                             (TA_MAType) signalMA,
                             &outBeg,
                             &outNb,
                             &out[0],
                             &out2[0],
                             &out3[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getMACD: TA-Lib error" << rc;
    return 1;
  }

  PlotFactory fac;
  PlotLine *macd = fac.plot(macdPlot);
  if (! macd)
    return 1;

  PlotLine *signal = fac.plot(signalPlot);
  if (! signal)
  {
    delete macd;
    return 1;
  }

  PlotLine *hist = fac.plot(histPlot);
  if (! hist)
  {
    delete macd;
    delete signal;
    return 1;
  }

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    PlotLineBar bar(macdColor, out[outLoop]);
    macd->setData(keys.at(keyLoop), bar);
    
    PlotLineBar bar2(signalColor, out2[outLoop]);
    signal->setData(keys.at(keyLoop), bar2);
    
    PlotLineBar bar3(histColor, out3[outLoop]);
    hist->setData(keys.at(keyLoop), bar3);
    keyLoop--;
    outLoop--;
  }

  pl.append(macd);
  pl.append(signal);
  pl.append(hist);

  return 0;
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

  MAFactory mau;
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

  PlotFactory fac;
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


