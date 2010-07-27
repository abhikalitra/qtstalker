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

#include "AROON.h"
#include "PlotStyleFactory.h"
#include "FunctionAROON.h"

#include <QtDebug>

AROON::AROON ()
{
  _indicator = "AROON";
  _settings.setData(Method, "AROON");
  _settings.setData(DownColor, "red");
  _settings.setData(UpColor, "green");
  _settings.setData(DownPlot, "Line");
  _settings.setData(UpPlot, "Line");
  _settings.setData(DownLabel, "AROOND");
  _settings.setData(UpLabel, "AROONU");
  _settings.setData(OSCColor, "red");
  _settings.setData(OSCPlot, "Histogram Bar");
  _settings.setData(OSCLabel, "AROONOSC");
  _settings.setData(Period, 14);
}

int AROON::getIndicator (Indicator &ind, BarData &data)
{
  FunctionAROON f;
  QStringList methodList = f.list();
  
  QString s;
  _settings.getData(Method, s);
  int method = methodList.indexOf(s);

  int period = _settings.getInt(Period);

  switch ((FunctionAROON::Method) method)
  {
    case FunctionAROON::_AROONOSC:
    {
      PlotLine *line = f.getAROONOSC(data, period);
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
      QList<PlotLine *> pl;
      if (f.getAROON(data, period, pl))
        return 1;

      PlotLine *line = pl.at(0);

      _settings.getData(UpPlot, s);
      line->setType(s);

      _settings.getData(UpColor, s);
      line->setColor(s);

      _settings.getData(UpLabel, s);
      line->setLabel(s);
      
      s = "1";
      ind.setLine(s, line);
      ind.addPlotOrder(s);

      line = pl.at(1);

      _settings.getData(DownPlot, s);
      line->setType(s);

      _settings.getData(DownColor, s);
      line->setColor(s);

      _settings.getData(DownLabel, s);
      line->setLabel(s);
      
      s = "2";
      ind.setLine(s, line);
      ind.addPlotOrder(s);
      break;
    }
  }

  return 0;
}

int AROON::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionAROON f;
  return f.script(set, ind, data);
}

int AROON::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("General");
  dialog->addPage(page, k);

  dialog->addIntItem(Period, page, QObject::tr("Period"), _settings.getInt(Period), 2, 100000);

  FunctionAROON f;
  QStringList methodList = f.list();
  
  _settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), methodList, d);

  page++;
  k = QObject::tr("Aroon");
  dialog->addPage(page, k);

  _settings.getData(UpColor, d);
  dialog->addColorItem(UpColor, page, QObject::tr("Up Color"), d);

  _settings.getData(DownColor, d);
  dialog->addColorItem(DownColor, page, QObject::tr("Down Color"), d);

  PlotStyleFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(UpPlot, d);
  dialog->addComboItem(UpPlot, page, QObject::tr("Up Plot"), plotList, d);

  _settings.getData(DownPlot, d);
  dialog->addComboItem(DownPlot, page, QObject::tr("Down Plot"), plotList, d);

  _settings.getData(UpLabel, d);
  dialog->addTextItem(UpLabel, page, QObject::tr("Up Label"), d, QString());

  _settings.getData(DownLabel, d);
  dialog->addTextItem(DownLabel, page, QObject::tr("Down Label"), d, QString());

  page++;
  k = QObject::tr("OSC");
  dialog->addPage(page, k);

  _settings.getData(OSCColor, d);
  dialog->addColorItem(OSCColor, page, QObject::tr("Color"), d);

  _settings.getData(OSCPlot, d);
  dialog->addComboItem(OSCPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(OSCLabel, d);
  dialog->addTextItem(OSCLabel, page, QObject::tr("Label"), d, QString());

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
  AROON *o = new AROON;
  return ((IndicatorPlugin *) o);
}


