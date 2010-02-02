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

#include "MAMA.h"
#include "BARS.h"

#include <QtDebug>


MAMA::MAMA ()
{
  indicator = "MAMA";
  oscKey = QObject::tr("Oscillator");
  mcKey = QObject::tr("MAMA Color");
  fcKey = QObject::tr("FAMA Color");
  mpKey = QObject::tr("MAMA Plot");
  fpKey = QObject::tr("FAMA Plot");
  mlKey = QObject::tr("MAMA Label");
  flKey = QObject::tr("FAMA Label");
  flmKey = QObject::tr("Fast Limit");
  slmKey = QObject::tr("Slow Limit");

  QString d;
  d = "red";
  settings.setData(mcKey, d);

  d = "yellow";
  settings.setData(fcKey, d);

  d = "Line";
  settings.setData(mpKey, d);

  settings.setData(fpKey, d);

  d = "MAMA";
  settings.setData(mlKey, d);

  d = "FAMA";
  settings.setData(flKey, d);

  d = "Close";
  settings.setData(inputKey, d);

  settings.setData(flmKey, 0.5);
  settings.setData(slmKey, 0.05);

  settings.setData(oscKey, 0);
}

int MAMA::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(inputKey, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  double fast = settings.getDouble(flmKey);
  double slow = settings.getDouble(slmKey);

  QList<PlotLine *> l;
  int rc = getMAMA(in, fast, slow, l);
  if (rc)
  {
    qDeleteAll(l);
    delete in;
    return 1;
  }

  // mama line
  PlotLine *mama = l.at(0);
  PlotLine *fama = l.at(1);

  int osc = settings.getInt(oscKey);
  if (osc)
  {
    PlotLine *line = new PlotLine;
    line->setType(PlotLine::HistogramBar);

    settings.getData(mcKey, s);
    line->setColor(s);

    s = "OSC";
    line->setLabel(s);

    int loop;
    for (loop = 0; loop < mama->getSize(); loop++)
      line->append(mama->getData(loop) - fama->getData(loop));

    ind.addLine(line);
  }
  else
  {
    BARS bars;
    bars.getIndicator(ind, data);

    settings.getData(mcKey, s);
    mama->setColor(s);

    settings.getData(mpKey, s);
    mama->setType(s);

    settings.getData(mlKey, s);
    mama->setLabel(s);

    ind.addLine(mama);

    // fama line
    settings.getData(fcKey, s);
    fama->setColor(s);

    settings.getData(fpKey, s);
    fama->setType(s);

    settings.getData(flKey, s);
    fama->setLabel(s);

    ind.addLine(fama);
  }

  delete in;

  return 0;
}

int MAMA::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MAMA,<INPUT>,<NAME_MAMA>,<NAME_FAMA>,<FAST_LIMIT>,<SLOW_LIMIT>

  if (set.count() != 7)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[3];
    return 1;
  }

  tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[4];
    return 1;
  }

  PlotLine *in = tlines.value(set[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[2]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[2];
      return 1;
    }

    tlines.insert(set[2], in);
  }

  bool ok;
  double fast = set[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fast limit" << set[5];
    return 1;
  }

  double slow = set[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid slow limit" << set[6];
    return 1;
  }

  QList<PlotLine *> l;
  int rc = getMAMA(in, fast, slow, l);
  if (rc)
  {
    qDeleteAll(l);
    return 1;
  }

  tlines.insert(set[3], l.at(0));
  tlines.insert(set[4], l.at(1));

  return 0;
}

int MAMA::getMAMA (PlotLine *in, double fast, double slow, QList<PlotLine *> &l)
{
  int size = in->getSize();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_MAMA(0, size - 1, &input[0], fast, slow, &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *fama = new PlotLine;
  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    line->append(out[loop]);
    fama->append(out2[loop]);
  }

  l.append(line);
  l.append(fama);

  return 0;
}

int MAMA::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(inputKey, d);
  dialog->addComboItem(page, inputKey, inputList, d);

  dialog->addDoubleItem(page, flmKey, settings.getDouble(flmKey), 0.01, 0.99);

  dialog->addDoubleItem(page, slmKey, settings.getDouble(slmKey), 0.01, 0.99);

  dialog->addCheckItem(page, oscKey, settings.getInt(oscKey));

  page++;
  k = QObject::tr("MAMA");
  dialog->addPage(page, k);

  settings.getData(mcKey, d);
  dialog->addColorItem(page, mcKey, d);

  settings.getData(mpKey, d);
  dialog->addComboItem(page, mpKey, plotList, d);

  settings.getData(mlKey, d);
  dialog->addTextItem(page, mlKey, d);

  page++;
  k = QObject::tr("FAMA");
  dialog->addPage(page, k);

  settings.getData(fcKey, d);
  dialog->addColorItem(page, fcKey, d);

  settings.getData(fpKey, d);
  dialog->addComboItem(page, fpKey, plotList, d);

  settings.getData(flKey, d);
  dialog->addTextItem(page, flKey, d);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  dialog->getItem(mcKey, d);
  settings.setData(mcKey, d);

  dialog->getItem(mpKey, d);
  settings.setData(mpKey, d);

  dialog->getItem(mlKey, d);
  settings.setData(mlKey, d);

  dialog->getItem(fcKey, d);
  settings.setData(fcKey, d);

  dialog->getItem(fpKey, d);
  settings.setData(fpKey, d);

  dialog->getItem(flKey, d);
  settings.setData(flKey, d);

  dialog->getItem(inputKey, d);
  settings.setData(inputKey, d);

  dialog->getItem(flmKey, d);
  settings.setData(flmKey, d);

  dialog->getItem(slmKey, d);
  settings.setData(slmKey, d);

  dialog->getItem(oscKey, d);
  settings.setData(oscKey, d);

  delete dialog;
  return rc;
}

