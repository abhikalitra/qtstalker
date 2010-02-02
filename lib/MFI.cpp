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
  showMAKey = QObject::tr("Show MA");
  showMFIKey = QObject::tr("Show MFI");

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "yellow";
  settings.setData(maColorKey, d);

  d = "Line";
  settings.setData(plotKey, d);
  settings.setData(maPlotKey, d);

  settings.setData(labelKey, indicator);

  d = "MFI_MA";
  settings.setData(maLabelKey, d);

  settings.setData(periodKey, 14);
  settings.setData(maPeriodKey, 10);

  d = "SMA";
  settings.setData(maTypeKey, d);

  settings.setData(showMFIKey, 1);
  settings.setData(showMAKey, 1);
}

int MFI::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  int period = settings.getInt(periodKey);

  PlotLine *ma = 0;
  PlotLine *line = getMFI(data, period);
  if (! line)
    return 1;

  if (settings.getInt(showMAKey))
  {
    int maPeriod = settings.getInt(maPeriodKey);

    settings.getData(maTypeKey, s);
    int type = maList.indexOf(s);

    ma = getMA(line, maPeriod, type);
    if (! ma)
    {
      delete line;
      return 1;
    }

    settings.getData(maColorKey, s);
    ma->setColor(s);

    settings.getData(maPlotKey, s);
    ma->setType(s);

    settings.getData(maLabelKey, s);
    ma->setLabel(s);
  }

  settings.getData(colorKey, s);
  line->setColor(s);

  settings.getData(plotKey, s);
  line->setType(s);

  settings.getData(labelKey, s);
  line->setLabel(s);

  if (settings.getInt(showMFIKey))
    ind.addLine(line);
  else
    delete line;

  if (settings.getInt(showMAKey))
    ind.addLine(ma);

  return 0;
}

int MFI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MFI,<NAME>,<PERIOD>

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

  PlotLine *line = getMFI(data, period);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * MFI::getMFI (BarData *data, int period)
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

  dialog->addCheckItem(page, showMFIKey, settings.getInt(showMFIKey));

  page++;
  k = QObject::tr("MA");
  dialog->addPage(page, k);

  settings.getData(maColorKey, d);
  dialog->addColorItem(page, maColorKey, d);

  settings.getData(maPlotKey, d);
  dialog->addComboItem(page, maPlotKey, plotList, d);

  settings.getData(maLabelKey, d);
  dialog->addTextItem(page, maLabelKey, d);

  dialog->addIntItem(page, maPeriodKey, settings.getInt(maPeriodKey), 2, 100000);

  settings.getData(maTypeKey, d);
  dialog->addComboItem(page, maTypeKey, maList, d);

  dialog->addCheckItem(page, showMAKey, settings.getInt(showMAKey));

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

  dialog->getItem(showMFIKey, d);
  settings.setData(showMFIKey, d);

  dialog->getItem(maColorKey, d);
  settings.setData(maColorKey, d);

  dialog->getItem(maPlotKey, d);
  settings.setData(maPlotKey, d);

  dialog->getItem(maLabelKey, d);
  settings.setData(maLabelKey, d);

  dialog->getItem(maPeriodKey, d);
  settings.setData(maPeriodKey, d);

  dialog->getItem(maTypeKey, d);
  settings.setData(maTypeKey, d);

  dialog->getItem(showMAKey, d);
  settings.setData(showMAKey, d);

  delete dialog;
  return rc;
}


