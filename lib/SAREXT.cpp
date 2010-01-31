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

#include "SAREXT.h"

#include <QtDebug>


SAREXT::SAREXT ()
{
  indicator = "SAREXT";
  startKey = QObject::tr("Start");
  offrevKey = QObject::tr("Offset Reverse");
  ilaKey = QObject::tr("Initial Long Acceleration");
  laKey = QObject::tr("Long Acceleration");
  mlaKey = QObject::tr("Max Long Acceleration");
  isaKey = QObject::tr("Initial Short Acceleration");
  saKey = QObject::tr("Short Acceleration");
  msaKey = QObject::tr("Max Short Acceleration");

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "Line";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);

  settings.setData(startKey, 0);
  settings.setData(offrevKey, 0.2);
  settings.setData(ilaKey, 0.02);
  settings.setData(laKey, 0.02);
  settings.setData(mlaKey, 0.2);
  settings.setData(isaKey, 0.02);
  settings.setData(saKey, 0.02);
  settings.setData(msaKey, 0.2);
}

int SAREXT::getIndicator (Indicator &ind, BarData *data)
{
  double start = settings.getDouble(startKey);
  double offrev = settings.getDouble(offrevKey);
  double ail = settings.getDouble(ilaKey);
  double al = settings.getDouble(laKey);
  double aml = settings.getDouble(mlaKey);
  double ais = settings.getDouble(isaKey);
  double as = settings.getDouble(saKey);
  double ams = settings.getDouble(msaKey);

  PlotLine *line = getSAREXT(data, start, offrev, ail, al, aml, ais, as, ams);
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

int SAREXT::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,SAREXT,<NAME>,<START>,<OFFSET_REVERSE>,<ACCEL_INIT_LONG>,<ACCEL_LONG>,<ACCEL_MAX_LONG>,
  // <ACCEL_INIT_SHORT>,<ACCEL_SHORT>,<ACCEL_MAX_SHORT>

  if (set.count() != 11)
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
  double start = set[3].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid start" << set[3];
    return 1;
  }

  double offrev = set[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid offset reverse" << set[4];
    return 1;
  }

  double ail = set[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid accel init long" << set[5];
    return 1;
  }

  double al = set[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid accel long" << set[6];
    return 1;
  }

  double aml = set[7].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid accel max long" << set[7];
    return 1;
  }

  double ais = set[8].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid accel init short" << set[8];
    return 1;
  }

  double as = set[9].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid accel short" << set[9];
    return 1;
  }

  double ams = set[10].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid accel max short" << set[10];
    return 1;
  }

  PlotLine *line = getSAREXT(data, start, offrev, ail, al, aml, ais, as, ams);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * SAREXT::getSAREXT (BarData *data, double start, double offrev, double ail, double al,
			      double aml, double ais, double as, double ams)
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
  TA_RetCode rc = TA_SAREXT(0, size - 1, &high[0], &low[0], start, offrev, ail, al, aml,
			    ais, as, ams, &outBeg, &outNb, &out[0]);
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

int SAREXT::dialog ()
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

  dialog->addDoubleItem(page, startKey, settings.getDouble(startKey), -100000, 100000);
  dialog->addDoubleItem(page, offrevKey, settings.getDouble(offrevKey), 0, 100000);
  dialog->addDoubleItem(page, ilaKey, settings.getDouble(ilaKey), 0, 100000);
  dialog->addDoubleItem(page, laKey, settings.getDouble(laKey), 0, 100000);
  dialog->addDoubleItem(page, mlaKey, settings.getDouble(mlaKey), 0, 100000);
  dialog->addDoubleItem(page, isaKey, settings.getDouble(isaKey), 0, 100000);
  dialog->addDoubleItem(page, saKey, settings.getDouble(saKey), 0, 100000);
  dialog->addDoubleItem(page, msaKey, settings.getDouble(msaKey), 0, 100000);

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

  dialog->getItem(startKey, d);
  settings.setData(startKey, d);

  dialog->getItem(offrevKey, d);
  settings.setData(offrevKey, d);

  dialog->getItem(ilaKey, d);
  settings.setData(ilaKey, d);

  dialog->getItem(laKey, d);
  settings.setData(laKey, d);

  dialog->getItem(mlaKey, d);
  settings.setData(mlaKey, d);

  dialog->getItem(isaKey, d);
  settings.setData(isaKey, d);

  dialog->getItem(saKey, d);
  settings.setData(saKey, d);

  dialog->getItem(msaKey, d);
  settings.setData(msaKey, d);

  delete dialog;
  return rc;
}

