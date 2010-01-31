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

#include "MINUS_DM.h"

#include <QtDebug>


MINUS_DM::MINUS_DM ()
{
  indicator = "MINUS_DM";

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "Line";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);

  settings.setData(periodKey, 14);
}

int MINUS_DM::getIndicator (Indicator &ind, BarData *data)
{
  int period = settings.getInt(periodKey);

  PlotLine *line = getMINUS_DM(data, period);
  if (! line)
    return 1;

  QString s;
  settings.getData(colorKey, s);
  line->setColor(s);

  settings.getData(plotKey, s);
  line->setType(s);

  settings.getData(labelKey, s);
  line->setLabel(s);

  ind.addLine(line);

  return 0;
}

int MINUS_DM::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MINUS_DM,<NAME>,<PERIOD>

  if (set.count() != 4)
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

  bool ok;
  int period = set[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period settings" << set[3];
    return 1;
  }

  PlotLine *line = getMINUS_DM(data, period);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * MINUS_DM::getMINUS_DM (BarData *data, int period)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_MINUS_DM(0, size - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0]);
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

int MINUS_DM::dialog ()
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

  dialog->addIntItem(page, periodKey, settings.getInt(periodKey), 1, 100000);

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

  dialog->getItem(periodKey, d);
  settings.setData(periodKey, d);

  delete dialog;
  return rc;
}


