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

  QString d;
  d = "red";
  settings.setData(dcKey, d);

  d = "green";
  settings.setData(ucKey, d);

  d = "Line";
  settings.setData(dpKey, d);
  settings.setData(upKey, d);

  d = "AROON_D";
  settings.setData(dlKey, d);

  d = "AROON_U";
  settings.setData(ulKey, d);

  settings.setData(periodKey, 14);
}

int AROON::getIndicator (Indicator &ind, BarData *data)
{
  int period = settings.getInt(periodKey);

  QList<PlotLine *> l;
  int rc = getAROON(data, period, l);
  if (rc)
    return 1;

  if (l.count() != 2)
  {
    qDeleteAll(l);
    return 1;
  }

  PlotLine *line = l.at(0);
  QString s;
  settings.getData(dcKey, s);
  line->setColor(s);

  settings.getData(dpKey, s);
  line->setType(s);

  settings.getData(dlKey, s);
  line->setLabel(s);

  ind.addLine(line);

  line = l.at(1);
  settings.getData(ucKey, s);
  line->setColor(s);

  settings.getData(upKey, s);
  line->setType(s);

  settings.getData(ulKey, s);
  line->setLabel(s);

  ind.addLine(line);

  return 0;
}

int AROON::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,AROON,<DOWN_NAME>,<UP_NAME>,<PERIOD>

  if (set.count() < 5)
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

  tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period settings" << set[4];
    return 1;
  }

  QList<PlotLine *> l;
  int rc = getAROON(data, period, l);
  if (rc)
    return 1;

  if (l.count() != 2)
  {
    qDeleteAll(l);
    return 1;
  }

  PlotLine *line = l.at(0);
  tlines.insert(set[2], line);
  line = l.at(1);
  tlines.insert(set[3], line);

  return 0;
}

int AROON::getAROON (BarData *data, int period, QList<PlotLine *> &l)
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
  TA_RetCode rc = TA_AROON(0, size - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *line2 = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    line->append(out[loop]);
    line2->append(out2[loop]);
  }

  l.append(line); // down
  l.append(line2); // up

  return 0;
}

int AROON::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(dcKey, d);
  QColor c(d);
  dialog->addColorItem(page, dcKey, c);

  settings.getData(ucKey, d);
  QColor c2(d);
  dialog->addColorItem(page, ucKey, c2);

  settings.getData(dpKey, d);
  dialog->addComboItem(page, dpKey, plotList, d);

  settings.getData(upKey, d);
  dialog->addComboItem(page, upKey, plotList, d);

  settings.getData(dlKey, d);
  dialog->addTextItem(page, dlKey, d);

  settings.getData(ulKey, d);
  dialog->addTextItem(page, ulKey, d);

  dialog->addIntItem(page, periodKey, settings.getInt(periodKey), 2, 100000);

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

  delete dialog;
  return rc;
}

