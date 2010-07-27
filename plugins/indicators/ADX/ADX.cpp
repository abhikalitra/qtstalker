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

#include "ADX.h"
#include "PlotStyleFactory.h"
#include "FunctionADX.h"

#include <QtDebug>

ADX::ADX ()
{
  _indicator = "ADX";

  _settings.setData(ADXColor, "blue");
  _settings.setData(ADXRColor, "yellow");
  _settings.setData(PDIColor, "green");
  _settings.setData(MDIColor, "red");
  _settings.setData(ADXPlot, "Line");
  _settings.setData(ADXRPlot, "Line");
  _settings.setData(PDIPlot, "Line");
  _settings.setData(MDIPlot, "Line");
  _settings.setData(ADXLabel, "ADX");
  _settings.setData(ADXRLabel, "ADXR");
  _settings.setData(PDILabel, "+DI");
  _settings.setData(MDILabel, "-DI");
  _settings.setData(ADXCheck, 1);
  _settings.setData(ADXRCheck, 1);
  _settings.setData(PDICheck, 1);
  _settings.setData(MDICheck, 1);
  _settings.setData(Period, 14);
}

int ADX::getIndicator (Indicator &ind, BarData &data)
{
  int period = _settings.getInt(Period);

  FunctionADX f;

  if (_settings.getInt(MDICheck))
  {
    PlotLine *line = f.calculate(data, period, (int) FunctionADX::_MDI);
    if (! line)
      return 1;

    QString s;
    _settings.getData(MDIPlot, s);
    line->setType(s);

    _settings.getData(MDIColor, s);
    line->setColor(s);

    _settings.getData(MDILabel, s);
    line->setLabel(s);

    s = "0";
    ind.setLine(s, line);
    ind.addPlotOrder(s);
  }

  if (_settings.getInt(PDICheck))
  {
    PlotLine *line = f.calculate(data, period, (int) FunctionADX::_PDI);
    if (! line)
      return 1;

    QString s;
    _settings.getData(PDIPlot, s);
    line->setType(s);

    _settings.getData(PDIColor, s);
    line->setColor(s);

    _settings.getData(PDILabel, s);
    line->setLabel(s);
    
    s = "1";
    ind.setLine(s, line);
    ind.addPlotOrder(s);
  }

  if (_settings.getInt(ADXCheck))
  {
    PlotLine *line = f.calculate(data, period, (int) FunctionADX::_ADX);
    if (! line)
      return 1;

    QString s;
    _settings.getData(ADXPlot, s);
    line->setType(s);

    _settings.getData(ADXColor, s);
    line->setColor(s);

    _settings.getData(ADXLabel, s);
    line->setLabel(s);
    
    s = "2";
    ind.setLine(s, line);
    ind.addPlotOrder(s);
  }

  if (_settings.getInt(ADXRCheck))
  {
    PlotLine *line = f.calculate(data, period, (int) FunctionADX::_ADXR);
    if (! line)
      return 1;

    QString s;
    _settings.getData(ADXRPlot, s);
    line->setType(s);

    _settings.getData(ADXRColor, s);
    line->setColor(s);

    _settings.getData(ADXRLabel, s);
    line->setLabel(s);
    
    s = "3";
    ind.setLine(s, line);
    ind.addPlotOrder(s);
  }

  return 0;
}

int ADX::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionADX f;
  return f.script(set, ind, data);
}

int ADX::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("General");
  dialog->addPage(page, k);

  dialog->addIntItem(Period, page, QObject::tr("Period"), _settings.getInt(Period), 2, 100000);

  page++;
  k = QObject::tr("-DI");
  dialog->addPage(page, k);

  _settings.getData(MDIColor, d);
  dialog->addColorItem(MDIColor, page, QObject::tr("Color"), d);

  PlotStyleFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(MDIPlot, d);
  dialog->addComboItem(MDIPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MDILabel, d);
  dialog->addTextItem(MDILabel, page, QObject::tr("Label"), d, QString());

  dialog->addCheckItem(MDICheck, page, QObject::tr("Show"), _settings.getInt(MDICheck));

  page++;
  k = QObject::tr("+DI");
  dialog->addPage(page, k);

  _settings.getData(PDIColor, d);
  dialog->addColorItem(PDIColor, page, QObject::tr("Color"), d);

  _settings.getData(PDIPlot, d);
  dialog->addComboItem(PDIPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(PDILabel, d);
  dialog->addTextItem(PDILabel, page, QObject::tr("Label"), d, QString());

  dialog->addCheckItem(PDICheck, page, QObject::tr("Show"), _settings.getInt(MDICheck));

  page++;
  k = QObject::tr("ADX");
  dialog->addPage(page, k);

  _settings.getData(ADXColor, d);
  dialog->addColorItem(ADXColor, page, QObject::tr("Color"), d);

  _settings.getData(ADXPlot, d);
  dialog->addComboItem(ADXPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(ADXLabel, d);
  dialog->addTextItem(ADXLabel, page, QObject::tr("Label"), d, QString());

  dialog->addCheckItem(ADXCheck, page, QObject::tr("Show"), _settings.getInt(MDICheck));

  page++;
  k = QObject::tr("ADXR");
  dialog->addPage(page, k);

  _settings.getData(ADXRColor, d);
  dialog->addColorItem(ADXRColor, page, QObject::tr("Color"), d);

  _settings.getData(ADXRPlot, d);
  dialog->addComboItem(ADXRPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(ADXRLabel, d);
  dialog->addTextItem(ADXRLabel, page, QObject::tr("Label"), d, QString());

  dialog->addCheckItem(ADXRCheck, page, QObject::tr("Show"), _settings.getInt(MDICheck));

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
  ADX *o = new ADX;
  return ((IndicatorPlugin *) o);
}


