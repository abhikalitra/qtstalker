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
#include "FunctionBARS.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>
#include <QColor>

BBANDS::BBANDS ()
{
  _indicator = "BBANDS";
}

int BBANDS::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QString s;
  settings.getData(_BarsUpColor, s);
  QColor up(s);
  
  settings.getData(_BarsDownColor, s);
  QColor down(s);
  
  settings.getData(_BarsNeutralColor, s);
  QColor neutral(s);
  
  FunctionBARS b;
  Curve *bars = b.getBARS(up, down, neutral, data);
  if (! bars)
    return 1;

  bars->setZ(0);
  ind.setLine(0, bars);

  settings.getData(_Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(_Period);
  double udev = settings.getDouble(_UpDeviation);
  double ldev = settings.getDouble(_DownDeviation);

  FunctionMA mau;
  settings.getData(_MAType, s);
  int maType = mau.typeFromString(s);

  QList<Curve *> pl;
  if (calculate(in, period, udev, ldev, maType, pl))
  {
    delete in;
    return 1;
  }

  Curve *line = pl.at(0);

  settings.getData(_UpPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_UpColor, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(_UpLabel, s);
  line->setLabel(s);
  
  line->setZ(1);
  ind.setLine(s, line);

  line = pl.at(1);

  settings.getData(_MidPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_MidColor, s);
  c.setNamedColor(s);
  line->setColor(c);

  settings.getData(_MidLabel, s);
  line->setLabel(s);
  
  line->setZ(2);
  ind.setLine(s, line);

  line = pl.at(2);

  settings.getData(_DownPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_DownColor, s);
  c.setNamedColor(s);
  line->setColor(c);

  settings.getData(_DownLabel, s);
  line->setLabel(s);
  
  line->setZ(3);
  ind.setLine(s, line);

  delete in;

  return 0;
}

int BBANDS::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,BBANDS,<INPUT>,<NAME UPPER>,<NAME MIDDLE>,<NAME LOWER>,<PERIOD>,<MA_TYPE>,<UP DEV>,<LOW DEV>
  //     0       1      2       3         4             5            6          7         8        9        10

  if (set.count() != 11)
  {
    qDebug() << "BBANDS::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *in = ind.line(set[3]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[3]));
    if (! in)
    {
      qDebug() << "BBANDS::getCUS: input not found" << set[3];
      return 1;
    }

    ind.setLine(set[3], in);
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "BBANDS::getCUS: duplicate upper name" << set[4];
    return 1;
  }

  tl = ind.line(set[5]);
  if (tl)
  {
    qDebug() << "BBANDS::getCUS: duplicate middle name" << set[5];
    return 1;
  }

  tl = ind.line(set[6]);
  if (tl)
  {
    qDebug() << "BBANDS::getCUS: duplicate lower name" << set[6];
    return 1;
  }

  bool ok;
  int period = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << "BBANDS::getCUS: invalid period" << set[7];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[8]);
  if (ma == -1)
  {
    qDebug() << "BBANDS::getCUS: invalid ma type" << set[8];
    return 1;
  }

  double udev = set[9].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "BBANDS::getCUS: invalid upper deviation" << set[9];
    return 1;
  }

  double ldev = set[10].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "BBANDS::getCUS: invalid lower deviation" << set[10];
    return 1;
  }

  QList<Curve *> pl;
  if (calculate(in, period, udev, ldev, ma, pl))
    return 1;

  pl.at(0)->setLabel(set[4]);
  pl.at(1)->setLabel(set[5]);
  pl.at(2)->setLabel(set[6]);

  ind.setLine(set[4], pl.at(0));
  ind.setLine(set[5], pl.at(1));
  ind.setLine(set[6], pl.at(2));

  return 0;
}

