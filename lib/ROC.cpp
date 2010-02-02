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

#include "ROC.h"

#include <QtDebug>


ROC::ROC ()
{
  indicator = "ROC";
  methodKey = QObject::tr("Method");
  smoothKey = QObject::tr("Smoothing Period");
  smoothTypeKey = QObject::tr("Smoothing Type");

  methodList << QObject::tr("ROC");
  methodList << QObject::tr("ROCP");
  methodList << QObject::tr("ROCR");
  methodList << QObject::tr("ROC100");

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "Histogram Bar";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);

  d = "Close";
  settings.setData(inputKey, d);

  settings.setData(periodKey, 10);
  settings.setData(smoothKey, 1);

  d = "SMA";
  settings.setData(smoothTypeKey, d);

  d = "ROC";
  settings.setData(methodKey, d);
}

int ROC::getIndicator (Indicator &ind, BarData *data)
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

  int smoothing = settings.getInt(smoothKey);

  settings.getData(methodKey, s);
  int method = methodList.indexOf(s);

  settings.getData(smoothTypeKey, s);
  int type = maList.indexOf(s);

  PlotLine *line = getROC(in, period, method, smoothing, type);
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

int ROC::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,ROC,<NAME>,<INPUT>,<PERIOD>,<METHOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>

  if (set.count() != 8)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[2];
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
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period parm" << set[4];
    return 1;
  }

  int method = methodList.indexOf(set[5]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[5];
    return 1;
  }

  int smoothing = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid smoothing" << set[6];
    return 1;
  }

  int type = maList.indexOf(set[7]);
  if (type == -1)
  {
    qDebug() << indicator << "::calculate: invalid smoothing type" << set[7];
    return 1;
  }

  PlotLine *line = getROC(in, period, method, smoothing, type);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * ROC::getROC (PlotLine *in, int period, int method, int smoothing, int type)
{
  int size = in->getSize();
  TA_Real input[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_ROC(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 1:
      rc = TA_ROCP(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 2:
      rc = TA_ROCR(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 3:
      rc = TA_ROCR100(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(line, smoothing, type);
    if (! ma)
    {
      delete line;
      return 0;
    }

    delete line;
    line = ma;
  }

  return line;
}

int ROC::dialog ()
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

  settings.getData(methodKey, d);
  dialog->addComboItem(page, methodKey, methodList, d);

  dialog->addIntItem(page, smoothKey, settings.getInt(smoothKey), 1, 100000);

  settings.getData(smoothTypeKey, d);
  dialog->addComboItem(page, smoothTypeKey, maList, d);

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

  dialog->getItem(methodKey, d);
  settings.setData(methodKey, d);

  dialog->getItem(smoothKey, d);
  settings.setData(smoothKey, d);

  dialog->getItem(smoothTypeKey, d);
  settings.setData(smoothTypeKey, d);

  delete dialog;
  return rc;
}

