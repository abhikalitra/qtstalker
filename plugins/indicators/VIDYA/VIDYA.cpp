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

#include "VIDYA.h"
#include "PlotFactory.h"
#include "ta_libc.h"
#include "BARSUtils.h"

#include <cmath>
#include <QVector>
#include <QtDebug>

#define PI 3.14159265

VIDYA::VIDYA ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("VIDYA::error on TA_Initialize");

  _indicator = "VIDYA";

  _settings.setData(Color, "red");
  _settings.setData(Plot, "Line");
  _settings.setData(Label, _indicator);
  _settings.setData(Period, 14);
  _settings.setData(Input, "Close");
  _settings.setData(VPeriod, 10);
}

int VIDYA::getIndicator (Indicator &ind, BarData &data)
{
  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  BARSUtils b;
  PlotLine *bars = b.getBARS(data, up, down, neutral);
  if (bars)
  {
    QString s = "0";
    ind.setLine(s, bars);
    ind.addPlotOrder(s);
  }
  
  QString s;
  _settings.getData(Input, s);
  PlotLine *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  int period = _settings.getInt(Period);
  int volPeriod = _settings.getInt(VPeriod);

  _settings.getData(Color, s);
  QColor color(s);

  PlotFactory fac;
  _settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  PlotLine *line = getVIDYA(in, period, volPeriod, lineType, color);
  if (! line)
  {
    delete in;
    return 1;
  }

  _settings.getData(Label, s);
  line->setLabel(s);
  
  s = "1";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  delete in;

  return 0;
}

int VIDYA::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,VIDYA,<NAME>,<INPUT>,<PERIOD>,<VOLUME_PERIOD>,<PLOT TYPE>,<COLOR>
  //     0       1      2     3       4       5            6             7         8

  if (set.count() != 9)
  {
    qDebug() << _indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  PlotLine *inSignal = ind.line(set[4]);
  if (! inSignal)
  {
    inSignal = data.getInput(data.getInputType(set[4]));
    if (! inSignal)
    {
      qDebug() << _indicator << "::getCUS: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], inSignal);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid fast period settings" << set[5];
    return 1;
  }

  int volPeriod = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid fast period settings" << set[6];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[7]);
  if (lineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid plot type" << set[7];
    return 1;
  }

  QColor color(set[8]);
  if (! color.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid color" << set[8];
    return 1;
  }

  PlotLine *line = getVIDYA(inSignal, period, volPeriod, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

PlotLine * VIDYA::calcCMO (PlotLine *in, int period)
{
  if (in->count() < period)
    return 0;

  TA_Real input[in->count()];
  TA_Real out[in->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar bar;
    in->data(keys.at(loop), bar);
    input[loop] = (TA_Real) bar.data();
  }

  TA_RetCode rc = TA_CMO(0,
                         keys.count() - 1,
                         &input[0],
                         period,
                         &outBeg,
                         &outNb,
                         &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getCMO: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    PlotLineBar bar(out[outLoop]);
    line->setData(keys.at(keyLoop), bar);
    keyLoop--;
    outLoop--;
  }

  return line;
}

PlotLine * VIDYA::getVIDYA (PlotLine *inSignal, int period, int volPeriod, int lineType, QColor &color)
{
  if (inSignal->count() < period || inSignal->count() < volPeriod)
    return 0;

  PlotLine *cmo = calcCMO(inSignal, volPeriod);
  if (! cmo)
    return 0;

  PlotFactory fac;
  PlotLine *out = fac.plot(lineType);
  if (! out)
  {
    delete cmo;
    return 0;
  }

  int size = inSignal->count();
  QVector<double> *inSeries = new QVector<double>(size);
  inSeries->fill(0.0);
  QVector<double> *offset = new QVector<double>(size);
  offset->fill(0.0);
  QVector<double> *absCmo = new QVector<double>(size);
  absCmo->fill(0.0);
  QVector<double> *vidya = new QVector<double>(size);
  vidya->fill(0.0);

  double c = 2 / (double) period + 1;

  QList<int> keys;
  inSignal->keys(keys);
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar bar;
    inSignal->data(keys.at(loop), bar);
    (*inSeries)[loop] = bar.data();
  }

  keys.clear();
  cmo->keys(keys);

  int index = inSeries->size() -1;
  loop = keys.count() - 1;
  for (; loop > -1; loop--)
  {
    PlotLineBar bar;
    cmo->data(keys.at(loop), bar);
    (*absCmo)[index] = fabs(bar.data() / 100);
    index--;
  }

  loop = volPeriod + period;
  for (; loop < (int) inSeries->size(); loop++)		// period safty
  {
    (*vidya)[loop] = (inSeries->at(loop) * c * absCmo->at(loop)) + ((1 - absCmo->at(loop) * c) * vidya->at(loop - 1));
    //!  (Price*Const*AbsCMO) + ((1-AbsCMO*Const)*VIDYA[1]),Price);
    
    PlotLineBar bar(color, vidya->at(loop));
    out->setData(loop, bar);
  }

  delete inSeries;
  delete offset;
  delete absCmo;
  delete vidya;
  delete cmo;

  return out;
}

int VIDYA::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  _settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(Period, page, QObject::tr("Period"), _settings.getInt(Period), 1, 100000);

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  _settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addIntItem(VPeriod, page, QObject::tr("Volume Period"), _settings.getInt(VPeriod), 1, 100000);

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
  VIDYA *o = new VIDYA;
  return ((IndicatorPlugin *) o);
}

