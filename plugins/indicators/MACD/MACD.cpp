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
#include "ta_libc.h"

#include <QtDebug>


MACD::MACD ()
{
  indicator = "MACD";

  settings.setData(MACDColor, "red");
  settings.setData(SignalColor, "yellow");
  settings.setData(HistColor, "blue");
  settings.setData(MACDPlot, "Line");
  settings.setData(SignalPlot, "Dash");
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

  QStringList maList;
  getMAList(maList);
  
  settings.getData(FastMA, s);
  int fastma = maList.indexOf(s);

  settings.getData(SlowMA, s);
  int slowma = maList.indexOf(s);

  settings.getData(SignalMA, s);
  int signalma = maList.indexOf(s);

  QList<PlotLine *> l;
  int rc = getMACD(in, fast, fastma, slow, slowma, signal, signalma, l);
  if (rc || l.count() != 3)
  {
    qDeleteAll(l);
    delete in;
    return 1;
  }

  // hist line first so we dont blot out the macd and  signal lines
  PlotLine *line = l.at(2);
  settings.getData(HistColor, s);
  line->setColor(s);
  settings.getData(HistPlot, s);
  line->setPlugin(s);
  settings.getData(HistLabel, s);
  line->setLabel(s);
  ind.addLine(line);

  // macd line
  line = l.at(0);
  settings.getData(MACDColor, s);
  line->setColor(s);
  settings.getData(MACDPlot, s);
  line->setPlugin(s);
  settings.getData(MACDLabel, s);
  line->setLabel(s);
  ind.addLine(line);

  // signal line
  line = l.at(1);
  settings.getData(SignalColor, s);
  line->setColor(s);
  settings.getData(SignalPlot, s);
  line->setPlugin(s);
  settings.getData(SignalLabel, s);
  line->setLabel(s);
  ind.addLine(line);

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
  getMAList(maList);
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

  QList<PlotLine *> l;
  int rc = getMACD(in, fast, fastma, slow, slowma, signal, signalma, l);
  if (rc)
  {
    qDeleteAll(l);
    return 1;
  }

  tlines.insert(set[4], l.at(0));
  tlines.insert(set[5], l.at(1));
  tlines.insert(set[6], l.at(2));

  return 0;
}

int MACD::getMACD (PlotLine *in, int fast, int fma, int slow, int sma, int signal,
			 int sigma, QList<PlotLine *> &l)
{
  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_MACDEXT(0, size - 1, &input[0], fast, (TA_MAType) fma,
			     slow, (TA_MAType) sma, signal, (TA_MAType) sigma,
			     &outBeg, &outNb, &out[0], &out2[0], &out3[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *sig = new PlotLine;
  PlotLine *hist = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    line->append(out[loop]);
    sig->append(out2[loop]);
    hist->append(out3[loop]);
  }

  l.append(line);
  l.append(sig);
  l.append(hist);

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

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addIntItem(FastPeriod, page, QObject::tr("Fast Period"), settings.getInt(FastPeriod), 2, 100000);

  dialog->addIntItem(SlowPeriod, page, QObject::tr("Slow Period"), settings.getInt(SlowPeriod), 2, 100000);

  QStringList maList;
  getMAList(maList);
  
  settings.getData(FastMA, d);
  dialog->addComboItem(FastMA, page, QObject::tr("Fast MA"), maList, d);

  settings.getData(SlowMA, d);
  dialog->addComboItem(SlowMA, page, QObject::tr("Slow MA"), maList, d);

  page++;
  k = QObject::tr("MACD");
  dialog->addPage(page, k);

  settings.getData(MACDColor, d);
  dialog->addColorItem(MACDColor, page, QObject::tr("Color"), d);

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


