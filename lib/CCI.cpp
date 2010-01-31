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

#include "CCI.h"

#include <QtDebug>


CCI::CCI ()
{
  indicator = "CCI";
  ref1Key = QObject::tr("Ref 1 Value");
  ref2Key = QObject::tr("Ref 2 Value");
  ref1ColorKey = QObject::tr("Ref 1 Color");
  ref2ColorKey = QObject::tr("Ref 2 Color");

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "Line";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);
  settings.setData(periodKey, 14);
  settings.setData(ref1Key, 100);
  settings.setData(ref2Key, -100);

  d = "red";
  settings.setData(ref1ColorKey, d);
  settings.setData(ref2ColorKey, d);
}

int CCI::getIndicator (Indicator &ind, BarData *data)
{
  int period = settings.getInt(periodKey);

  PlotLine *line = getCCI(data, period);
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

  // create the ref1 line
  int ref = settings.getInt(ref1Key);
  line = new PlotLine;
  settings.getData(ref1ColorKey, s);
  line->setColor(s);
  line->setType(PlotLine::Horizontal);
  line->append(ref);
  ind.addLine(line);

  // create the ref2 line
  ref = settings.getInt(ref2Key);
  line = new PlotLine;
  settings.getData(ref2ColorKey, s);
  line->setColor(s);
  line->setType(PlotLine::Horizontal);
  line->append(ref);
  ind.addLine(line);

  return 0;
}

int CCI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,CCI,<NAME>,<PERIOD>

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

  bool ok;
  int period = set[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period parm" << set[3];
    return 1;
  }

  PlotLine *line = getCCI(data, period);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * CCI::getCCI (BarData *data, int period)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_CCI(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
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

int CCI::dialog ()
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

  dialog->addIntItem(page, periodKey, settings.getInt(periodKey), 2, 100000);

  page++;
  k = QObject::tr("Ref Lines");
  dialog->addPage(page, k);

  settings.getData(ref1ColorKey, d);
  dialog->addColorItem(page, ref1ColorKey, d);

  settings.getData(ref2ColorKey, d);
  dialog->addColorItem(page, ref2ColorKey, d);

  dialog->addIntItem(page, ref1Key, settings.getInt(ref1Key), -100000, 100000);

  dialog->addIntItem(page, ref2Key, settings.getInt(ref2Key), -100000, 100000);

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

  dialog->getItem(ref1Key, d);
  settings.setData(ref1Key, d);

  dialog->getItem(ref2Key, d);
  settings.setData(ref2Key, d);

  dialog->getItem(ref1ColorKey, d);
  settings.setData(ref1ColorKey, d);

  dialog->getItem(ref2ColorKey, d);
  settings.setData(ref2ColorKey, d);

  delete dialog;
  return rc;
}
