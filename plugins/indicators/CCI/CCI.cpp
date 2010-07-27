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

#include "CCI.h"
#include "FunctionMA.h"
#include "PlotStyleFactory.h"
#include "FunctionCCI.h"

#include <QtDebug>
#include <cmath>

CCI::CCI ()
{
  _indicator = "CCI";

  _settings.setData(Color, "red");
  _settings.setData(Plot, "Line");
  _settings.setData(Label, _indicator);
  _settings.setData(Smoothing, 10);
  _settings.setData(SmoothingType, "SMA");
  _settings.setData(Period, 20);
  _settings.setData(Ref1, 100);
  _settings.setData(Ref2, -100);
  _settings.setData(Ref1Color, "white");
  _settings.setData(Ref2Color, "white");
}

int CCI::getIndicator (Indicator &ind, BarData &data)
{
  // create the ref1 line
  QString s = "Horizontal";
  PlotLine *rline = new PlotLine;
  rline->setType(s);

  _settings.getData(Ref1Color, s);
  QColor color(s);
  int ref = _settings.getInt(Ref1);
  rline->setData(0, (double) ref, color);
  
  s = "0";
  ind.setLine(s, rline);
  ind.addPlotOrder(s);

  // create the ref2 line
  s = "Horizontal";
  rline = new PlotLine;
  rline->setType(s);

  _settings.getData(Ref2Color, s);
  color.setNamedColor(s);
  ref = _settings.getInt(Ref2);
  rline->setData(0, (double) ref, color);

  s = "1";
  ind.setLine(s, rline);
  ind.addPlotOrder(s);

  // get the CCI plot
  int period = _settings.getInt(Period);
  int smoothing = _settings.getInt(Smoothing);

  FunctionMA mau;
  _settings.getData(SmoothingType, s);
  int type = mau.typeFromString(s);

  FunctionCCI f;
  PlotLine *line = f.calculate(data, period, smoothing, type);
  if (! line)
    return 1;

  _settings.getData(Plot, s);
  line->setType(s);

  _settings.getData(Color, s);
  line->setColor(s);

  _settings.getData(Label, s);
  line->setLabel(s);

  s = "2";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  return 0;
}

int CCI::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionCCI f;
  return f.script(set, ind, data);
}

int CCI::dialog (int)
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

  dialog->addIntItem(Smoothing, page, QObject::tr("Smoothing"), _settings.getInt(Smoothing), 1, 100000);

  FunctionMA mau;
  QStringList maList = mau.list();

  _settings.getData(SmoothingType, d);
  dialog->addComboItem(SmoothingType, page, QObject::tr("Smoothing Type"), maList, d);

  page++;
  k = QObject::tr("Ref Lines");
  dialog->addPage(page, k);

  _settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref 1 Color"), d);

  _settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref 2 Color"), d);

  dialog->addIntItem(Ref1, page, QObject::tr("Ref 1"), _settings.getInt(Ref1), -100000, 100000);

  dialog->addIntItem(Ref2, page, QObject::tr("Ref 2"), _settings.getInt(Ref2), -100000, 100000);

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
  CCI *o = new CCI;
  return ((IndicatorPlugin *) o);
}


