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

#include "MINMAXINDEX.h"

#include <QtDebug>


MINMAXINDEX::MINMAXINDEX ()
{
  indicator = "MINMAXINDEX";
  mincKey = QObject::tr("MIN Color");
  maxcKey = QObject::tr("MAX Color");
  minpKey = QObject::tr("MIN Plot");
  maxpKey = QObject::tr("MAX Plot");
  minlKey = QObject::tr("MIN Label");
  maxlKey = QObject::tr("MAX Label");

  QString d;
  d = "red";
  settings.setData(mincKey, d);
  settings.setData(maxcKey, d);

  d = "Line";
  settings.setData(minpKey, d);
  settings.setData(maxpKey, d);

  d = "MIN";
  settings.setData(minlKey, d);

  d = "MAX";
  settings.setData(maxlKey, d);

  d = "Close";
  settings.setData(inputKey, d);

  settings.setData(periodKey, 30);
}

int MINMAXINDEX::getIndicator (Indicator &ind, BarData *data)
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

  QList<PlotLine *> l;
  int rc = getMINMAXINDEX(in, period, l);
  if (rc)
  {
    qDeleteAll(l);
    delete in;
    return 1;
  }

  // min line
  PlotLine *line = l.at(0);
  settings.getData(mincKey, s);
  line->setColor(s);

  settings.getData(minpKey, s);
  line->setType(s);

  settings.getData(minlKey, s);
  line->setLabel(s);

  ind.addLine(line);

  // max line
  line = l.at(1);
  settings.getData(maxcKey, s);
  line->setColor(s);

  settings.getData(maxpKey, s);
  line->setType(s);

  settings.getData(maxlKey, s);
  line->setLabel(s);

  ind.addLine(line);

  delete in;

  return 0;
}

int MINMAXINDEX::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MINMAXINDEX,<INPUT>,<NAME_MIN>,<NAME_MAX>,<PERIOD>

  if (set.count() != 7)
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
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period" << set[5];
    return 1;
  }

  QList<PlotLine *> l;
  int rc = getMINMAXINDEX(in, period, l);
  if (rc)
  {
    qDeleteAll(l);
    return 1;
  }

  tlines.insert(set[3], l.at(0));
  tlines.insert(set[4], l.at(1));

  return 0;
}

int MINMAXINDEX::getMINMAXINDEX (PlotLine *in, int period, QList<PlotLine *> &l)
{
  int size = in->getSize();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Integer out[size];
  TA_Integer out2[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_MINMAXINDEX(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *min = new PlotLine;
  PlotLine *max = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    min->append(out[loop]);
    max->append(out2[loop]);
  }

  l.append(min);
  l.append(max);

  return 0;
}

int MINMAXINDEX::dialog ()
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

  page++;
  k = QObject::tr("Min");
  dialog->addPage(page, k);

  settings.getData(mincKey, d);
  dialog->addColorItem(page, mincKey, d);

  settings.getData(minpKey, d);
  dialog->addComboItem(page, minpKey, plotList, d);

  settings.getData(minlKey, d);
  dialog->addTextItem(page, minlKey, d);

  page++;
  k = QObject::tr("Max");
  dialog->addPage(page, k);

  settings.getData(maxcKey, d);
  dialog->addColorItem(page, maxcKey, d);

  settings.getData(maxpKey, d);
  dialog->addComboItem(page, maxpKey, plotList, d);

  settings.getData(maxlKey, d);
  dialog->addTextItem(page, maxlKey, d);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  dialog->getItem(mincKey, d);
  settings.setData(mincKey, d);

  dialog->getItem(minpKey, d);
  settings.setData(minpKey, d);

  dialog->getItem(minlKey, d);
  settings.setData(minlKey, d);

  dialog->getItem(maxcKey, d);
  settings.setData(maxcKey, d);

  dialog->getItem(maxpKey, d);
  settings.setData(maxpKey, d);

  dialog->getItem(maxlKey, d);
  settings.setData(maxlKey, d);

  dialog->getItem(inputKey, d);
  settings.setData(inputKey, d);

  dialog->getItem(periodKey, d);
  settings.setData(periodKey, d);

  delete dialog;
  return rc;
}

