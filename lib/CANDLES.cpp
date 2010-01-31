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

#include "CANDLES.h"

#include <QtDebug>


CANDLES::CANDLES ()
{
  indicator = "CANDLES";

  QString d;
  d = "green";
  settings.setData(colorKey, d);

  d = "Line";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);
}

int CANDLES::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(colorKey, s);
  QColor color(s);

  PlotLine *line = getCANDLES(data, color);
  if (! line)
    return 1;

  line->setColor(color);

  settings.getData(labelKey, s);
  line->setLabel(s);

  ind.addLine(line);

  return 0;
}

int CANDLES::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,CANDLES,<NAME>,<CANDLE_COLOR>

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

  QColor color(set[3]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::calculate: invalid color" << set[3];
    return 1;
  }

  PlotLine *line = getCANDLES(data, color);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * CANDLES::getCANDLES (BarData *data, QColor &color)
{
  PlotLine *line = new PlotLine;
  line->setType(PlotLine::Candle);

  int loop;
  int size = data->count();
  for (loop = 0; loop < size; loop++)
  {
    double cl = data->getClose(loop);
    double op = data->getOpen(loop);
    bool fillFlag = FALSE;

    if (op != 0)
    {
      if (cl < op)
        fillFlag = TRUE;
    }

    line->append(color, op, data->getHigh(loop), data->getLow(loop), cl, fillFlag);
  }

  return line;
}

int CANDLES::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(colorKey, d);
  dialog->addColorItem(page, colorKey, d);

  settings.getData(labelKey, d);
  dialog->addTextItem(page, labelKey, d);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  dialog->getItem(colorKey, d);
  settings.setData(colorKey, d);

  dialog->getItem(labelKey, d);
  settings.setData(labelKey, d);

  delete dialog;
  return rc;
}

