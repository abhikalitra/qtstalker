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

#include "APO.h"

#include <QtDebug>


APO::APO ()
{
  indicator = "APO";
  fpKey = QObject::tr("Fast Period");
  spKey = QObject::tr("Slow Period");
  maKey = QObject::tr("MA Type");

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "Histogram Bar";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);

  d = "Close";
  settings.setData(inputKey, d);

  settings.setData(fpKey, 12);

  settings.setData(spKey, 26);

  d = "SMA";
  settings.setData(maKey, d);
}

int APO::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(inputKey, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int fast = settings.getInt(fpKey);

  int slow = settings.getInt(spKey);

  settings.getData(maKey, s);
  int ma = maList.indexOf(s);

  PlotLine *line = getAPO(in, fast, slow, ma);
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

int APO::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,APO,<NAME>,<INPUT>,<FAST_PERIOD>,<SLOW_PERIOD>,<MA_TYPE>

  if (set.count() != 7)
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
  int fast = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fast period parm" << set[4];
    return 1;
  }

  int slow = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid slow period parm" << set[5];
    return 1;
  }

  int ma = maList.indexOf(set[6]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid ma parm" << set[6];
    return 1;
  }

  PlotLine *line = getAPO(in, fast, slow, ma);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * APO::getAPO (PlotLine *in, int fast, int slow, int ma)
{
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_APO(0, in->getSize() - 1, &input[0], fast, slow, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
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

int APO::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(colorKey, d);
  QColor c(d);
  dialog->addColorItem(page, colorKey, c);

  settings.getData(plotKey, d);
  dialog->addComboItem(page, plotKey, plotList, d);

  settings.getData(labelKey, d);
  dialog->addTextItem(page, labelKey, d);

  settings.getData(inputKey, d);
  dialog->addComboItem(page, inputKey, inputList, d);

  dialog->addIntItem(page, fpKey, settings.getInt(fpKey), 2, 100000);

  dialog->addIntItem(page, spKey, settings.getInt(spKey), 2, 100000);

  settings.getData(maKey, d);
  dialog->addComboItem(page, maKey, maList, d);

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

  dialog->getItem(fpKey, d);
  settings.setData(fpKey, d);

  dialog->getItem(spKey, d);
  settings.setData(spKey, d);

  dialog->getItem(maKey, d);
  settings.setData(maKey, d);

  delete dialog;
  return rc;
}



