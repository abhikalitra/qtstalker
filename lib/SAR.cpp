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

#include "SAR.h"
#include "BARS.h"

#include <QtDebug>


SAR::SAR ()
{
  indicator = "SAR";

  settings.setData(Color, "red");
  settings.setData(Plot, "Dot");
  settings.setData(Label, indicator);
  settings.setData(Init, 0.02);
  settings.setData(Max, 0.2);
}

int SAR::getIndicator (Indicator &ind, BarData *data)
{
  double init = settings.getDouble(Init);
  double max = settings.getDouble(Max);

  PlotLine *line = getSAR(data, init, max);
  if (! line)
    return 1;

  BARS bars;
  bars.turnOffMAs();
  int rc = bars.getIndicator(ind, data);
  if (rc)
  {
    delete line;
    return 1;
  }

  QString s;
  settings.getData(Color, s);
  line->setColor(s);

  settings.getData(Plot, s);
  line->setType(s);

  settings.getData(Label, s);
  line->setLabel(s);

  ind.addLine(line);

  return 0;
}

int SAR::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,SAR,<NAME>,<INITIAL_STEP>,<MAX_STEP>

  if (set.count() != 5)
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
  double init = set[3].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid init" << set[3];
    return 1;
  }

  double max = set[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid max" << set[4];
    return 1;
  }

  PlotLine *line = getSAR(data, init, max);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * SAR::getSAR (BarData *data, double init, double max)
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
  TA_RetCode rc = TA_SAR(0, size - 1, &high[0], &low[0], init, max, &outBeg, &outNb, &out[0]);
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

int SAR::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  dialog->addDoubleItem(Init, page, QObject::tr("Initial"), settings.getDouble(Init), 0, 100000);

  dialog->addDoubleItem(Max, page, QObject::tr("Max"), settings.getDouble(Max), 0, 100000);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  getDialogSettings(dialog);

  delete dialog;
  return rc;
}

