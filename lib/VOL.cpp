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

#include "VOL.h"

#include <QtDebug>


VOL::VOL ()
{
  indicator = "VOL";
  methodKey = QObject::tr("Method");

  methodList << "OBV";
  methodList << "VOL";

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "Histogram Bar";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);

  d = "VOL";
  settings.setData(methodKey, d);
}

int VOL::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(methodKey, s);
  int method = methodList.indexOf(s);

  PlotLine *line = getVOL(data, method);
  if (! line)
    return 1;

  settings.getData(colorKey, s);
  line->setColor(s);

  settings.getData(plotKey, s);
  line->setType(s);

  settings.getData(labelKey, s);
  line->setLabel(s);

  ind.addLine(line);

  return 0;
}

int VOL::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,VOL,<NAME>,<METHOD>

  if (set.count() != 4)
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

  int method = methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[3];
    return 1;
  }

  PlotLine *line = getVOL(data, method);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * VOL::getVOL (BarData *data, int method)
{
  int size = data->count();
  TA_Real close[size];
  TA_Real volume[size];
  TA_Real out[size];

  int loop;
  for (loop = 0; loop < size; loop++)
  {
    close[loop] = (TA_Real) data->getClose(loop);
    volume[loop] = (TA_Real) data->getVolume(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_OBV(0, size - 1, &close[0], &volume[0], &outBeg, &outNb, &out[0]);
      break;
    case 1: // volume;
    {
      int loop;
      outNb = size;
      for (loop = 0; loop < size; loop++)
        out[loop] = (TA_Real) data->getVolume(loop);
      break;
    }
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: : TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  return line;
}

int VOL::dialog ()
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

  settings.getData(methodKey, d);
  dialog->addComboItem(page, methodKey, methodList, d);

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

  dialog->getItem(methodKey, d);
  settings.setData(methodKey, d);

  delete dialog;
  return rc;
}


