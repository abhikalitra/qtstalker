/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(ShortPeriod, 7);
  settings.setData(MidPeriod, 14);
  settings.setData(LongPeriod, 28);
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
  settings.setData(Ref3Color, "white");
}

int ULTOSC::getIndicator (Indicator &ind, BarData *data)
{
  int sp = settings.getInt(ShortPeriod);
  int mp = settings.getInt(MidPeriod);
  int lp = settings.getInt(LongPeriod);

  PlotLine *line = getULTOSC(data, sp, mp, lp);
  if (! line)
    return 1;

  QString s;
  PlotLine *ref30 = new PlotLine;
  s = "Horizontal";
  ref30->setPlugin(s);
  ref30->append(30);
  settings.getData(Ref1Color, s);
  ref30->setColor(s);
  ind.addLine(ref30);

  PlotLine *ref50 = new PlotLine;
  s = "Horizontal";
  ref50->setPlugin(s);
  ref50->append(50);
  settings.getData(Ref2Color, s);
  ref50->setColor(s);
  ind.addLine(ref50);

  PlotLine *ref70 = new PlotLine;
  s = "Horizontal";
  ref70->setPlugin(s);
  ref70->append(70);
  settings.getData(Ref3Color, s);
  ref70->setColor(s);
  ind.addLine(ref70);

  settings.getData(Color, s);
  line->setColor(s);
  settings.getData(Plot, s);
  line->setPlugin(s);
  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  return 0;
}

int ULTOSC::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,ULTOSC,<NAME>,<SHORT PERIOD>,<MED PERIOD>,<LONG PERIOD>

  if (set.count() != 5)
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

  bool ok;
  int sp = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid short period" << set[4];
    return 1;
  }

  int mp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid med period" << set[5];
    return 1;
  }

  int lp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid long period" << set[6];
    return 1;
  }

  PlotLine *line = getULTOSC(data, sp, mp, lp);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

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
    Bar *bar = data->getBar(loop);
    high[loop] = (TA_Real) bar->getHigh();
    low[loop] = (TA_Real) bar->getLow();
    close[loop] = (TA_Real) bar->getClose();
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

int ULTOSC::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  dialog->addIntItem(ShortPeriod, page, QObject::tr("Short Period"), settings.getInt(ShortPeriod), 1, 100000);

  dialog->addIntItem(MidPeriod, page, QObject::tr("Mid Period"), settings.getInt(MidPeriod), 1, 100000);

  dialog->addIntItem(LongPeriod, page, QObject::tr("Long Period"), settings.getInt(LongPeriod), 1, 100000);

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 30 Color"), d);

  settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 50 Color"), d);

  settings.getData(Ref3Color, d);
  dialog->addColorItem(Ref3Color, page, QObject::tr("Ref. 70 Color"), d);

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

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  ULTOSC *o = new ULTOSC;
  return ((IndicatorPlugin *) o);
}

