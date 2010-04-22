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

#include "MA.h"
#include "MAUtils.h"
#include "BARSUtils.h"

#include <QtDebug>
#include <cmath>

MA::MA ()
{
  indicator = "MA";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Input, "Close");
  settings.setData(Period, 14);
  settings.setData(Method, "SMA");
}

int MA::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);

  settings.getData(Method, s);
  QStringList methodList;
  MAUtils mau;
  mau.getMAList(methodList);
  int method = methodList.indexOf(s);

  PlotLine *line = mau.getMA(in, period, method);
  if (! line)
  {
    delete in;
    return 1;
  }

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  BARSUtils b;
  PlotLine *bars = b.getBARS(data, up, down, neutral);
  if (bars)
    ind.addLine(bars);

  settings.getData(Color, s);
  line->setColor(s);
  settings.getData(Plot, s);
  line->setPlugin(s);
  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  delete in;

  return 0;
}

int MA::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,MA,METHOD,<NAME>,<INPUT>,<PERIOD>

  if (set.count() != 7)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  MAUtils mau;
  QStringList methodList;
  mau.getMAList(methodList);
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

  PlotLine *in = tlines.value(set[5]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[5]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[5];
      return 1;
    }

    tlines.insert(set[5], in);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period" << set[6];
    return 1;
  }

  PlotLine *line = mau.getMA(in, period, method);
  if (! line)
    return 1;

  tlines.insert(set[4], line);

  return 0;
}

PlotLine * MA::getMA (PlotLine *in, int period, int method)
{
  PlotLine *line = 0;
  MAUtils mau;
  
  switch ((MAUtils::Method) method)
  {
    case MAUtils::EMA:
      line = mau.getEMA(in, period);
    case MAUtils::DEMA:
      line = mau.getDEMA(in, period);
    case MAUtils::KAMA:
      line = mau.getKAMA(in, period);
    case MAUtils::SMA:
      line = mau.getSMA(in, period);
    case MAUtils::TEMA:
      line = mau.getTEMA(in, period);
    case MAUtils::TRIMA:
      line = mau.getTRIMA(in, period);
    case MAUtils::Wilder:
      line = mau.getWilder(in, period);
    case MAUtils::WMA:
      line = mau.getWMA(in, period);
    default:
      break;
  }

  return line;
}

int MA::dialog (int)
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

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 2, 100000);

  QStringList methodList;
  MAUtils mau;
  mau.getMAList(methodList);
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
  MA *o = new MA;
  return ((IndicatorPlugin *) o);
}


