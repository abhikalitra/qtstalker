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

#include "FI.h"
#include "MATH1.h"

#include <QtDebug>


FI::FI ()
{
  indicator = "FI";

  settings.setData(Color, "red");
  settings.setData(Plot, "HistogramBar");
  settings.setData(Label, indicator);
  settings.setData(MAType, "EMA");
  settings.setData(Period, 2);
}

int FI::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  int period = settings.getInt(Period);

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  
  settings.getData(MAType, s);
  int ma = maList.indexOf(s);

  PlotLine *line = getFI(data, period, ma);
  if (! line)
    return 1;

  settings.getData(Color, s);
  line->setColor(s);
  settings.getData(Plot, s);
  line->setPlugin(s);
  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  return 0;
}

int FI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,FI,NAME,PERIOD,MA_TYPE

  if (set.count() != 6)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period" << set[4];
    return 1;
  }

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  int ma = maList.indexOf(set[5]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid ma" << set[5];
    return 1;
  }

  PlotLine *line = getFI(data, period, ma);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * FI::getFI (BarData *data, int period, int type)
{
  PlotLine *line = new PlotLine();
  int loop;
  double force = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    Bar *bar = data->getBar(loop);
    Bar *pbar = data->getBar(loop - 1);
    double cdiff = bar->getClose() - pbar->getClose();
    force = bar->getVolume() * cdiff;
    line->append(force);
  }

  if (period > 1)
  {
    MATH1 m;
    PlotLine *ma = m.getMA(line, period, type);
    if (! ma)
    {
      delete line;
      return 0;
    }

    delete line;
    line = ma;
  }

  return line;
}

int FI::dialog (int)
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

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 1, 100000);

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  
  settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("MA Type"), maList, d);

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
  FI *o = new FI;
  return ((IndicatorPlugin *) o);
}


