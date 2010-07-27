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

/* The "Market Thermometer" _indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

/* Alert Notes
1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days
*/

#include "THERM.h"
#include "FunctionMA.h"
#include "PlotStyleFactory.h"
#include "FunctionTHERM.h"

#include <QtDebug>
#include <cmath>

THERM::THERM ()
{
  _indicator = "THERM";

  _settings.setData(DownColor, "green");
  _settings.setData(UpColor, "magenta");
  _settings.setData(ThreshColor, "red");
  _settings.setData(MAColor, "yellow");
  _settings.setData(MAPlot, "Line");
  _settings.setData(Label, _indicator);
  _settings.setData(MALabel, "THERM_MA");
  _settings.setData(Threshold, 3);
  _settings.setData(Smoothing, 2);
  _settings.setData(MAPeriod, 22);
  _settings.setData(MAType, "SMA");
  _settings.setData(SmoothingType, "SMA");
}

int THERM::getIndicator (Indicator &ind, BarData &data)
{
  QString s;
  int smoothing = _settings.getInt(Smoothing);

  FunctionMA mau;
  _settings.getData(SmoothingType, s);
  int type = mau.typeFromString(s);

  FunctionTHERM f;
  PlotLine *line = f.calculate(data, smoothing, type);
  if (! line)
    return 1;

  s = "Histogram Bar";
  line->setType(s);

  _settings.getData(MAColor, s);
  line->setColor(s);

  _settings.getData(Label, s);
  line->setLabel(s);

  // therm ma
  int maPeriod = _settings.getInt(MAPeriod);

  _settings.getData(MAType, s);
  int maType = mau.typeFromString(s);

  PlotLine *ma = mau.calculate(line, maPeriod, maType);
  if (! ma)
  {
    delete line;
    return 1;
  }

  _settings.getData(MAPlot, s);
  ma->setType(s);

  _settings.getData(MAColor, s);
  ma->setColor(s);

  _settings.getData(MALabel, s);
  ma->setLabel(s);

  // assign therm colors
  double threshold = _settings.getDouble(Threshold);

  _settings.getData(ThreshColor, s);
  QColor threshColor(s);
  _settings.getData(UpColor, s);
  QColor upColor(s);
  _settings.getData(DownColor, s);
  QColor downColor(s);

  QList<int> keys;
  line->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *lbar = line->data(keys.at(loop));
    PlotLineBar *mbar = ma->data(keys.at(loop));
    
    if (! mbar)
      lbar->setColor(downColor);
    else
    {
      double thrm = lbar->data();
      double thrmma = mbar->data();

      if (thrm > (thrmma * threshold))
        lbar->setColor(threshColor);
      else
      {
        if (thrm > thrmma)
          lbar->setColor(upColor);
        else
          lbar->setColor(downColor);
      }
    }
  }

  s = "0";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  s = "1";
  ind.setLine(s, ma);
  ind.addPlotOrder(s);

  return 0;
}

int THERM::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionTHERM f;
  return f.script(set, ind, data);
}

int THERM::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  _settings.getData(UpColor, d);
  dialog->addColorItem(UpColor, page, QObject::tr("Up Color"), d);

  _settings.getData(DownColor, d);
  dialog->addColorItem(DownColor, page, QObject::tr("Down Color"), d);

  _settings.getData(ThreshColor, d);
  dialog->addColorItem(ThreshColor, page, QObject::tr("Threshold Color"), d);

  _settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d, QString());

  dialog->addDoubleItem(Threshold, page, QObject::tr("Threshold"), _settings.getDouble(Threshold), 0, 100000);

  dialog->addIntItem(Smoothing, page, QObject::tr("Smoothing"), _settings.getInt(Smoothing), 1, 100000);

  FunctionMA mau;
  QStringList maList = mau.list();
  
  _settings.getData(SmoothingType, d);
  dialog->addComboItem(SmoothingType, page, QObject::tr("Smoothing Type"), maList, d);

  page++;
  k = QObject::tr("MA");
  dialog->addPage(page, k);

  _settings.getData(MAColor, d);
  dialog->addColorItem(MAColor, page, QObject::tr("Color"), d);

  PlotStyleFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(MAPlot, d);
  dialog->addComboItem(MAPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MALabel, d);
  dialog->addTextItem(MALabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(MAPeriod, page, QObject::tr("Period"), _settings.getInt(MAPeriod), 1, 100000);

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
  THERM *o = new THERM;
  return ((IndicatorPlugin *) o);
}

