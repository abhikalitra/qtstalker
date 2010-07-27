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

#include "STOCH.h"
#include "FunctionMA.h"
#include "FunctionSTOCH.h"
#include "PlotStyleFactory.h"

#include <QtDebug>

STOCH::STOCH ()
{
  _indicator = "STOCH";

  _settings.setData(FastKColor, "red");
  _settings.setData(FastDColor, "yellow");
  _settings.setData(Ref1Color, "white");
  _settings.setData(Ref2Color, "white");
  _settings.setData(FastKPlot, "Line");
  _settings.setData(FastDPlot, "Dash");
  _settings.setData(FastKLabel, "FASTK");
  _settings.setData(FastDLabel, "FASTD");
  _settings.setData(FastKPeriod, 5);
  _settings.setData(FastDPeriod, 3);
  _settings.setData(FastDMA, "SMA");
  _settings.setData(Ref1, 20);
  _settings.setData(Ref2, 80);
}

int STOCH::getIndicator (Indicator &ind, BarData &data)
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

  // create the fastk line
  int kperiod = _settings.getInt(FastKPeriod);
  int dperiod = _settings.getInt(FastDPeriod);

  FunctionMA mau;
  _settings.getData(FastDMA, s);
  int maType = mau.typeFromString(s);

  FunctionSTOCH f;
  QList<PlotLine *> pl;
  if (f.calculate(data, kperiod, dperiod, maType, pl))
    return 1;

  line = pl.at(0);

  _settings.getData(FastKPlot, s);
  line->setType(s);

  _settings.getData(FastKColor, s);
  line->setColor(s);

  _settings.getData(FastKLabel, s);
  line->setLabel(s);
  
  s = "2";
  ind.setLine(s, line);
  ind.addPlotOrder(s);
  
  line = pl.at(1);

  _settings.getData(FastDPlot, s);
  line->setType(s);

  _settings.getData(FastDColor, s);
  line->setColor(s);

  _settings.getData(FastDLabel, s);
  line->setLabel(s);
  
  s = "3";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  return 0;
}

int STOCH::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionSTOCH f;
  return f.script(set, ind, data);
}

int STOCH::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("FastK");
  dialog->addPage(page, k);

  _settings.getData(FastKColor, d);
  dialog->addColorItem(FastKColor, page, QObject::tr("Color"), d);

  PlotStyleFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(FastKPlot, d);
  dialog->addComboItem(FastKPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(FastKLabel, d);
  dialog->addTextItem(FastKLabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(FastKPeriod, page, QObject::tr("Period"), _settings.getInt(FastKPeriod), 1, 100000);

  page++;
  k = QObject::tr("FastD");
  dialog->addPage(page, k);

  _settings.getData(FastDColor, d);
  dialog->addColorItem(FastDColor, page, QObject::tr("Color"), d);

  _settings.getData(FastDPlot, d);
  dialog->addComboItem(FastDPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(FastDLabel, d);
  dialog->addTextItem(FastDLabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(FastDPeriod, page, QObject::tr("Period"), _settings.getInt(FastDPeriod), 1, 100000);

  FunctionMA mau;
  QStringList maList = mau.list();

  _settings.getData(FastDMA, d);
  dialog->addComboItem(FastDMA, page, QObject::tr("MA Type"), maList, d);

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  _settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 1 Color"), d);

  _settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 2 Color"), d);

  dialog->addIntItem(Ref1, page, QObject::tr("Ref. 1"), _settings.getInt(Ref1), 0, 100);

  dialog->addIntItem(Ref2, page, QObject::tr("Ref. 2"), _settings.getInt(Ref2), 0, 100);

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
  STOCH *o = new STOCH;
  return ((IndicatorPlugin *) o);
}


