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

#include "FI.h"

#include <QtDebug>


FI::FI ()
{
  indicator = "FI";

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "Histogram Bar";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);

  d = "EMA";
  settings.setData(maKey, d);

  settings.setData(periodKey, 2);
}

int FI::getIndicator (Indicator &ind, BarData *data)
{
  PlotLine *line = getFI(data);
  if (! line)
    return 1;

  int period = settings.getInt(periodKey);

  QString s;
  settings.getData(maKey, s);
  int mat = maList.indexOf(s);

  PlotLine *ma = getMA(line, period, mat);
  if (! ma)
  {
    delete line;
    return 1;
  }

  settings.getData(colorKey, s);
  ma->setColor(s);

  settings.getData(plotKey, s);
  ma->setType(s);

  settings.getData(labelKey, s);
  ma->setLabel(s);

  ind.addLine(ma);

  return 0;
}

int FI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,FI,NAME

  if (set.count() != 3)
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

  PlotLine *line = getFI(data);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * FI::getFI (BarData *data)
{
  PlotLine *fi = new PlotLine();
  int loop;
  double force = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double cdiff = data->getClose(loop) - data->getClose(loop - 1);
    force = data->getVolume(loop) * cdiff;
    fi->append(force);
  }

  return fi;
}

int FI::dialog ()
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

  dialog->getItem(periodKey, d);
  settings.setData(periodKey, d);

  dialog->getItem(maKey, d);
  settings.setData(maKey, d);

  delete dialog;
  return rc;
}

