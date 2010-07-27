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

#include "MAVP.h"
#include "FunctionMAVP.h"
#include "FunctionMA.h"
#include "BARSUtils.h"
#include "PlotStyleFactory.h"

#include <QtDebug>

MAVP::MAVP ()
{
  _indicator = "MAVP";

  _settings.setData(Color, "red");
  _settings.setData(Plot, "Line");
  _settings.setData(Label, _indicator);
  _settings.setData(Input, "Close");
  _settings.setData(Input2, "Close");
  _settings.setData(Min, 2);
  _settings.setData(Max, 30);
  _settings.setData(MAType, "SMA");
}

int MAVP::getIndicator (Indicator &ind, BarData &data)
{
  QString s;
  _settings.getData(Input, s);
  PlotLine *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  _settings.getData(Input2, s);
  PlotLine *in2 = data.getInput(data.getInputType(s));
  if (! in2)
  {
    delete in;
    qDebug() << _indicator << "::getIndicator: input 2 not found" << s;
    return 1;
  }

  int min = _settings.getInt(Min);
  int max = _settings.getInt(Max);

  FunctionMA mau;
  _settings.getData(MAType, s);
  int ma = mau.typeFromString(s);

  FunctionMAVP f;
  PlotLine *line = f.calculate(in, in2, min, max, ma);
  if (! line)
  {
    delete in;
    delete in2;
    return 1;
  }

  _settings.getData(Plot, s);
  line->setType(s);

  _settings.getData(Color, s);
  line->setColor(s);

  _settings.getData(Label, s);
  line->setLabel(s);

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  BARSUtils b;
  PlotLine *bars = b.getBARS(data, up, down, neutral);
  if (bars)
  {
    s = "0";
    ind.setLine(s, bars);
    ind.addPlotOrder(s);
  }

  s = "1";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  delete in;
  delete in2;

  return 0;
}

int MAVP::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionMAVP f;
  return f.script(set, ind, data);
}

int MAVP::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  _settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  PlotStyleFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d, QString());

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  _settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  _settings.getData(Input2, d);
  dialog->addComboItem(Input2, page, QObject::tr("Input 2"), inputList, d);

  dialog->addIntItem(Min, page, QObject::tr("Min"), _settings.getInt(Min), 2, 100000);

  dialog->addIntItem(Max, page, QObject::tr("Max"), _settings.getInt(Max), 2, 100000);

  FunctionMA mau;
  QStringList maList = mau.list();

  _settings.getData(MAType, d);
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
  MAVP *o = new MAVP;
  return ((IndicatorPlugin *) o);
}


