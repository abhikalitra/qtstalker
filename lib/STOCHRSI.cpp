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

#include "STOCHRSI.h"
#include "ta_libc.h"

#include <QtDebug>


STOCHRSI::STOCHRSI ()
{
  indicator = "STOCHRSI";
  fkcKey = QObject::tr("Fast K Color");
  fdcKey = QObject::tr("Fast D Color");
  fkpKey = QObject::tr("Fast K Plot");
  fdpKey = QObject::tr("Fast D Plot");
  fklKey = QObject::tr("Fast K Label");
  fdlKey = QObject::tr("Fast D Label");
  fdpdKey = QObject::tr("Fast D Period");
  fkpdKey = QObject::tr("Fast K Period");
  fdmaKey = QObject::tr("Fast D MA");
  ref1Key = QObject::tr("Ref. 1 Line");
  ref2Key = QObject::tr("Ref. 2 Line");
  ref1ColorKey = QObject::tr("Ref 1 Color");
  ref2ColorKey = QObject::tr("Ref 2 Color");

  QString d;
  d = "red";
  settings.setData(fkcKey, d);

  d = "yellow";
  settings.setData(fdcKey, d);

  d = "white";
  settings.setData(ref1ColorKey, d);
  settings.setData(ref2ColorKey, d);

  d = "Line";
  settings.setData(fkpKey, d);

  d = "Dash";
  settings.setData(fdpKey, d);

  d = "STORSI_K";
  settings.setData(fklKey, d);

  d = "STORSI_D";
  settings.setData(fdlKey, d);

  settings.setData(fkpdKey, 5);
  settings.setData(fdpdKey, 3);

  d = "SMA";
  settings.setData(fdmaKey, d);

  d = "Close";
  settings.setData(inputKey, d);

  settings.setData(periodKey, 14);

  settings.setData(ref1Key, 25);
  settings.setData(ref2Key, 75);
}

int STOCHRSI::getIndicator (Indicator &ind, BarData *data)
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
  int fastk = settings.getInt(fkpdKey);
  int fastd = settings.getInt(fdpdKey);

  settings.getData(fdmaKey, s);
  int ma = maList.indexOf(s);

  QList<PlotLine *> l;
  int rc = getSTOCHRSI(in, period, fastk, fastd, ma, l);
  if (rc || l.count() != 2)
  {
    qDeleteAll(l);
    delete in;
    return 1;
  }

  PlotLine *ref1 = new PlotLine;
  ref1->setType(PlotLine::Horizontal);
  settings.getData(ref1ColorKey, s);
  ref1->setColor(s);
  ref1->append(settings.getInt(ref1Key));
  ind.addLine(ref1);

  PlotLine *ref2 = new PlotLine;
  ref2->setType(PlotLine::Horizontal);
  settings.getData(ref2ColorKey, s);
  ref2->setColor(s);
  ref2->append(settings.getInt(ref2Key));
  ind.addLine(ref2);

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

  delete in;

  return 0;
}

int STOCHRSI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,STOCHRSI,<INPUT>,<NAME_FASTK>,<NAME_FASTD>,<PERIOD>,<FASTK_PERIOD>,<FASTD_PERIOD>,
  // <FASTD_MA_TYPE>

  if (set.count() != 9)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
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

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period" << set[5];
    return 1;
  }

  int fkp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fastkk period" << set[6];
    return 1;
  }

  int fdp = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fastd period" << set[7];
    return 1;
  }

  int ma = maList.indexOf(set[8]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid ma settings" << set[8];
    return 1;
  }

  QList<PlotLine *> l;
  int rc = getSTOCHRSI(in, period, fkp, fdp, ma, l);
  if (rc || l.count() != 2)
  {
    qDeleteAll(l);
    return 1;
  }

  tlines.insert(set[3], l.at(0));
  tlines.insert(set[4], l.at(1));

  return 0;
}

int STOCHRSI::getSTOCHRSI (PlotLine *in, int period, int fkp, int fdp, int ma, QList<PlotLine *> &l)
{
  int size = in->getSize();
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_STOCHRSI(0, in->getSize() - 1, &input[0], period, fkp, fdp, (TA_MAType) ma, &outBeg, &outNb, &out[0], &out2[0]);
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

int STOCHRSI::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  dialog->addIntItem(page, periodKey, settings.getInt(periodKey), 2, 100000);

  settings.getData(inputKey, d);
  dialog->addComboItem(page, inputKey, inputList, d);

  page++;
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

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  settings.getData(ref1ColorKey, d);
  dialog->addColorItem(page, ref1ColorKey, d);

  settings.getData(ref2ColorKey, d);
  dialog->addColorItem(page, ref2ColorKey, d);

  dialog->addIntItem(page, ref1Key, settings.getInt(ref1Key), 0, 100);

  dialog->addIntItem(page, ref2Key, settings.getInt(ref2Key), 0, 100);

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

  dialog->getItem(periodKey, d);
  settings.setData(periodKey, d);

  dialog->getItem(inputKey, d);
  settings.setData(inputKey, d);

  dialog->getItem(ref1ColorKey, d);
  settings.setData(ref1ColorKey, d);

  dialog->getItem(ref2ColorKey, d);
  settings.setData(ref2ColorKey, d);

  dialog->getItem(ref1Key, d);
  settings.setData(ref1Key, d);

  dialog->getItem(ref2Key, d);
  settings.setData(ref2Key, d);

  delete dialog;
  return rc;
}

