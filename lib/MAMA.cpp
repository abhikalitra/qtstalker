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

  settings.setData(OSC, 1);
  settings.setData(OSCColor, "red");
  settings.setData(MAMAColor, "red");
  settings.setData(FAMAColor, "yellow");
  settings.setData(MAMAPlot, "Line");
  settings.setData(FAMAPlot, "Line");
  settings.setData(MAMALabel, "MAMA");
  settings.setData(FAMALabel, "FAMA");
  settings.setData(OSCLabel, "MAMAOSC");
  settings.setData(FastLimit, 0.5);
  settings.setData(SlowLimit, 0.05);
  settings.setData(Input, "Close");
}

int MAMA::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  double fast = settings.getDouble(FastLimit);
  double slow = settings.getDouble(SlowLimit);

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

  int osc = settings.getInt(OSC);
  if (osc)
  {
    PlotLine *line = new PlotLine;
    line->setType(PlotLine::HistogramBar);

    settings.getData(OSCColor, s);
    line->setColor(s);

    settings.getData(OSCLabel, s);
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

    settings.getData(MAMAColor, s);
    mama->setColor(s);

    settings.getData(MAMAPlot, s);
    mama->setType(s);

    settings.getData(MAMALabel, s);
    mama->setLabel(s);

    ind.addLine(mama);

    // fama line
    settings.getData(FAMAColor, s);
    fama->setColor(s);

    settings.getData(FAMAPlot, s);
    fama->setType(s);

    settings.getData(FAMALabel, s);
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

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addDoubleItem(FastLimit, page, QObject::tr("Fast Limit"), settings.getDouble(FastLimit), 0.01, 0.99);

  dialog->addDoubleItem(SlowLimit, page, QObject::tr("Slow Limit"), settings.getDouble(SlowLimit), 0.01, 0.99);

  dialog->addCheckItem(OSC, page, QObject::tr("Oscillator"), settings.getInt(OSC));

  page++;
  k = QObject::tr("MAMA");
  dialog->addPage(page, k);

  settings.getData(MAMAColor, d);
  dialog->addColorItem(MAMAColor, page, QObject::tr("Color"), d);

  settings.getData(MAMAPlot, d);
  dialog->addComboItem(MAMAPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MAMALabel, d);
  dialog->addTextItem(MAMALabel, page, QObject::tr("Label"), d);

  page++;
  k = QObject::tr("FAMA");
  dialog->addPage(page, k);

  settings.getData(FAMAColor, d);
  dialog->addColorItem(FAMAColor, page, QObject::tr("Color"), d);

  settings.getData(FAMAPlot, d);
  dialog->addComboItem(FAMAPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(FAMALabel, d);
  dialog->addTextItem(FAMALabel, page, QObject::tr("Label"), d);

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

