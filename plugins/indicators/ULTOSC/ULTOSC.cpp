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
#include "TR.h"
#include "PlotFactory.h"

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
  // 30 ref line
  QString s = "Horizontal";
  PlotFactory fac;
  PlotLine *line = fac.plot(s);
  if (! line)
    return 0;

  settings.getData(Ref1Color, s);
  QColor color(s);

  line->setData(0, new PlotLineBar(color, (double) 30));
  ind.addLine(line);

  // 50 ref line
  s = "Horizontal";
  line = fac.plot(s);
  if (! line)
    return 0;

  settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setData(0, new PlotLineBar(color, (double) 50));
  ind.addLine(line);

  // 70 ref line
  s = "Horizontal";
  line = fac.plot(s);
  if (! line)
    return 0;

  settings.getData(Ref3Color, s);
  color.setNamedColor(s);

  line->setData(0, new PlotLineBar(color, (double) 70));
  ind.addLine(line);

  // ultosc line
  int sp = settings.getInt(ShortPeriod);
  int mp = settings.getInt(MidPeriod);
  int lp = settings.getInt(LongPeriod);

  settings.getData(Color, s);
  color.setNamedColor(s);

  settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  line = getULTOSC(data, sp, mp, lp, lineType, color);
  if (! line)
    return 1;

  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  return 0;
}

int ULTOSC::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,ULTOSC,<NAME>,<SHORT PERIOD>,<MED PERIOD>,<LONG PERIOD>,<PLOT TYPE>,<COLOR>

  if (set.count() != 7)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int sp = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid short period" << set[4];
    return 1;
  }

  int mp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid med period" << set[5];
    return 1;
  }

  int lp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid long period" << set[6];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[7]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid plot type" << set[7];
    return 1;
  }

  QColor color(set[8]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[8];
    return 1;
  }

  PlotLine *line = getULTOSC(data, sp, mp, lp, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * ULTOSC::getULTOSC (BarData *data, int sp, int mp, int lp, int lineType, QColor &color)
{
  if (data->count() <= sp || data->count() <= mp || data->count() <= lp)
    return 0;

  PlotLine *bp = getBP(data);

  TR ttr;
  PlotLine *tr = ttr.tr(data, lineType, color);

  QList<int> keys;
  bp->keys(keys);

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
  {
    delete bp;
    delete tr;
    return 0;
  }

  int loop = lp - 1;
  for (; loop < keys.count(); loop++)
  {
    double bpsum1 = 0;
    double trsum1 = 0;
    int count = 0;
    for (; count < sp; count++)
    {
      PlotLineBar *bbar = bp->data(keys.at(loop - count));
      PlotLineBar *tbar = tr->data(keys.at(loop - count));
      bpsum1 += bbar->data();
      trsum1 += tbar->data();
    }

    double bpsum2 = 0;
    double trsum2 = 0;
    for (count = 0; count < mp; count++)
    {
      PlotLineBar *bbar = bp->data(keys.at(loop - count));
      PlotLineBar *tbar = tr->data(keys.at(loop - count));
      bpsum2 += bbar->data();
      trsum2 += tbar->data();
    }

    double bpsum3 = 0;
    double trsum3 = 0;
    for (count = 0; count < lp; count++)
    {
      PlotLineBar *bbar = bp->data(keys.at(loop - count));
      PlotLineBar *tbar = tr->data(keys.at(loop - count));
      bpsum3 += bbar->data();
      trsum3 += tbar->data();
    }

    double raw = (4 * (bpsum1 / trsum1)) + (2 * (bpsum2 / trsum2)) + (bpsum3 / trsum3);
    
    line->setData(keys.at(loop), new PlotLineBar(color, (raw / 7) * 100));
  }

  delete bp;
  delete tr;
  return line;
}

PlotLine * ULTOSC::getBP (BarData *data)
{
  PlotLine *line = new PlotLine;
  int size = data->count();
  int loop = 1;
  for (; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    Bar *pbar = data->getBar(loop - 1);
    double tl = bar->getLow();
    if (pbar->getLow() < tl)
      tl = pbar->getLow();
    
    line->setData(loop, new PlotLineBar(bar->getClose() - tl));
  }

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

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

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

