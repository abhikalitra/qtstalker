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

#include "STOCHS.h"
#include "FunctionMA.h"
#include "FunctionSTOCHS.h"
#include "PlotStyleFactory.h"

#include <QtDebug>

STOCHS::STOCHS ()
{
  _indicator = "STOCHS";

  _settings.setData(SlowKColor, "red");
  _settings.setData(SlowDColor, "yellow");
  _settings.setData(Ref1Color, "white");
  _settings.setData(Ref2Color, "white");
  _settings.setData(SlowKPlot, "Line");
  _settings.setData(SlowDPlot, "Dash");
  _settings.setData(SlowKLabel, "SLOWK");
  _settings.setData(SlowDLabel, "SLOWD");
  _settings.setData(FastKPeriod, 5);
  _settings.setData(SlowKPeriod, 3);
  _settings.setData(SlowDPeriod, 3);
  _settings.setData(SlowKMA, "SMA");
  _settings.setData(SlowDMA, "SMA");
  _settings.setData(Ref1, 20);
  _settings.setData(Ref2, 80);
}

int STOCHS::getIndicator (Indicator &ind, BarData &data)
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

  int fkperiod = _settings.getInt(FastKPeriod);
  int skperiod = _settings.getInt(SlowKPeriod);

  FunctionMA mau;
  _settings.getData(SlowKMA, s);
  int kmaType = mau.typeFromString(s);

  int dperiod = _settings.getInt(SlowDPeriod);

  _settings.getData(SlowDMA, s);
  int dmaType = mau.typeFromString(s);

  FunctionSTOCHS f;
  QList<PlotLine *> pl;
  if (f.calculate(data, fkperiod, skperiod, dperiod, kmaType, dmaType, pl))
    return 1;

  line = pl.at(0);

  _settings.getData(SlowKPlot, s);
  line->setType(s);

  _settings.getData(SlowKColor, s);
  line->setColor(s);

  _settings.getData(SlowKLabel, s);
  line->setLabel(s);
  
  s = "2";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  line = pl.at(1);

  _settings.getData(SlowDPlot, s);
  line->setType(s);

  _settings.getData(SlowDColor, s);
  line->setColor(s);

  _settings.getData(SlowDLabel, s);
  line->setLabel(s);
  
  s = "3";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  return 0;
}

int STOCHS::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionSTOCHS f;
  return f.script(set, ind, data);
}

int STOCHS::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("SlowK");
  dialog->addPage(page, k);

  _settings.getData(SlowKColor, d);
  dialog->addColorItem(SlowKColor, page, QObject::tr("Color"), d);

  PlotStyleFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(SlowKPlot, d);
  dialog->addComboItem(SlowKPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(SlowKLabel, d);
  dialog->addTextItem(SlowKLabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(FastKPeriod, page, QObject::tr("FastK Period"), _settings.getInt(FastKPeriod), 1, 100000);

  dialog->addIntItem(SlowKPeriod, page, QObject::tr("Period"), _settings.getInt(SlowKPeriod), 1, 100000);

  FunctionMA mau;
  QStringList maList = mau.list();

  _settings.getData(SlowKMA, d);
  dialog->addComboItem(SlowKMA, page, QObject::tr("MA Type"), maList, d);

  page++;
  k = QObject::tr("SlowD");
  dialog->addPage(page, k);

  _settings.getData(SlowDColor, d);
  dialog->addColorItem(SlowDColor, page, QObject::tr("Color"), d);

  _settings.getData(SlowDPlot, d);
  dialog->addComboItem(SlowDPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(SlowDLabel, d);
  dialog->addTextItem(SlowDLabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(SlowDPeriod, page, QObject::tr("Period"), _settings.getInt(SlowDPeriod), 1, 100000);

  _settings.getData(SlowDMA, d);
  dialog->addComboItem(SlowDMA, page, QObject::tr("MA Type"), maList, d);

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
  STOCHS *o = new STOCHS;
  return ((IndicatorPlugin *) o);
}