IndicatorPluginDialog * BBANDS::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  Setting _settings = i.settings();

  // general tab
  int tab = dialog->addTab(tr("General"));

  QString d;
  _settings.getData(_Input, d);
  dialog->addInput(tab, _Input, tr("Input"), d);

  dialog->addInt(tab, _Period, tr("Period"), _settings.getInt(_Period), 100000, 2);

  dialog->addDouble(tab, _UpDeviation, tr("Upper Deviation"), _settings.getDouble(_UpDeviation), 100000, -100000);

  dialog->addDouble(tab, _DownDeviation, tr("Lower Deviation"), _settings.getDouble(_DownDeviation), 100000, -100000);

  FunctionMA mau;
  QStringList l = mau.list();

  _settings.getData(_MAType, d);
  dialog->addCombo(tab, _MAType, tr("Type"), l, d);

  // upper tab
  tab = dialog->addTab(tr("Upper"));

  _settings.getData(_UpColor, d);
  dialog->addColor(tab, _UpColor, tr("Color"), d);

  _settings.getData(_UpPlot, d);
  dialog->addPlot(tab, _UpPlot, tr("Plot"), d);

  _settings.getData(_UpLabel, d);
  dialog->addText(tab, _UpLabel, tr("Label"), d);

  // middle tab
  tab = dialog->addTab(tr("Middle"));

  _settings.getData(_MidColor, d);
  dialog->addColor(tab, _MidColor, tr("Color"), d);

  _settings.getData(_MidPlot, d);
  dialog->addPlot(tab, _MidPlot, tr("Plot"), d);

  _settings.getData(_MidLabel, d);
  dialog->addText(tab, _MidLabel, tr("Label"), d);

  // lower tab
  tab = dialog->addTab(tr("Lower"));

  _settings.getData(_DownColor, d);
  dialog->addColor(tab, _DownColor, tr("Color"), d);

  _settings.getData(_DownPlot, d);
  dialog->addPlot(tab, _DownPlot, tr("Plot"), d);

  _settings.getData(_DownLabel, d);
  dialog->addText(tab, _DownLabel, tr("Label"), d);

  return dialog;
}

void BBANDS::defaults (Indicator &i)
{
  Setting set;
  set.setData(_UpColor, "red");
  set.setData(_MidColor, "red");
  set.setData(_DownColor, "red");
  set.setData(_UpPlot, "Line");
  set.setData(_MidPlot, "Line");
  set.setData(_DownPlot, "Line");
  set.setData(_UpLabel, "BBU");
  set.setData(_MidLabel, "BBM");
  set.setData(_DownLabel, "BBL");
  set.setData(_UpDeviation, 2);
  set.setData(_DownDeviation, 2);
  set.setData(_Input, "Close");
  set.setData(_Period, 20);
  set.setData(_MAType, "SMA");
  set.setData(_BarsUpColor, "green");
  set.setData(_BarsDownColor, "red");
  set.setData(_BarsNeutralColor, "blue");
  set.setData(_BarsLabel, "BARS");
  i.setSettings(set);
}

void BBANDS::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  
  settings.getData(_UpLabel, s);
  l.append(s);

  settings.getData(_MidLabel, s);
  l.append(s);

  settings.getData(_DownLabel, s);
  l.append(s);
}

int BBANDS::calculate (Curve *in, int period, double udev, double ddev, int maType, QList<Curve *> &rl)
{
  if (in->count() < period)
    return 1;

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
    CurveBar *bar = in->bar(keys.at(loop));
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
    qDebug() << "BBANDS::getBBANDS: TA-Lib error" << rc;
    return 1;
  }

  Curve *upper = new Curve;
  Curve *middle = new Curve;
  Curve *lower = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    upper->setBar(keys.at(keyLoop), new CurveBar(uout[outLoop]));
    middle->setBar(keys.at(keyLoop), new CurveBar(mout[outLoop]));
    lower->setBar(keys.at(keyLoop), new CurveBar(lout[outLoop]));

    keyLoop--;
    outLoop--;
  }

  rl.append(upper);
  rl.append(middle);
  rl.append(lower);

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  BBANDS *o = new BBANDS;
  return ((IndicatorPlugin *) o);
}


