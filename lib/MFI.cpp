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

#include "MFI.h"

#include <QtDebug>


MFI::MFI ()
{
  indicator = "MFI";
  smoothKey = QObject::tr("Smoothing Period");
  smoothTypeKey = QObject::tr("Smoothing Type");

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "Line";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);

  settings.setData(periodKey, 14);
  settings.setData(smoothKey, 10);

  d = "SMA";
  settings.setData(smoothTypeKey, d);
}

int MFI::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  int period = settings.getInt(periodKey);

  int smoothing = settings.getInt(smoothKey);

  settings.getData(smoothTypeKey, s);
  int type = maList.indexOf(s);

  PlotLine *line = getMFI(data, period, smoothing, type);
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

int MFI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MFI,<NAME>,<PERIOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>

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

  bool ok;
  int period = set[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period settings" << set[3];
    return 1;
  }

  int smoothing = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid smoothing period" << set[4];
    return 1;
  }

  int ma = maList.indexOf(set[5]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid smoothing type" << set[5];
    return 1;
  }

  PlotLine *line = getMFI(data, period, smoothing, ma);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * MFI::getMFI (BarData *data, int period, int smoothing, int type)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real volume[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
    volume[loop] = (TA_Real) data->getVolume(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_MFI(0, size - 1, &high[0], &low[0], &close[0], &volume[0], period, &outBeg, &outNb, &out[0]);
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

int MFI::dialog ()
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

  dialog->addIntItem(page, smoothKey, settings.getInt(smoothKey), 1, 100000);

  settings.getData(smoothTypeKey, d);
  dialog->addComboItem(page, smoothTypeKey, maList, d);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  QStringList keys;
  settings.getKeyList(keys);
  int loop;
  for (loop = 0; loop < keys.count(); loop++)
  {
    QString d;
    dialog->getItem(keys[loop], d);
    if (! d.isEmpty())
      settings.setData(keys[loop], d);
  }

  delete dialog;
  return rc;
}


