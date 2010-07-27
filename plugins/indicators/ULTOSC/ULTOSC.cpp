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
#include "FunctionULTOSC.h"
#include "PlotStyleFactory.h"

#include <QtDebug>

ULTOSC::ULTOSC ()
{
  _indicator = "ULTOSC";

  _settings.setData(Color, "red");
  _settings.setData(Plot, "Line");
  _settings.setData(Label, _indicator);
  _settings.setData(ShortPeriod, 7);
  _settings.setData(MidPeriod, 14);
  _settings.setData(LongPeriod, 28);
  _settings.setData(Ref1Color, "white");
  _settings.setData(Ref2Color, "white");
  _settings.setData(Ref3Color, "white");
}

int ULTOSC::getIndicator (Indicator &ind, BarData &data)
{
  // 30 ref line
  QString s = "Horizontal";
  PlotLine *line = new PlotLine;
  line->setType(s);

  _settings.getData(Ref1Color, s);
  QColor color(s);

  line->setData(0, 30.0, color);
  
  s = "0";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  // 50 ref line
  s = "Horizontal";
  line = new PlotLine;
  line->setType(s);

  _settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setData(0, 50.0, color);
  
  s = "1";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  // 70 ref line
  s = "Horizontal";
  line = new PlotLine;
  line->setType(s);

  _settings.getData(Ref3Color, s);
  color.setNamedColor(s);

  line->setData(0, 70.0, color);
  
  s = "2";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  // ultosc line
  int sp = _settings.getInt(ShortPeriod);
  int mp = _settings.getInt(MidPeriod);
  int lp = _settings.getInt(LongPeriod);

  FunctionULTOSC f;
  line = f.calculate(data, sp, mp, lp);
  if (! line)
    return 1;

  _settings.getData(Plot, s);
  line->setType(s);

  _settings.getData(Color, s);
  line->setColor(s);

  _settings.getData(Label, s);
  line->setLabel(s);
  
  s = "3";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  return 0;
}

int ULTOSC::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionULTOSC f;
  return f.script(set, ind, data);
}

int ULTOSC::dialog (int)
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

  dialog->addIntItem(ShortPeriod, page, QObject::tr("Short Period"), _settings.getInt(ShortPeriod), 1, 100000);

  dialog->addIntItem(MidPeriod, page, QObject::tr("Mid Period"), _settings.getInt(MidPeriod), 1, 100000);

  dialog->addIntItem(LongPeriod, page, QObject::tr("Long Period"), _settings.getInt(LongPeriod), 1, 100000);

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  _settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 30 Color"), d);

  _settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 50 Color"), d);

  _settings.getData(Ref3Color, d);
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

