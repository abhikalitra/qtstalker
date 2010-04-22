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

#include "ATR.h"
#include "TR.h"
#include "MAUtils.h"

#include <QtDebug>

ATR::ATR ()
{
  indicator = "ATR";

  settings.setData(Method, "ATR");
  settings.setData(Period, 14);
  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);

  methodList << "ATR" << "NATR";
}

int ATR::getIndicator (Indicator &ind, BarData *data)
{
  int period = settings.getInt(Period);

  QString s;
  settings.getData(Method, s);
  int method = methodList.indexOf(s);

  PlotLine *line = getLine(data, period, method);
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

int ATR::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,ATR,<METHOD>,<NAME>,<PERIOD>

  if (set.count() != 5)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[3];
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
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

  PlotLine *line = getLine(data, period, method);
  if (! line)
    return 1;

  tlines.insert(set[4], line);

  return 0;
}

PlotLine * ATR::getLine (BarData *data, int period, int method)
{
  PlotLine *line = 0;
  
  switch ((_Method) method)
  {
    case _ATR:
      line = getATR(data, period);
      break;
    case NATR:
      line = getNATR(data, period);
      break;
    default:
      break;
  }

  return line;
}

PlotLine * ATR::getATR (BarData *data, int period)
{
  if (data->count() < period)
    return 0;

  TR ttr;
  PlotLine *tr = ttr.getTR(data);
  if (! tr)
    return 0;

  MAUtils mau;
  PlotLine *atr = mau.getMA(tr, period, MAUtils::Wilder);

  delete tr;
  return atr;
}

PlotLine * ATR::getNATR (BarData *data, int period)
{
  if (data->count() < period)
    return 0;

  TR ttr;
  PlotLine *ntr = ttr.getNTR(data);
  if (! ntr)
    return 0;

  MAUtils mau;
  PlotLine *natr = mau.getMA(ntr, period, MAUtils::Wilder);

  delete ntr;
  
  return natr;
}

int ATR::dialog (int)
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

  settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), methodList, d);

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
  ATR *o = new ATR;
  return ((IndicatorPlugin *) o);
}


