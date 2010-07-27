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

#include "CORREL.h"
#include "PlotStyleFactory.h"
#include "QuoteServerRequest.h"
#include "FunctionCORREL.h"

#include <QtDebug>

CORREL::CORREL ()
{
  _indicator = "CORREL";

  _settings.setData(Color, "red");
  _settings.setData(Plot, "Line");
  _settings.setData(Label, _indicator);
  _settings.setData(Input, "Close");
  _settings.setData(Input2, "SP500");
  _settings.setData(Exchange, "XNYS");
  _settings.setData(Period, 30);
  _settings.setData(Ref1Color, "white");
  _settings.setData(Ref2Color, "white");
  _settings.setData(Ref3Color, "white");
}

int CORREL::getIndicator (Indicator &ind, BarData &data)
{
  if (! data.count())
    return 1;

  QString s;
  _settings.getData(Input, s);
  PlotLine *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  BarData bd;
  _settings.getData(Input2, s);
  bd.setSymbol(s);
  _settings.getData(Exchange, s);
  bd.setExchange(s);
  bd.setBarLength(data.getBarLength());

  QStringList l;
  l << "GetQuotes" << bd.getExchange() << bd.getSymbol();
  bd.barLengthText(bd.getBarLength(), s);
  l << s;
  Bar tbar = data.getBar(0);
  l << tbar.date().toString("yyyyMMddHHmmss");
  tbar = data.getBar(data.count() - 1);
  l << tbar.date().toString("yyyyMMddHHmmss");

  QString command = l.join(",") + "\n";

  QuoteServerRequest qsr;
  if (qsr.run(command))
    return 1;

  bd.setBars(qsr.data());

  PlotLine *in2 = bd.getInput(BarData::Close);
  if (! in2)
  {
    qDebug() << _indicator << "::getIndicator: input 2 not found";
    delete in;
    return 1;
  }

  int period = _settings.getInt(Period);

  FunctionCORREL f;
  PlotLine *line = f.calculate(in, in2, period);
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

  // 1 reference line
  s = "Horizontal";
  PlotLine *hline = new PlotLine;
  hline->setType(s);

  _settings.getData(Ref3Color, s);
  QColor color(s);

  hline->setData(0, 1.0, color);

  s = "0";
  ind.setLine(s, hline);
  ind.addPlotOrder(s);

  // 0 reference line
  s = "Horizontal";
  hline = new PlotLine;
  hline->setType(s);

  _settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  hline->setData(0, 0, color);

  s = "1";
  ind.setLine(s, hline);
  ind.addPlotOrder(s);

  // -1 reference line
  s = "Horizontal";
  hline->setType(s);

  _settings.getData(Ref1Color, s);
  color.setNamedColor(s);

  hline->setData(0, -1.0, color);

  s = "2";
  ind.setLine(s, hline);
  ind.addPlotOrder(s);

  s = "3";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  delete in;
  delete in2;

  return 0;
}

int CORREL::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionCORREL f;
  return f.script(set, ind, data);
}

int CORREL::dialog (int)
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
  dialog->addTextItem(Input2, page, QObject::tr("Input 2"), d,
                      QObject::tr("Index symbol used for comparison eg. SP500"));

  dialog->addIntItem(Period, page, QObject::tr("Period"), _settings.getInt(Period), 1, 100000);

  page++;
  k = QObject::tr("Ref.");
  dialog->addPage(page, k);

  _settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 1 Color"), d);

  _settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 2 Color"), d);

  _settings.getData(Ref3Color, d);
  dialog->addColorItem(Ref3Color, page, QObject::tr("Ref. 3 Color"), d);

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
  CORREL *o = new CORREL;
  return ((IndicatorPlugin *) o);
}


