/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include <math.h>
#include <QVector>
#include <QtDebug>

#define PI 3.14159265


VIDYA::VIDYA ()
{
  indicator = "VIDYA";
  vpKey = QObject::tr("Volume Period");

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "Line";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);

  d = "Close";
  settings.setData(inputKey, d);

  settings.setData(periodKey, 14);

  settings.setData(vpKey, 10);
}

int VIDYA::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(inputKey, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(periodKey);
  int volPeriod = settings.getInt(vpKey);

  PlotLine *line = getVIDYA(in, period, volPeriod);
  if (! line)
  {
    delete in;
    return 1;
  }

  settings.getData(colorKey, s);
  line->setColor(s);

  settings.getData(plotKey, s);
  line->setType(s);

  settings.getData(labelKey, s);
  line->setLabel(s);

  ind.addLine(line);

  delete in;

  return 0;
}

int VIDYA::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,VIDYA,<NAME>,<INPUT>,<PERIOD>,<VOLUME_PERIOD>

  if (set.count() != 6)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[2];
    return 1;
  }

  PlotLine *inSignal = tlines.value(set[3]);
  if (! inSignal)
  {
    inSignal = data->getInput(data->getInputType(set[3]));
    if (! inSignal)
    {
      qDebug() << indicator << "::calculate: input not found" << set[3];
      return 1;
    }

    tlines.insert(set[3], inSignal);
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fast period settings" << set[4];
    return 1;
  }

  int volPeriod = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fast period settings" << set[5];
    return 1;
  }

  PlotLine *line = getVIDYA(inSignal, period, volPeriod);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

void VIDYA::calcCMO (PlotLine *outSignal, PlotLine *inSignal, int iPeriod)
{
  //!  Chande Momentum Oscillator
  //!  Raw VIDYA

  int loop = (int)inSignal->getSize();

  QVector<double> *inSeries = new QVector<double>(loop);
  inSeries->fill(0.0);
  QVector<double> *offset = new QVector<double>(loop);
  offset->fill(0.0);
  QVector<double> *mom = new QVector<double>(loop);
  mom->fill(0.0);
  QVector<double> *posSeries = new QVector<double>(loop);
  posSeries->fill(0.0);
  QVector<double> *negSeries = new QVector<double>(loop);
  negSeries->fill(0.0);
  QVector<double> *sumPos = new QVector<double>(loop);
  sumPos->fill(0.0);
  QVector<double> *sumNeg = new QVector<double>(loop);
  sumNeg->fill(0.0);
  QVector<double> *cmoUp = new QVector<double>(loop);
  cmoUp->fill(0.0);
  QVector<double> *cmoDown = new QVector<double>(loop);
  cmoDown->fill(0.0);
  QVector<double> *rawCmo = new QVector<double>(loop);
  rawCmo->fill(0.0);

  int i = 0;

  for (i = 0; i < loop; i++)
    (*inSeries)[i] = inSignal->getData(i);

  for (i = iPeriod - 1; i < loop; i++)
  {
    (*offset)[i] = inSeries->at(i-1);

    (*mom)[i] = inSeries->at(i) - offset->at(i);

    if (mom->at(i) > 0)
      (*posSeries)[i] = mom->at(i);
    else
      (*posSeries)[i] = 0;

    if (mom->at(i) < 0)
      (*negSeries)[i] = fabs(mom->at(i));
    else
      (*negSeries)[i] = 0;

    int j = 0;
    double sumUp = 0;
    double sumDown = 0;

    for (j = 0; j < iPeriod; j++)
    {
      sumUp += posSeries->at(i-j);
      sumDown += negSeries->at(i-j);
    }

    (*sumPos)[i] = sumUp;
    (*sumNeg)[i] = sumDown;

    (*cmoUp)[i] = 100 * ((sumPos->at(i) - sumNeg->at(i)));

    (*cmoDown)[i] = sumPos->at(i) + sumNeg->at(i);

    (*rawCmo)[i] = cmoUp->at(i) / cmoDown->at(i);

    if (i > iPeriod -1)
      outSignal->append(rawCmo->at(i));
  }

  delete inSeries;
  delete offset;
  delete mom;
  delete posSeries;
  delete negSeries;
  delete sumPos;
  delete sumNeg;
  delete cmoUp;
  delete cmoDown;
  delete rawCmo;
}

PlotLine * VIDYA::getVIDYA (PlotLine *inSignal, int period, int volPeriod)
{
  PlotLine *out = new PlotLine;
  PlotLine *cmo = new PlotLine;

  calcCMO(cmo, inSignal, volPeriod);

  int i = 0;
  int loop = (int)inSignal->getSize();

  QVector<double> *inSeries = new QVector<double>(loop);
  inSeries->fill(0.0);
  QVector<double> *offset = new QVector<double>(loop);
  offset->fill(0.0);
  QVector<double> *absCmo = new QVector<double>(loop);
  absCmo->fill(0.0);
  QVector<double> *vidya = new QVector<double>(loop);
  vidya->fill(0.0);

  double c = 2 / (double) period + 1;

  for ( i = 0; i < loop; i++)
    (*inSeries)[i] = inSignal->getData(i);

  int index = inSeries->size() -1;
  for (i = cmo->getSize() -1; i >= 0; i--)
  {
    (*absCmo)[index] = fabs(cmo->getData(i) / 100);
    index--;
  }

  for (i = volPeriod + period; i < (int)inSeries->size(); i++)		// period safty
  {
    (*vidya)[i] = ( inSeries->at(i) * c * absCmo->at(i) ) + ( (1 - absCmo->at(i) * c) * vidya->at(i-1) );
    //!  (Price*Const*AbsCMO) + ((1-AbsCMO*Const)*VIDYA[1]),Price);
    out->append(vidya->at(i));
  }

  delete inSeries;
  delete offset;
  delete absCmo;
  delete vidya;
  delete cmo;

  return out;
}

int VIDYA::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(colorKey, d);
  dialog->addColorItem(page, colorKey, d);

  settings.getData(plotKey, d);
  dialog->addComboItem(page, plotKey, plotList, d);

  settings.getData(labelKey, d);
  dialog->addTextItem(page, labelKey, d);

  settings.getData(inputKey, d);
  dialog->addComboItem(page, inputKey, inputList, d);

  dialog->addIntItem(page, periodKey, settings.getInt(periodKey), 1, 100000);

  dialog->addIntItem(page, vpKey, settings.getInt(vpKey), 1, 100000);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  dialog->getItem(colorKey, d);
  settings.setData(colorKey, d);

  dialog->getItem(plotKey, d);
  settings.setData(plotKey, d);

  dialog->getItem(labelKey, d);
  settings.setData(labelKey, d);

  dialog->getItem(inputKey, d);
  settings.setData(inputKey, d);

  dialog->getItem(periodKey, d);
  settings.setData(periodKey, d);

  dialog->getItem(vpKey, d);
  settings.setData(vpKey, d);

  delete dialog;
  return rc;
}

