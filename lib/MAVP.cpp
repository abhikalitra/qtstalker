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

#include "MAVP.h"
#include "BARS.h"

#include <QtDebug>


MAVP::MAVP ()
{
  indicator = "MAVP";
  input2Key = QObject::tr("Input 2");
  minKey = QObject::tr("Min");
  maxKey = QObject::tr("Max");

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "Line";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);

  d = "Close";
  settings.setData(inputKey, d);
  settings.setData(input2Key, d);

  settings.setData(minKey, 2);

  settings.setData(maxKey, 30);

  d = "SMA";
  settings.setData(maTypeKey, d);
}

int MAVP::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(inputKey, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  settings.getData(input2Key, s);
  PlotLine *in2 = data->getInput(data->getInputType(s));
  if (! in2)
  {
    delete in;
    qDebug() << indicator << "::calculate: input 2 not found" << s;
    return 1;
  }

  int min = settings.getInt(minKey);
  int max = settings.getInt(maxKey);

  settings.getData(maTypeKey, s);
  int ma = maList.indexOf(s);

  PlotLine *line = getMAVP(in, in2, min, max, ma);
  if (! line)
  {
    delete in;
    delete in2;
    return 1;
  }

  BARS bars;
  bars.getIndicator(ind, data);

  settings.getData(colorKey, s);
  line->setColor(s);

  settings.getData(plotKey, s);
  line->setType(s);

  settings.getData(labelKey, s);
  line->setLabel(s);

  ind.addLine(line);

  delete in;
  delete in2;

  return 0;
}

int MAVP::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MAVP,<NAME>,<INPUT_1>,<INPUT_2>,<MIN_PERIOD>,<MAX_PERIOD>,<MA_TYPE>

  if (set.count() != 8)
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

  PlotLine *in2 = tlines.value(set[4]);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(set[4]));
    if (! in2)
    {
      qDebug() << indicator << "::calculate: input not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in2);
  }

  bool ok;
  int min = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid min period settings" << set[5];
    return 1;
  }

  int max = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid max period settings" << set[6];
    return 1;
  }

  int ma = maList.indexOf(set[7]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid ma settings" << set[7];
    return 1;
  }

  PlotLine *line = getMAVP(in, in2, min, max, ma);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * MAVP::getMAVP (PlotLine *in, PlotLine *in2, int min, int max, int ma)
{
  int size = in->getSize();
  if (in2->getSize() < size)
    size = in2->getSize();

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = in->getSize() - 1;
  int loop2 = in2->getSize() - 1;
  int count = size - 1;
  for (; count > -1; loop--, loop2--, count--)
  {
    input[loop] = (TA_Real) in->getData(loop);
    input2[loop2] = (TA_Real) in2->getData(loop2);
  }

  TA_RetCode rc = TA_MAVP(0, size - 1, &input[0], &input2[0], min, max, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  return line;
}

int MAVP::dialog ()
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

  settings.getData(input2Key, d);
  dialog->addComboItem(page, input2Key, inputList, d);

  dialog->addIntItem(page, minKey, settings.getInt(minKey), 2, 100000);

  dialog->addIntItem(page, maxKey, settings.getInt(maxKey), 2, 100000);

  settings.getData(maTypeKey, d);
  dialog->addComboItem(page, maTypeKey, maList, d);

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

  dialog->getItem(input2Key, d);
  settings.setData(input2Key, d);

  dialog->getItem(minKey, d);
  settings.setData(minKey, d);

  dialog->getItem(maxKey, d);
  settings.setData(maxKey, d);

  dialog->getItem(maTypeKey, d);
  settings.setData(maTypeKey, d);

  delete dialog;
  return rc;
}

