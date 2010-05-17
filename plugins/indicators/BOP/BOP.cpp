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

#include "BOP.h"
#include "MAFactory.h"
#include "PlotFactory.h"

#include <QtDebug>


BOP::BOP ()
{
  indicator = "BOP";

  settings.setData(Color, "red");
  settings.setData(Plot, "Histogram Bar");
  settings.setData(Label, indicator);
  settings.setData(Smoothing, 10);
  settings.setData(SmoothingType, "SMA");
}

int BOP::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  int smoothing = settings.getInt(Smoothing);

  MAFactory mau;
  settings.getData(SmoothingType, s);
  int type = mau.typeFromString(s);

  settings.getData(Color, s);
  QColor color(s);

  settings.getData(Plot, s);
  PlotFactory fac;
  int lineType = fac.typeFromString(s);

  PlotLine *line = getBOP(data, smoothing, type, lineType, color);
  if (! line)
    return 1;

  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  return 0;
}

int BOP::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,BOP,<NAME>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>,<PLOT TYPE>,<COLOR>
  //     0       1     2    3             4                 5             6         7

  if (set.count() != 8)
  {
    qDebug() << indicator << "::getCUS: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int smoothing = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid smoothing period" << set[4];
    return 1;
  }

  MAFactory mau;
  int ma = mau.typeFromString(set[5]);
  if (ma == -1)
  {
    qDebug() << indicator << "::getCUS: invalid smoothing type" << set[5];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[6]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid plot type" << set[6];
    return 1;
  }

  QColor color(set[7]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[7];
    return 1;
  }

  PlotLine *line = getBOP(data, smoothing, ma, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * BOP::getBOP (BarData *data, int smoothing, int type, int lineType, QColor &color)
{
  if (! data->count())
    return 0;
  
  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  int size = data->count();
  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    double v = (bar->getClose() - bar->getOpen()) / (bar->getHigh() - bar->getLow());
    line->setData(loop, new PlotLineBar(color, v));
  }

  if (smoothing > 1)
  {
    MAFactory mau;
    PlotLine *ma = mau.ma(line, smoothing, type, lineType, color);
    delete line;
    line = ma;
  }

  return line;
}

int BOP::dialog (int)
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

  dialog->addIntItem(Smoothing, page, QObject::tr("Smoothing"), settings.getInt(Smoothing), 1, 100000);

  QStringList maList;
  MAFactory mau;
  mau.list(maList);

  settings.getData(SmoothingType, d);
  dialog->addComboItem(SmoothingType, page, QObject::tr("Smoothing Type"), maList, d);

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
  BOP *o = new BOP;
  return ((IndicatorPlugin *) o);
}


