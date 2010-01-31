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

#include "STOCHF.h"

#include <QtDebug>


STOCHF::STOCHF ()
{
  indicator = "STOCHF";
  fkcKey = QObject::tr("Fast K Color");
  fdcKey = QObject::tr("Fast D Color");
  fkpKey = QObject::tr("Fast K Plot");
  fdpKey = QObject::tr("Fast D Plot");
  fklKey = QObject::tr("Fast K Label");
  fdlKey = QObject::tr("Fast D Label");
  fdpdKey = QObject::tr("Fast D Period");
  fkpdKey = QObject::tr("Fast K Period");
  fdmaKey = QObject::tr("Fast D MA");

  QString d;
  d = "red";
  settings.setData(fkcKey, d);

  d = "yellow";
  settings.setData(fdcKey, d);

  d = "Line";
  settings.setData(fkpKey, d);

  d = "Dash";
  settings.setData(fdpKey, d);

  d = "K";
  settings.setData(fklKey, d);

  d = "D";
  settings.setData(fdlKey, d);

  settings.setData(fkpdKey, 5);
  settings.setData(fdpdKey, 3);

  d = "SMA";
  settings.setData(fdmaKey, d);
}

int STOCHF::getIndicator (Indicator &ind, BarData *data)
{
  int fastk = settings.getInt(fkpdKey);
  int fastd = settings.getInt(fdpdKey);

  QString s;
  settings.getData(fdmaKey, s);
  int ma = maList.indexOf(s);

  QList<PlotLine *> l;
  int rc = getSTOCHF(data, fastk, fastd, ma, l);
  if (rc || l.count() != 2)
  {
    qDeleteAll(l);
    return 1;
  }

  // fastk line
  PlotLine *line = l.at(0);
  settings.getData(fkcKey, s);
  line->setColor(s);

  settings.getData(fkpKey, s);
  line->setType(s);

  settings.getData(fklKey, s);
  line->setLabel(s);

  ind.addLine(line);

  // fastd line
  line = l.at(1);
  settings.getData(fdcKey, s);
  line->setColor(s);

  settings.getData(fdpKey, s);
  line->setType(s);

  settings.getData(fdlKey, s);
  line->setLabel(s);

  ind.addLine(line);

  return 0;
}

int STOCHF::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,STOCHF,<NAME_FASTK>,<NAME_FASTD>,<FASTK_PERIOD>,<FASTD_PERIOD>,<FASTD_MA_TYPE>

  if (set.count() != 7)
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
  int fkp = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fastk period" << set[4];
    return 1;
  }

  int fdp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fastd period" << set[5];
    return 1;
  }

  int ma = maList.indexOf(set[6]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid ma" << set[6];
    return 1;
  }

  QList<PlotLine *> l;
  int rc = getSTOCHF(data, fkp, fdp, ma, l);
  if (rc || l.count() != 2)
  {
    qDeleteAll(l);
    return 1;
  }

  tlines.insert(set[2], l.at(0));
  tlines.insert(set[3], l.at(1));

  return 0;
}

int STOCHF::getSTOCHF (BarData *data, int fkp, int fdp, int ma, QList<PlotLine *> &l)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_STOCHF(0, size - 1, &high[0], &low[0], &close[0], fkp, fdp, (TA_MAType) ma, &outBeg, &outNb, &out[0], &out2[0]);
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

  l.append(line);
  l.append(line2);

  return 0;
}

int STOCHF::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("FastK");
  dialog->addPage(page, k);

  settings.getData(fkcKey, d);
  dialog->addColorItem(page, fkcKey, d);

  settings.getData(fkpKey, d);
  dialog->addComboItem(page, fkpKey, plotList, d);

  settings.getData(fklKey, d);
  dialog->addTextItem(page, fklKey, d);

  dialog->addIntItem(page, fkpdKey, settings.getInt(fkpdKey), 1, 100000);

  page++;
  k = QObject::tr("FastD");
  dialog->addPage(page, k);

  settings.getData(fdcKey, d);
  dialog->addColorItem(page, fdcKey, d);

  settings.getData(fdpKey, d);
  dialog->addComboItem(page, fdpKey, plotList, d);

  settings.getData(fdlKey, d);
  dialog->addTextItem(page, fdlKey, d);

  dialog->addIntItem(page, fdpdKey, settings.getInt(fdpdKey), 1, 100000);

  settings.getData(fdmaKey, d);
  dialog->addComboItem(page, fdmaKey, maList, d);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  dialog->getItem(fkcKey, d);
  settings.setData(fkcKey, d);

  dialog->getItem(fkpKey, d);
  settings.setData(fkpKey, d);

  dialog->getItem(fklKey, d);
  settings.setData(fklKey, d);

  dialog->getItem(fdcKey, d);
  settings.setData(fdcKey, d);

  dialog->getItem(fdpKey, d);
  settings.setData(fdpKey, d);

  dialog->getItem(fdlKey, d);
  settings.setData(fdlKey, d);

  dialog->getItem(fkpdKey, d);
  settings.setData(fkpdKey, d);

  dialog->getItem(fdpdKey, d);
  settings.setData(fdpdKey, d);

  dialog->getItem(fdmaKey, d);
  settings.setData(fdmaKey, d);

  delete dialog;
  return rc;
}

