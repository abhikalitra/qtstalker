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

#include "STOCH.h"

#include <QtDebug>


STOCH::STOCH ()
{
  indicator = "STOCH";
  skcKey = QObject::tr("SlowK Color");
  sdcKey = QObject::tr("SlowD Color");
  skpKey = QObject::tr("SlowK Plot");
  sdpKey = QObject::tr("SlowD Plot");
  sklKey = QObject::tr("SlowK Label");
  sdlKey = QObject::tr("SlowD Label");
  fkpKey = QObject::tr("FastK Period");
  skpdKey = QObject::tr("SlowK Period");
  sdpdKey = QObject::tr("SlowD Period");
  skmaKey = QObject::tr("SlowK MA");
  sdmaKey = QObject::tr("SlowD MA");
  ref1Key = QObject::tr("Ref. 1 Line");
  ref2Key = QObject::tr("Ref. 2 Line");
  ref1ColorKey = QObject::tr("Ref 1 Color");
  ref2ColorKey = QObject::tr("Ref 2 Color");

  QString d;
  d = "red";
  settings.setData(skcKey, d);

  d = "yellow";
  settings.setData(sdcKey, d);

  d = "white";
  settings.setData(ref1ColorKey, d);
  settings.setData(ref2ColorKey, d);

  d = "Line";
  settings.setData(skpKey, d);

  d = "Dash";
  settings.setData(sdpKey, d);

  d = "SLOW_K";
  settings.setData(sklKey, d);

  d = "SLOW_D";
  settings.setData(sdlKey, d);

  settings.setData(fkpKey, 5);
  settings.setData(skpdKey, 3);
  settings.setData(sdpdKey, 3);

  d = "SMA";
  settings.setData(skmaKey, d);
  settings.setData(sdmaKey, d);

  settings.setData(ref1Key, 25);
  settings.setData(ref2Key, 75);
}

int STOCH::getIndicator (Indicator &ind, BarData *data)
{
  int fastk = settings.getInt(fkpKey);
  int slowk = settings.getInt(skpdKey);
  int slowd = settings.getInt(sdpdKey);

  QString s;
  settings.getData(skmaKey, s);
  int skma = maList.indexOf(s);

  settings.getData(sdmaKey, s);
  int sdma = maList.indexOf(s);

  QList<PlotLine *> l;
  int rc = getSTOCH(data, fastk, slowk, skma, slowd, sdma, l);
  if (rc || l.count() != 2)
  {
    qDeleteAll(l);
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

  // slowk line
  PlotLine *line = l.at(0);
  settings.getData(skcKey, s);
  line->setColor(s);

  settings.getData(skpKey, s);
  line->setType(s);

  settings.getData(sklKey, s);
  line->setLabel(s);

  ind.addLine(line);

  // slowd line
  line = l.at(1);
  settings.getData(sdcKey, s);
  line->setColor(s);

  settings.getData(sdpKey, s);
  line->setType(s);

  settings.getData(sdlKey, s);
  line->setLabel(s);

  ind.addLine(line);

  return 0;
}

int STOCH::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,STOCH,<NAME_SLOWK>,<NAME_SLOWD>,<FASTK_PERIOD>,<SLOWK_PERIOD>,<SLOWK_MA_TYPE>,
  // <SLOWD_PERIOD>,<SLOWD_MA_TYPE>

  if (set.count() != 9)
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

  int skp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid slowk period" << set[5];
    return 1;
  }

  int kma = maList.indexOf(set[6]);
  if (kma == -1)
  {
    qDebug() << indicator << "::calculate: invalid slowk ma" << set[6];
    return 1;
  }

  int sdp = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid slowd period" << set[7];
    return 1;
  }

  int dma = maList.indexOf(set[8]);
  if (dma == -1)
  {
    qDebug() << indicator << "::calculate: invalid slowd ma" << set[8];
    return 1;
  }

  QList<PlotLine *> l;
  int rc = getSTOCH(data, fkp, skp, kma, sdp, dma, l);
  if (rc || l.count() != 2)
  {
    qDeleteAll(l);
    return 1;
  }

  tlines.insert(set[2], l.at(0));
  tlines.insert(set[3], l.at(1));

  return 0;
}

int STOCH::getSTOCH (BarData *data, int fkp, int skp, int skma, int sdp, int sdma, QList<PlotLine *> &l)
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
  TA_RetCode rc = TA_STOCH(0, size - 1, &high[0], &low[0], &close[0], fkp, skp, (TA_MAType) skma, sdp, (TA_MAType) sdma, &outBeg, &outNb, &out[0], &out2[0]);
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

int STOCH::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("SlowK");
  dialog->addPage(page, k);

  settings.getData(skcKey, d);
  dialog->addColorItem(page, skcKey, d);

  settings.getData(skpKey, d);
  dialog->addComboItem(page, skpKey, plotList, d);

  settings.getData(sklKey, d);
  dialog->addTextItem(page, sklKey, d);

  dialog->addIntItem(page, fkpKey, settings.getInt(fkpKey), 1, 100000);

  dialog->addIntItem(page, skpdKey, settings.getInt(skpdKey), 1, 100000);

  settings.getData(skmaKey, d);
  dialog->addComboItem(page, skmaKey, maList, d);

  page++;
  k = QObject::tr("SlowD");
  dialog->addPage(page, k);

  settings.getData(sdcKey, d);
  dialog->addColorItem(page, sdcKey, d);

  settings.getData(sdpKey, d);
  dialog->addComboItem(page, sdpKey, plotList, d);

  settings.getData(sdlKey, d);
  dialog->addTextItem(page, sdlKey, d);

  dialog->addIntItem(page, sdpdKey, settings.getInt(sdpdKey), 1, 100000);

  settings.getData(sdmaKey, d);
  dialog->addComboItem(page, sdmaKey, maList, d);

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

  dialog->getItem(skcKey, d);
  settings.setData(skcKey, d);

  dialog->getItem(skpKey, d);
  settings.setData(skpKey, d);

  dialog->getItem(sklKey, d);
  settings.setData(sklKey, d);

  dialog->getItem(sdcKey, d);
  settings.setData(sdcKey, d);

  dialog->getItem(sdpKey, d);
  settings.setData(sdpKey, d);

  dialog->getItem(sdlKey, d);
  settings.setData(sdlKey, d);

  dialog->getItem(fkpKey, d);
  settings.setData(fkpKey, d);

  dialog->getItem(skpdKey, d);
  settings.setData(skpdKey, d);

  dialog->getItem(sdpdKey, d);
  settings.setData(sdpdKey, d);

  dialog->getItem(skmaKey, d);
  settings.setData(skmaKey, d);

  dialog->getItem(sdmaKey, d);
  settings.setData(sdmaKey, d);

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

