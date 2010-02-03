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

#include "AROON.h"

#include <QtDebug>


AROON::AROON ()
{
  indicator = "AROON";
  dcKey = QObject::tr("Down Color");
  ucKey = QObject::tr("Up Color");
  dpKey = QObject::tr("Down Plot");
  upKey = QObject::tr("Up Plot");
  dlKey = QObject::tr("Down Label");
  ulKey = QObject::tr("Up Label");
  oscKey = QObject::tr("Oscillator");

  methodList << "UP";
  methodList << "DOWN";
  methodList << "OSC";

  QString d;
  d = "red";
  settings.setData(dcKey, d);
  settings.setData(colorKey, d);

  d = "green";
  settings.setData(ucKey, d);

  d = "Line";
  settings.setData(dpKey, d);
  settings.setData(upKey, d);
  settings.setData(plotKey, d);

  d = "AROON_D";
  settings.setData(dlKey, d);

  d = "AROON_U";
  settings.setData(ulKey, d);

  d = "AROON_OSC";
  settings.setData(labelKey, d);

  settings.setData(periodKey, 14);

  settings.setData(oscKey, 0);
}

int AROON::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  int period = settings.getInt(periodKey);
  int osc = settings.getInt(oscKey);

  if (osc)
  {
    PlotLine *line = getAROON(data, period, 2);
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

  // get arron up line
  PlotLine *up = getAROON(data, period, 0);
  if (! up)
    return 1;

  settings.getData(ucKey, s);
  up->setColor(s);

  settings.getData(upKey, s);
  up->setType(s);

  settings.getData(ulKey, s);
  up->setLabel(s);

  // get aroon down line
  PlotLine *down = getAROON(data, period, 1);
  if (! down)
  {
    delete up;
    return 1;
  }

  settings.getData(dcKey, s);
  down->setColor(s);

  settings.getData(dpKey, s);
  down->setType(s);

  settings.getData(dlKey, s);
  down->setLabel(s);

  ind.addLine(up);
  ind.addLine(down);

  return 0;
}

int AROON::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,AROON,<NAME>,<PERIOD>,<METHOD>

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
  int period = set[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period" << set[3];
    return 1;
  }

  int method = methodList.indexOf(set[4]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[4];
    return 1;
  }

  PlotLine *line = getAROON(data, period, method);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * AROON::getAROON (BarData *data, int period, int method)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  TA_Real out2[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_AROON(0, size - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0], &out2[0]);
      break;
    case 1:
      // we switch out with out2 only here
      rc = TA_AROON(0, size - 1, &high[0], &low[0], period, &outBeg, &outNb, &out2[0], &out[0]);
      break;
    case 2:
      rc = TA_AROONOSC(0, size - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

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

int AROON::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(ucKey, d);
  QColor c2(d);
  dialog->addColorItem(page, ucKey, c2);

  settings.getData(dcKey, d);
  QColor c(d);
  dialog->addColorItem(page, dcKey, c);

  settings.getData(upKey, d);
  dialog->addComboItem(page, upKey, plotList, d);

  settings.getData(dpKey, d);
  dialog->addComboItem(page, dpKey, plotList, d);

  settings.getData(ulKey, d);
  dialog->addTextItem(page, ulKey, d);

  settings.getData(dlKey, d);
  dialog->addTextItem(page, dlKey, d);

  dialog->addIntItem(page, periodKey, settings.getInt(periodKey), 2, 100000);

  page++;
  k = QObject::tr("OSC");
  dialog->addPage(page, k);

  settings.getData(colorKey, d);
  dialog->addColorItem(page, colorKey, d);

  settings.getData(plotKey, d);
  dialog->addComboItem(page, plotKey, plotList, d);

  settings.getData(labelKey, d);
  dialog->addTextItem(page, labelKey, d);

  dialog->addCheckItem(page, oscKey, settings.getInt(oscKey));

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  dialog->getItem(dcKey, d);
  settings.setData(dcKey, d);

  dialog->getItem(ucKey, d);
  settings.setData(ucKey, d);

  dialog->getItem(dpKey, d);
  settings.setData(dpKey, d);

  dialog->getItem(upKey, d);
  settings.setData(upKey, d);

  dialog->getItem(dlKey, d);
  settings.setData(dlKey, d);

  dialog->getItem(ulKey, d);
  settings.setData(ulKey, d);

  dialog->getItem(periodKey, d);
  settings.setData(periodKey, d);

  dialog->getItem(colorKey, d);
  settings.setData(colorKey, d);

  dialog->getItem(plotKey, d);
  settings.setData(plotKey, d);

  dialog->getItem(labelKey, d);
  settings.setData(labelKey, d);

  dialog->getItem(oscKey, d);
  settings.setData(oscKey, d);

  delete dialog;
  return rc;
}

