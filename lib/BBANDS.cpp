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

#include "BBANDS.h"
#include "BARS.h"

#include <QtDebug>

BBANDS::BBANDS ()
{
  indicator = "BBANDS";
  ucKey = QObject::tr("Upper Color");
  mcKey = QObject::tr("Middle Color");
  lcKey = QObject::tr("Lower Color");
  upKey = QObject::tr("Upper Plot");
  mpKey = QObject::tr("Middle Plot");
  lpKey = QObject::tr("Lower Plot");
  ulKey = QObject::tr("Upper Label");
  mlKey = QObject::tr("Middle Label");
  llKey = QObject::tr("Lower Label");
  udKey = QObject::tr("Upper Deviation");
  ldKey = QObject::tr("Lower Deviation");

  QString d;
  d = "red";
  settings.setData(ucKey, d);
  settings.setData(mcKey, d);
  settings.setData(lcKey, d);

  d = "Line";
  settings.setData(upKey, d);
  settings.setData(mpKey, d);
  settings.setData(lpKey, d);

  d = "BB_U";
  settings.setData(ulKey, d);

  d = "BB_M";
  settings.setData(mlKey, d);

  d = "BB_L";
  settings.setData(llKey, d);

  d = "Close";
  settings.setData(inputKey, d);

  settings.setData(periodKey, 20);

  settings.setData(udKey, 2);

  settings.setData(ldKey, 2);

  d = "SMA";
  settings.setData(maTypeKey, d);
}

int BBANDS::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(inputKey, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(periodKey);
  double udev = settings.getDouble(udKey);
  double ldev = settings.getDouble(ldKey);

  settings.getData(maTypeKey, s);
  int ma = maList.indexOf(s);

  QList<PlotLine *> l;
  int rc = getBBANDS(in, period, udev, ldev, ma, l);
  if (rc)
  {
    delete in;
    return 1;
  }

  if (l.count() != 3)
  {
    delete in;
    qDeleteAll(l);
    return 1;
  }

  BARS bars;
  rc = bars.getIndicator(ind, data);
  if (rc)
  {
    delete in;
    qDeleteAll(l);
    return 1;
  }

  // upper line
  PlotLine *line = l.at(0);
  settings.getData(ucKey, s);
  line->setColor(s);

  settings.getData(upKey, s);
  line->setType(s);

  settings.getData(ulKey, s);
  line->setLabel(s);

  ind.addLine(line);

  // middle line
  line = l.at(1);
  settings.getData(mcKey, s);
  line->setColor(s);

  settings.getData(mpKey, s);
  line->setType(s);

  settings.getData(mlKey, s);
  line->setLabel(s);

  ind.addLine(line);

  // lower line
  line = l.at(2);
  settings.getData(lcKey, s);
  line->setColor(s);

  settings.getData(lpKey, s);
  line->setType(s);

  settings.getData(llKey, s);
  line->setLabel(s);

  ind.addLine(line);

  delete in;

  return 0;
}

int BBANDS::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,BBANDS,<INPUT>,<NAME_UPPER>,<NAME_MIDDLE>,<NAME_LOWER>,<PERIOD>,<UPPER_DEVIATION>,<LOWER_DEVIATION>,<MA_TYPE>

  if (set.count() != 10)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[3];
    return 1;
  }

  tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[4];
    return 1;
  }

  tl = tlines.value(set[5]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[5];
    return 1;
  }

  PlotLine *in = tlines.value(set[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[2]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[2];
      return 1;
    }

    tlines.insert(set[2], in);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period settings" << set[6];
    return 1;
  }

  double upDev = set[7].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid up deviation settings" << set[7];
    return 1;
  }

  double lowDev = set[8].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid lower deviation settings" << set[8];
    return 1;
  }

  int ma = maList.indexOf(set[9]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid ma settings" << set[9];
    return 1;
  }

  QList<PlotLine *> l;
  int rc = getBBANDS(in, period, upDev, lowDev, ma, l);
  if (rc)
  {
    delete in;
    return 1;
  }

  if (l.count() != 3)
  {
    delete in;
    qDeleteAll(l);
    return 1;
  }

  PlotLine *line = l.at(0);
  tlines.insert(set[3], line);
  line = l.at(1);
  tlines.insert(set[4], line);
  line = l.at(2);
  tlines.insert(set[5], line);

  return 0;
}

int BBANDS::getBBANDS (PlotLine *in, int period, double udev, double ldev, int ma, QList<PlotLine *> &_l)
{
  int size = in->getSize();
  TA_Real input[size];
  TA_Real upper[size];
  TA_Real middle[size];
  TA_Real lower[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_BBANDS(0, size - 1, &input[0], period, udev, ldev, (TA_MAType) ma, &outBeg, &outNb, &upper[0], &middle[0], &lower[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *u = new PlotLine;
  PlotLine *m = new PlotLine;
  PlotLine *l = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    u->append(upper[loop]);
    m->append(middle[loop]);
    l->append(lower[loop]);
  }

  _l.append(u);
  _l.append(m);
  _l.append(l);

  return 0;
}

int BBANDS::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(inputKey, d);
  dialog->addComboItem(page, inputKey, inputList, d);

  dialog->addIntItem(page, periodKey, settings.getInt(periodKey), 2, 100000);

  dialog->addDoubleItem(page, udKey, settings.getDouble(udKey), -100000, 100000);

  dialog->addDoubleItem(page, ldKey, settings.getDouble(ldKey), -100000, 100000);

  settings.getData(maTypeKey, d);
  dialog->addComboItem(page, maTypeKey, maList, d);

  page++;
  k = QObject::tr("Up");
  dialog->addPage(page, k);

  settings.getData(ucKey, d);
  dialog->addColorItem(page, ucKey, d);

  settings.getData(upKey, d);
  dialog->addComboItem(page, upKey, plotList, d);

  settings.getData(ulKey, d);
  dialog->addTextItem(page, ulKey, d);

  page++;
  k = QObject::tr("Mid");
  dialog->addPage(page, k);

  settings.getData(mcKey, d);
  dialog->addColorItem(page, mcKey, d);

  settings.getData(mpKey, d);
  dialog->addComboItem(page, mpKey, plotList, d);

  settings.getData(mlKey, d);
  dialog->addTextItem(page, mlKey, d);

  page++;
  k = QObject::tr("Low");
  dialog->addPage(page, k);

  settings.getData(lcKey, d);
  dialog->addColorItem(page, lcKey, d);

  settings.getData(lpKey, d);
  dialog->addComboItem(page, lpKey, plotList, d);

  settings.getData(llKey, d);
  dialog->addTextItem(page, llKey, d);

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

