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
#include "MAFactory.h"
#include "BARSUtils.h"
#include "ta_libc.h"
#include "PlotFactory.h"

#include <QtDebug>
#include <QColor>

BBANDS::BBANDS ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("BBANDS::error on TA_Initialize");

  indicator = "BBANDS";

  settings.setData(UpColor, "red");
  settings.setData(MidColor, "red");
  settings.setData(DownColor, "red");
  settings.setData(UpPlot, "Line");
  settings.setData(MidPlot, "Line");
  settings.setData(DownPlot, "Line");
  settings.setData(UpLabel, "BBU");
  settings.setData(MidLabel, "BBM");
  settings.setData(DownLabel, "BBL");
  settings.setData(UpDeviation, 2);
  settings.setData(DownDeviation, 2);
  settings.setData(Input, "Close");
  settings.setData(Period, 20);
  settings.setData(MAType, "SMA");
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

  MAFactory mau;
  settings.getData(MAType, s);
  int maType = mau.typeFromString(s);

  // upper line
  settings.getData(UpColor, s);
  QColor ucolor(s);

  PlotFactory fac;
  settings.getData(UpPlot, s);
  int ulineType = fac.typeFromString(s);

  // middle line
  settings.getData(MidColor, s);
  QColor mcolor(s);

  settings.getData(MidPlot, s);
  int mlineType = fac.typeFromString(s);

  // lower line
  settings.getData(DownColor, s);
  QColor lcolor(s);

  settings.getData(DownPlot, s);
  int llineType = fac.typeFromString(s);

  QList<PlotLine *> pl;
  if (getBBANDS(in, period, udev, ldev, maType, ulineType, mlineType, llineType, ucolor, mcolor, lcolor, pl))
  {
    delete in;
    return 1;
  }

  PlotLine *line = pl.at(0);
  settings.getData(UpLabel, s);
  line->setLabel(s);
  ind.addLine(line);

  line = pl.at(1);
  settings.getData(MidLabel, s);
  line->setLabel(s);
  ind.addLine(line);

  line = pl.at(2);
  settings.getData(DownLabel, s);
  line->setLabel(s);
  ind.addLine(line);

  delete in;

  return 0;
}

int BBANDS::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,BBANDS,<INPUT>,<NAME UPPER>,<NAME MIDDLE>,<NAME LOWER>,<PERIOD>,<MA_TYPE>,<UP DEV>,<LOW DEV>,<UPPER PLOT TYPE>,<MIDDLE PLOT TYPE>,<LOWER PLOT TYPE>,<UPPER COLOR>,<MIDDLE COLOR>,<LOWER COLOR>
  //     0       1      2       3         4             5            6          7         8        9        10            11                12                   13             14             15             16

  if (set.count() != 17)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
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

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate upper name" << set[4];
    return 1;
  }

  tl = tlines.value(set[5]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate middle name" << set[5];
    return 1;
  }

  tl = tlines.value(set[6]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate lower name" << set[6];
    return 1;
  }

  bool ok;
  int period = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid period" << set[7];
    return 1;
  }

  MAFactory mau;
  int ma = mau.typeFromString(set[8]);
  if (ma == -1)
  {
    qDebug() << indicator << "::getCUS: invalid ma type" << set[8];
    return 1;
  }

  double udev = set[9].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid upper deviation" << set[9];
    return 1;
  }

  double ldev = set[10].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid lower deviation" << set[10];
    return 1;
  }

  PlotFactory fac;
  int ulineType = fac.typeFromString(set[11]);
  if (ulineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid upper plot type" << set[11];
    return 1;
  }

  int mlineType = fac.typeFromString(set[12]);
  if (mlineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid middle plot type" << set[12];
    return 1;
  }

  int llineType = fac.typeFromString(set[13]);
  if (llineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid lower plot type" << set[13];
    return 1;
  }

  QColor ucolor(set[14]);
  if (! ucolor.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid upper color" << set[14];
    return 1;
  }

  QColor mcolor(set[15]);
  if (! mcolor.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid middle color" << set[15];
    return 1;
  }

  QColor lcolor(set[16]);
  if (! lcolor.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid lower color" << set[16];
    return 1;
  }

  QList<PlotLine *> pl;
  if (getBBANDS(in, period, udev, ldev, ma, ulineType, mlineType, llineType, ucolor, mcolor, lcolor, pl))
    return 1;
  
  pl.at(0)->setLabel(set[4]);
  pl.at(1)->setLabel(set[5]);
  pl.at(2)->setLabel(set[6]);

  tlines.insert(set[4], pl.at(0));
  tlines.insert(set[5], pl.at(1));
  tlines.insert(set[6], pl.at(2));

  return 0;
}

int BBANDS::getBBANDS (PlotLine *in, int period, double udev, double ddev, int maType, int ulineType,
                        int mlineType, int llineType, QColor &ucolor, QColor &mcolor, QColor &lcolor,
                        QList<PlotLine *> &rl)
{
  int size = in->count();  
  TA_Real input[size];
  TA_Real uout[size];
  TA_Real mout[size];
  TA_Real lout[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  in->keys(keys);
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_BBANDS(0,
                            keys.count() - 1,
                            &input[0],
                            period,
                            udev,
                            ddev,
                            (TA_MAType) maType,
                            &outBeg,
                            &outNb,
                            &uout[0],
                            &mout[0],
                            &lout[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::getBBANDS: TA-Lib error" << rc;
    return 1;
  }

  PlotFactory fac;
  PlotLine *upper = fac.plot(ulineType);
  if (! upper)
    return 1;

  PlotLine *middle = fac.plot(mlineType);
  if (! middle)
  {
    delete upper;
    return 1;
  }

  PlotLine *lower = fac.plot(llineType);
  if (! lower)
  {
    delete upper;
    delete middle;
    return 1;
  }

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    upper->setData(keys.at(keyLoop), new PlotLineBar(ucolor, uout[outLoop]));
    middle->setData(keys.at(keyLoop), new PlotLineBar(mcolor, mout[outLoop]));
    lower->setData(keys.at(keyLoop), new PlotLineBar(lcolor, lout[outLoop]));

    keyLoop--;
    outLoop--;
  }

  rl.append(upper);
  rl.append(middle);
  rl.append(lower);
  
  return 0;
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
  
  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 2, 100000);

  dialog->addDoubleItem(UpDeviation, page, QObject::tr("Upper Deviation"), settings.getDouble(UpDeviation), -100000, 100000);

  dialog->addDoubleItem(DownDeviation, page, QObject::tr("Lower Deviation"), settings.getDouble(DownDeviation), -100000, 100000);

  QStringList maList;
  MAFactory mau;
  mau.list(maList);
  
  settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("MA Type"), maList, d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

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


