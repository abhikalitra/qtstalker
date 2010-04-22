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
#include "MAUtils.h"
#include "BARSUtils.h"
#include "STDDEV.h"
#include "ADD.h"
#include "SUB.h"

#include <QtDebug>
#include <QColor>

BBANDS::BBANDS ()
{
  indicator = "BBANDS";

  settings.setData(UpColor, "red");
  settings.setData(MidColor, "red");
  settings.setData(DownColor, "red");
  settings.setData(UpPlot, "Line");
  settings.setData(MidPlot, "Line");
  settings.setData(DownPlot, "Line");
  settings.setData(UpLabel, "BBANDSU");
  settings.setData(MidLabel, "BBANDSM");
  settings.setData(DownLabel, "BBANDSL");
  settings.setData(UpDeviation, 2);
  settings.setData(DownDeviation, 2);
  settings.setData(Input, "Close");
  settings.setData(Period, 20);
  settings.setData(MAType, "SMA");

  methodList << "Upper" << "Middle" << "Lower";
}

int BBANDS::getIndicator (Indicator &ind, BarData *data)
{
  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  BARSUtils b;
  PlotLine *bars = b.getBARS(data, up, down, neutral);
  if (! bars)
    return 1;

  ind.addLine(bars);

  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);
  double udev = settings.getDouble(UpDeviation);
  double ldev = settings.getDouble(DownDeviation);

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  
  settings.getData(MAType, s);
  int ma = maList.indexOf(s);

  PlotLine *middle = getMiddle(in, period, ma);
  if (! middle)
  {
    delete in;
    return 1;
  }

  // middle line
  settings.getData(MidColor, s);
  middle->setColor(s);
  settings.getData(MidPlot, s);
  middle->setPlugin(s);
  settings.getData(MidLabel, s);
  middle->setLabel(s);
  ind.addLine(middle);

  PlotLine *upper = getUpper(in, middle, period, udev);
  if (! upper)
  {
    delete in;
    return 1;
  }

  // upper line
  settings.getData(UpColor, s);
  upper->setColor(s);
  settings.getData(UpPlot, s);
  upper->setPlugin(s);
  settings.getData(UpLabel, s);
  upper->setLabel(s);
  ind.addLine(upper);

  PlotLine *lower = getLower(in, middle, period, ldev);
  if (! lower)
  {
    delete in;
    return 1;
  }

  // lower line
  settings.getData(DownColor, s);
  lower->setColor(s);
  settings.getData(DownPlot, s);
  lower->setPlugin(s);
  settings.getData(DownLabel, s);
  lower->setLabel(s);
  ind.addLine(lower);

  delete in;

  return 0;
}

int BBANDS::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,BBANDS,METHOD,*
  //     0       1      2      3

  int method = methodList.indexOf(set[3]);
  int rc = 1;
  switch ((Method) method)
  {
    case Upper:
      rc = getCUSUpper(set, tlines, data);
      break;
    case Middle:
      rc = getCUSMiddle(set, tlines, data);
      break;
    case Lower:
      rc = getCUSUpper(set, tlines, data);
      break;
  }

  return rc;
}

int BBANDS::getCUSMiddle (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,BBANDS,Middle,<NAME>,<INPUT>,<PERIOD>,<MA_TYPE>
  //     0       1      2      3      4       5       6        7

  if (set.count() != 8)
  {
    qDebug() << indicator << "::getCUSMiddle: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUSMiddle: duplicate name" << set[4];
    return 1;
  }

  PlotLine *in = tlines.value(set[5]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[5]));
    if (! in)
    {
      qDebug() << indicator << "::getCUSMiddle: input not found" << set[5];
      return 1;
    }

    tlines.insert(set[5], in);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUSMiddle: invalid period settings" << set[6];
    return 1;
  }

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  int ma = maList.indexOf(set[7]);
  if (ma == -1)
  {
    qDebug() << indicator << "::getCUSMiddle: invalid ma settings" << set[7];
    return 1;
  }

  PlotLine *line = getMiddle(in, period, ma);
  if (! line)
    return 1;
  
  tlines.insert(set[4], line);

  return 0;
}

