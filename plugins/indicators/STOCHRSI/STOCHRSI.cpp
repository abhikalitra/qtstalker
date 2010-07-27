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

#include "STOCHRSI.h"
#include "FunctionMA.h"
#include "FunctionSTOCHRSI.h"
#include "PlotStyleFactory.h"

#include <QtDebug>

STOCHRSI::STOCHRSI ()
{
  _indicator = "STOCHRSI";

  _settings.setData(Color, "red");
  _settings.setData(Ref1Color, "white");
  _settings.setData(Ref2Color, "white");
  _settings.setData(Plot, "Line");
  _settings.setData(Label, "STOCHRSI");
  _settings.setData(Ref1, 0.2);
  _settings.setData(Ref2, 0.8);
  _settings.setData(Input, "Close");
  _settings.setData(Period, 14);
}

int STOCHRSI::getIndicator (Indicator &ind, BarData &data)
{
  // create first ref line
  QString s = "Horizontal";
  PlotLine *line = new PlotLine;
  line->setType(s);

  _settings.getData(Ref1Color, s);
  QColor color(s);

  line->setData(0, (double) _settings.getInt(Ref1), color);
  
  s = "0";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  // create second ref line
  s = "Horizontal";
  line = new PlotLine;
  line->setType(s);

  _settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setData(0, (double) _settings.getInt(Ref2), color);
  
  s = "1";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  _settings.getData(Input, s);
  PlotLine *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = _settings.getInt(Period);

  FunctionSTOCHRSI f;
  line = f.calculate(in, period);
  if (! line)
  {
    delete in;
    return 1;
  }
  
  _settings.getData(Plot, s);
  line->setType(s);

  _settings.getData(Color, s);
  line->setColor(s);

  _settings.getData(Label, s);
  line->setLabel(s);
  
  s = "2";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  delete in;

  return 0;
}

int STOCHRSI::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionSTOCHRSI f;
  return f.script(set, ind, data);
}

int STOCHRSI::dialog (int)
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

  dialog->addIntItem(Period, page, QObject::tr("Period"), _settings.getInt(Period), 2, 100000);

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  _settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  _settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 1 Color"), d);

  _settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 2 Color"), d);

  dialog->addDoubleItem(Ref1, page, QObject::tr("Ref. 1"), _settings.getDouble(Ref1), 0.0, 1.0);

  dialog->addDoubleItem(Ref2, page, QObject::tr("Ref. 2"), _settings.getDouble(Ref2), 0.0, 1.0);

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
  STOCHRSI *o = new STOCHRSI;
  return ((IndicatorPlugin *) o);
}


