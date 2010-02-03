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

#include "ULTOSC.h"
#include "ta_libc.h"

#include <QtDebug>


ULTOSC::ULTOSC ()
{
  indicator = "ULTOSC";
  spKey = QObject::tr("Short Period");
  mpKey = QObject::tr("Medium Period");
  lpKey = QObject::tr("Long Period");
  ref30ColorKey = QObject::tr("Ref. 30 Color");
  ref50ColorKey = QObject::tr("Ref. 50 Color");
  ref70ColorKey = QObject::tr("Ref. 70 Color");

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "white";
  settings.setData(ref30ColorKey, d);
  settings.setData(ref50ColorKey, d);
  settings.setData(ref70ColorKey, d);

  d = "Line";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);

  settings.setData(spKey, 7);
  settings.setData(mpKey, 14);
  settings.setData(lpKey, 28);
}

int ULTOSC::getIndicator (Indicator &ind, BarData *data)
{
  int sp = settings.getInt(spKey);
  int mp = settings.getInt(mpKey);
  int lp = settings.getInt(lpKey);

  PlotLine *line = getULTOSC(data, sp, mp, lp);
  if (! line)
    return 1;

  QString s;
  PlotLine *ref30 = new PlotLine;
  ref30->setType(PlotLine::Horizontal);
  settings.getData(ref30ColorKey, s);
  ref30->setColor(s);
  ref30->append(30);
  ind.addLine(ref30);

  PlotLine *ref50 = new PlotLine;
  ref50->setType(PlotLine::Horizontal);
  settings.getData(ref50ColorKey, s);
  ref50->setColor(s);
  ref50->append(50);
  ind.addLine(ref50);

  PlotLine *ref70 = new PlotLine;
  ref70->setType(PlotLine::Horizontal);
  settings.getData(ref70ColorKey, s);
  ref70->setColor(s);
  ref70->append(70);
  ind.addLine(ref70);

  settings.getData(colorKey, s);
  line->setColor(s);

  settings.getData(plotKey, s);
  line->setType(s);

  settings.getData(labelKey, s);
  line->setLabel(s);

  ind.addLine(line);

  return 0;
}

int ULTOSC::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,ULTOSC,<NAME>,<SHORT PERIOD>,<MED PERIOD>,<LONG PERIOD>

  if (set.count() != 4)
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
  int sp = set[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid short period" << set[3];
    return 1;
  }

  int mp = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid med period" << set[4];
    return 1;
  }

  int lp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid long period" << set[5];
    return 1;
  }

  PlotLine *line = getULTOSC(data, sp, mp, lp);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * ULTOSC::getULTOSC (BarData *data, int sp, int mp, int lp)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_ULTOSC(0, size - 1, &high[0], &low[0], &close[0], sp, mp, lp, &outBeg, &outNb, &out[0]);
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

int ULTOSC::dialog ()
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

  dialog->addIntItem(page, spKey, settings.getInt(spKey), 1, 100000);

  dialog->addIntItem(page, mpKey, settings.getInt(mpKey), 1, 100000);

  dialog->addIntItem(page, lpKey, settings.getInt(lpKey), 1, 100000);

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  settings.getData(ref30ColorKey, d);
  dialog->addColorItem(page, ref30ColorKey, d);

  settings.getData(ref50ColorKey, d);
  dialog->addColorItem(page, ref50ColorKey, d);

  settings.getData(ref70ColorKey, d);
  dialog->addColorItem(page, ref70ColorKey, d);

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