int BBANDS::getCUSUpper (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,BBANDS,Upper,<NAME>,<INPUT>,<PERIOD>,<MA_TYPE>,<DEVIATION>
  //     0       1      2      3     4       5        6        7         8

  if (set.count() != 9)
  {
    qDebug() << indicator << "::getCUSUpper: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUSUpper: duplicate name" << set[4];
    return 1;
  }

  PlotLine *in = tlines.value(set[5]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[5]));
    if (! in)
    {
      qDebug() << indicator << "::getCUSUpper: input not found" << set[5];
      return 1;
    }

    tlines.insert(set[5], in);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUSUpper: invalid period settings" << set[6];
    return 1;
  }

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  int ma = maList.indexOf(set[7]);
  if (ma == -1)
  {
    qDebug() << indicator << "::getCUSUpper: invalid ma settings" << set[7];
    return 1;
  }

  double dev = set[8].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUSUpper: invalid deviation setting" << set[8];
    return 1;
  }

  PlotLine *middle = getMiddle(in, period, ma);
  if (! middle)
    return 1;

  PlotLine *line = getUpper(in, middle, period, dev);
  if (! line)
  {
    delete middle;
    return 1;
  }

  tlines.insert(set[4], line);

  return 0;
}

int BBANDS::getCUSLower (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,BBANDS,Lower,<NAME>,<INPUT>,<PERIOD>,<MA_TYPE>,<DEVIATION>
  //     0       1      2      3     4       5        6        7         8

  if (set.count() != 9)
  {
    qDebug() << indicator << "::getCUSLower: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUSLower: duplicate name" << set[4];
    return 1;
  }

  PlotLine *in = tlines.value(set[5]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[5]));
    if (! in)
    {
      qDebug() << indicator << "::getCUSLower: input not found" << set[5];
      return 1;
    }

    tlines.insert(set[5], in);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUSLower: invalid period settings" << set[6];
    return 1;
  }

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  int ma = maList.indexOf(set[7]);
  if (ma == -1)
  {
    qDebug() << indicator << "::getCUSLower: invalid ma settings" << set[7];
    return 1;
  }

  double dev = set[8].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUSLower: invalid deviation setting" << set[8];
    return 1;
  }

  PlotLine *middle = getMiddle(in, period, ma);
  if (! middle)
    return 1;

  PlotLine *line = getLower(in, middle, period, dev);
  if (! line)
  {
    delete middle;
    return 1;
  }

  tlines.insert(set[4], line);

  return 0;
}

PlotLine * BBANDS::getUpper (PlotLine *in, PlotLine *middle, int period, double dev)
{
  STDDEV s;
  PlotLine *std = s.getSTDDEV(in, period, dev);
  if (! std)
    return 0;

  ADD add;
  PlotLine *upper = add.getADD(middle, std);

  delete std;
  
  return upper;
}

PlotLine * BBANDS::getLower (PlotLine *in, PlotLine *middle, int period, double dev)
{
  STDDEV s;
  PlotLine *std = s.getSTDDEV(in, period, dev);
  if (! std)
    return 0;

  SUB sub;
  PlotLine *lower = sub.getSUB(middle, std);

  delete std;

  return lower;
}

PlotLine * BBANDS::getMiddle (PlotLine *in, int period, int maType)
{
  MAUtils mau;
  PlotLine *line = mau.getMA(in, period, maType);
  return line;
}

int BBANDS::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 2, 100000);

  dialog->addDoubleItem(UpDeviation, page, QObject::tr("Upper Deviation"), settings.getDouble(UpDeviation), -100000, 100000);

  dialog->addDoubleItem(DownDeviation, page, QObject::tr("Lower Deviation"), settings.getDouble(DownDeviation), -100000, 100000);

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  
  settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("MA Type"), maList, d);

  page++;
  k = QObject::tr("Upper");
  dialog->addPage(page, k);

  settings.getData(UpColor, d);
  dialog->addColorItem(UpColor, page, QObject::tr("Color"), d);

  settings.getData(UpPlot, d);
  dialog->addComboItem(UpPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(UpLabel, d);
  dialog->addTextItem(UpLabel, page, QObject::tr("Label"), d);

  page++;
  k = QObject::tr("Mid");
  dialog->addPage(page, k);

  settings.getData(MidColor, d);
  dialog->addColorItem(MidColor, page, QObject::tr("Color"), d);

  settings.getData(MidPlot, d);
  dialog->addComboItem(MidPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MidLabel, d);
  dialog->addTextItem(MidLabel, page, QObject::tr("Label"), d);

  page++;
  k = QObject::tr("Lower");
  dialog->addPage(page, k);

  settings.getData(DownColor, d);
  dialog->addColorItem(DownColor, page, QObject::tr("Color"), d);

  settings.getData(DownPlot, d);
  dialog->addComboItem(DownPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(DownLabel, d);
  dialog->addTextItem(DownLabel, page, QObject::tr("Label"), d);

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


