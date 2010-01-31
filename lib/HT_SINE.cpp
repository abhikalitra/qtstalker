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

#include "HT_SINE.h"

#include <QtDebug>


HT_SINE::HT_SINE ()
{
  indicator = "HT_SINE";
  pcKey = QObject::tr("Sine Color");
  qcKey = QObject::tr("Lead Color");
  ppKey = QObject::tr("Sine Plot");
  qpKey = QObject::tr("Lead Plot");
  plKey = QObject::tr("Sine Label");
  qlKey = QObject::tr("Lead Label");

  QString d;
  d = "red";
  settings.setData(pcKey, d);

  d = "red";
  settings.setData(qcKey, d);

  d = "Line";
  settings.setData(ppKey, d);
  settings.setData(qpKey, d);

  d = "SINE";
  settings.setData(plKey, d);

  d = "LEAD";
  settings.setData(qlKey, d);

  d = "Close";
  settings.setData(inputKey, d);
}

int HT_SINE::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(inputKey, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  QList<PlotLine *> l;
  int rc = getHT_SINE(in, l);
  if (rc || l.count() != 2)
  {
    delete in;
    qDeleteAll(l);
    return 1;
  }

  // phase line
  PlotLine *line = l.at(0);
  settings.getData(pcKey, s);
  line->setColor(s);

  settings.getData(ppKey, s);
  line->setType(s);

  settings.getData(plKey, s);
  line->setLabel(s);

  ind.addLine(line);

  // quad line
  line = l.at(1);
  settings.getData(qcKey, s);
  line->setColor(s);

  settings.getData(qpKey, s);
  line->setType(s);

  settings.getData(qlKey, s);
  line->setLabel(s);

  ind.addLine(line);

  delete in;

  return 0;
}

int HT_SINE::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,HT_SINE,<INPUT>,<NAME_SINE>,<NAME_LEAD>

  if (set.count() != 5)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
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

  QList<PlotLine *> l;
  int rc = getHT_SINE(in, l);
  if (rc || l.count() != 2)
  {
    qDeleteAll(l);
    return 1;
  }

  tlines.insert(set[3], l.at(0));
  tlines.insert(set[4], l.at(1));

  return 0;
}

int HT_SINE::getHT_SINE (PlotLine *in, QList<PlotLine *> &l)
{
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  TA_Real out2[in->getSize()];
  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_HT_SINE(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *line2 = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    line->append(out[loop]);
    line2->append(out2[loop]);
  }

  l.append(line);
  l.append(line2);

  return 0;
}

int HT_SINE::dialog ()
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

  delete dialog;
  return rc;
}


