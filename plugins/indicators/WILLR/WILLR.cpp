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

#include "WILLR.h"
#include "PlotFactory.h"

#include <QtDebug>


WILLR::WILLR ()
{
  indicator = "WILLR";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Period, 14);
}

int WILLR::getIndicator (Indicator &ind, BarData *data)
{
  int period = settings.getInt(Period);

  PlotLine *line = getWILLR(data, period);
  if (! line)
    return 1;

  QString s;
  settings.getData(Color, s);
  line->setColor(s);
  settings.getData(Plot, s);
  line->setPlugin(s);
  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  return 0;
}

int WILLR::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,WILLR,<NAME>,<PERIOD>

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
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period settings" << set[4];
    return 1;
  }

  PlotLine *line = getWILLR(data, period);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * WILLR::getWILLR (BarData *data, int period)
{
  if (data->count() < period)
    return 0;
  
  PlotLine *line = new PlotLine;
  int size = data->count();
  int loop = period - 1;
  for (; loop < size; loop++)
  {
    int count = 0;
    double high = -99999999;
    double low = 99999999;
    for (; count < period; count++)
    {
      Bar *bar = data->getBar(loop - count);
      if (bar->getHigh() > high)
	high = bar->getHigh();
      if (bar->getLow() < low)
	low = bar->getLow();
    }
    
    Bar *bar = data->getBar(loop);
    double r = ((high - bar->getClose()) / (high - low)) * -100;
    line->append(r);
  }
  
  return line;
}

int WILLR::dialog (int)
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

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 1, 100000);

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
  WILLR *o = new WILLR;
  return ((IndicatorPlugin *) o);
}

