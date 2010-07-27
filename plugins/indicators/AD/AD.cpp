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

#include "AD.h"
#include "PlotStyleFactory.h"
#include "FunctionAD.h"

#include <QtDebug>

AD::AD ()
{
  _indicator = "AD";

  _settings.setData(Method, QString("AD"));
  _settings.setData(ADColor, QString("red"));
  _settings.setData(ADPlot, QString("Line"));
  _settings.setData(ADLabel, QString("AD"));
  _settings.setData(FastPeriod, 3);
  _settings.setData(SlowPeriod, 10);
  _settings.setData(OSCColor, QString("red"));
  _settings.setData(OSCPlot, QString("Histogram Bar"));
  _settings.setData(OSCLabel, QString("ADOSC"));
}

int AD::getIndicator (Indicator &ind, BarData &data)
{
  FunctionAD f;
  QStringList methodList = f.list();
  
  QString s;
  _settings.getData(Method, s);
  int method = methodList.indexOf(s);

  switch ((FunctionAD::Method) method)
  {
    case FunctionAD::_ADOSC:
    {
      int fp = _settings.getInt(FastPeriod);
      int sp = _settings.getInt(SlowPeriod);

      PlotLine *line = f.getADOSC(data, fp, sp);
      if (! line)
	return 1;

      _settings.getData(OSCPlot, s);
      line->setType(s);
      
      _settings.getData(OSCColor, s);
      line->setColor(s);
      
      _settings.getData(OSCLabel, s);
      line->setLabel(s);

      s = "0";
      ind.setLine(s, line);
      ind.addPlotOrder(s);
      break;
    }
    default:
    {
      PlotLine *line = f.getAD(data);
      if (! line)
	return 1;
      
      _settings.getData(ADPlot, s);
      line->setType(s);

      _settings.getData(ADColor, s);
      line->setColor(s);

      _settings.getData(ADLabel, s);
      line->setLabel(s);

      s = "0";
      ind.setLine(s, line);
      ind.addPlotOrder(s);
      break;
    }
  }

  return 0;
}

int AD::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionAD f;
  return f.script(set, ind, data);
}

int AD::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("General");
  dialog->addPage(page, k);

  FunctionAD f;
  QStringList methodList = f.list();
  
  _settings.getData(Method, d);
  dialog->addComboItem((int) Method, page, QObject::tr("Method"), methodList, d);

  page++;
  k = QObject::tr("AD");
  dialog->addPage(page, k);

  _settings.getData(ADColor, d);
  dialog->addColorItem((int) ADColor, page, QObject::tr("Color"), d);

  PlotStyleFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);
  
  _settings.getData(ADPlot, d);
  dialog->addComboItem((int) ADPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(ADLabel, d);
  dialog->addTextItem((int) ADLabel, page, QObject::tr("Label"), d, QString());

  page++;
  k = QObject::tr("OSC");
  dialog->addPage(page, k);

  _settings.getData(OSCColor, d);
  dialog->addColorItem(OSCColor, page, QObject::tr("Color"), d);

  _settings.getData(OSCPlot, d);
  dialog->addComboItem(OSCPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(OSCLabel, d);
  dialog->addTextItem(OSCLabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(FastPeriod, page, QObject::tr("Fast Period"), _settings.getInt(FastPeriod), 1, 100000);

  dialog->addIntItem(SlowPeriod, page, QObject::tr("Slow Period"), _settings.getInt(SlowPeriod), 1, 100000);

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
  AD *o = new AD;
  return ((IndicatorPlugin *) o);
}

