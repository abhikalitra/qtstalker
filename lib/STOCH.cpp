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

  settings.setData(SlowkColor, "red");
  settings.setData(SlowdColor, "yellow");
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
  settings.setData(SlowkPlot, "Line");
  settings.setData(SlowdPlot, "Dash");
  settings.setData(SlowkLabel, "SLOWK");
  settings.setData(SlowdLabel, "SLOWD");
  settings.setData(FastkPeriod, 5);
  settings.setData(SlowkPeriod, 3);
  settings.setData(SlowdPeriod, 3);
  settings.setData(SlowkMA, "SMA");
  settings.setData(SlowdMA, "SMA");
  settings.setData(Ref1, 25);
  settings.setData(Ref2, 75);
}

int STOCH::getIndicator (Indicator &ind, BarData *data)
{
  int fastk = settings.getInt(FastkPeriod);
  int slowk = settings.getInt(SlowkPeriod);
  int slowd = settings.getInt(SlowdPeriod);

  QString s;
  settings.getData(SlowkMA, s);
  int skma = maList.indexOf(s);

  settings.getData(SlowdMA, s);
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
  settings.getData(Ref1Color, s);
  ref1->setColor(s);
  ref1->append(settings.getInt(Ref1));
  ind.addLine(ref1);

  PlotLine *ref2 = new PlotLine;
  ref2->setType(PlotLine::Horizontal);
  settings.getData(Ref2Color, s);
  ref2->setColor(s);
  ref2->append(settings.getInt(Ref2));
  ind.addLine(ref2);

  // slowk line
  PlotLine *line = l.at(0);
  settings.getData(SlowkColor, s);
  line->setColor(s);

  settings.getData(SlowkPlot, s);
  line->setType(s);

  settings.getData(SlowkLabel, s);
  line->setLabel(s);

  ind.addLine(line);

  // slowd line
  line = l.at(1);
  settings.getData(SlowdColor, s);
  line->setColor(s);

  settings.getData(SlowdPlot, s);
  line->setType(s);

  settings.getData(SlowdLabel, s);
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

  settings.getData(SlowkColor, d);
  dialog->addColorItem(SlowkColor, page, QObject::tr("Color"), d);

  settings.getData(SlowkPlot, d);
  dialog->addComboItem(SlowkPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(SlowkLabel, d);
  dialog->addTextItem(SlowkLabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(FastkPeriod, page, QObject::tr("FastK Period"), settings.getInt(FastkPeriod), 1, 100000);

  dialog->addIntItem(SlowkPeriod, page, QObject::tr("Period"), settings.getInt(SlowkPeriod), 1, 100000);

  settings.getData(SlowkMA, d);
  dialog->addComboItem(SlowkMA, page, QObject::tr("MA Type"), maList, d);

  page++;
  k = QObject::tr("SlowD");
  dialog->addPage(page, k);

  settings.getData(SlowdColor, d);
  dialog->addColorItem(SlowdColor, page, QObject::tr("Color"), d);

  settings.getData(SlowdPlot, d);
  dialog->addComboItem(SlowdPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(SlowdLabel, d);
  dialog->addTextItem(SlowdLabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(SlowdPeriod, page, QObject::tr("Period"), settings.getInt(SlowdPeriod), 1, 100000);

  settings.getData(SlowdMA, d);
  dialog->addComboItem(SlowdMA, page, QObject::tr("MA Type"), maList, d);

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 1 Color"), d);

  settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 2 Color"), d);

  dialog->addIntItem(Ref1, page, QObject::tr("Ref. 1"), settings.getInt(Ref1), 0, 100);

  dialog->addIntItem(Ref2, page, QObject::tr("Ref. 2"), settings.getInt(Ref2), 0, 100);

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

