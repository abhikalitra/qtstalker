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

#include "CANDLES.h"
#include "FunctionMA.h"
#include "PlotStyleFactory.h"
#include "FunctionCANDLES.h"

#include <QtDebug>

CANDLES::CANDLES ()
{
  _indicator = "CANDLES";

  _settings.setData(Penetration, 0.3);
  _settings.setData(Method, "NONE");
  _settings.setData(MethodColor, "cyan");
  _settings.setData(Color, "green");
  _settings.setData(Label, _indicator);
  _settings.setData(MAColor, "red");
  _settings.setData(MA2Color, "yellow");
  _settings.setData(MA3Color, "blue");
  _settings.setData(MAPlot, "Line");
  _settings.setData(MA2Plot, "Line");
  _settings.setData(MA3Plot, "Line");
  _settings.setData(MALabel, "MA1");
  _settings.setData(MA2Label, "MA2");
  _settings.setData(MA3Label, "MA3");
  _settings.setData(MAPeriod, 20);
  _settings.setData(MA2Period, 50);
  _settings.setData(MA3Period, 200);
  _settings.setData(MAType, "SMA");
  _settings.setData(MA2Type, "SMA");
  _settings.setData(MA3Type, "SMA");
}

int CANDLES::getIndicator (Indicator &ind, BarData &data)
{
  FunctionCANDLES f;
  PlotLine *line = f.candles(data);
  if (! line)
    return 1;

  QString s;
  _settings.getData(Color, s);
  line->setColor(s);

  _settings.getData(Label, s);
  line->setLabel(s);

  QStringList methodList = f.list();
  _settings.getData(Method, s);
  int method = methodList.indexOf(s);
  if (method != FunctionCANDLES::_NONE)
  {
    double pen = _settings.getDouble(Penetration);

    PlotLine *line2 = f.getMethod(data, method, pen);
    if (line2)
    {
      _settings.getData(MethodColor, s);
      QColor color(s);
      
      int loop = 0;
      for (; loop < line2->count(); loop++)
      {
        PlotLineBar *bar2 = line2->data(loop);
        if (bar2->data() > 0)
        {
          PlotLineBar *bar = line->data(loop);
          bar->setColor(color);
        }
      }
      
      delete line2;
    }
  }

  s = "0";
  ind.setLine(s, line);
  ind.addPlotOrder(s);
  
  int period = _settings.getInt(MAPeriod);
  if (period > 1)
  {
    FunctionMA mau;
    _settings.getData(MAType, s);
    int type = mau.typeFromString(s);

    PlotLine *ma = mau.calculate(line, period, type);
    if (ma)
    {
      _settings.getData(MAPlot, s);
      ma->setType(s);

      _settings.getData(MAColor, s);
      ma->setColor(s);

      _settings.getData(MALabel, s);
      ma->setLabel(s);
      
      s = "1";
      ind.setLine(s, ma);
      ind.addPlotOrder(s);
    }
  }

  period = _settings.getInt(MA2Period);
  if (period > 1)
  {
    FunctionMA mau;
    _settings.getData(MA2Type, s);
    int type = mau.typeFromString(s);

    PlotLine *ma = mau.calculate(line, period, type);
    if (ma)
    {
      _settings.getData(MA2Plot, s);
      ma->setType(s);

      _settings.getData(MA2Color, s);
      ma->setColor(s);

      _settings.getData(MA2Label, s);
      ma->setLabel(s);
      
      s = "2";
      ind.setLine(s, ma);
      ind.addPlotOrder(s);
    }
  }

  period = _settings.getInt(MA3Period);
  if (period > 1)
  {
    FunctionMA mau;
    _settings.getData(MA3Type, s);
    int type = mau.typeFromString(s);

    PlotLine *ma = mau.calculate(line, period, type);
    if (ma)
    {
      _settings.getData(MA3Plot, s);
      ma->setType(s);

      _settings.getData(MA3Color, s);
      ma->setColor(s);

      _settings.getData(MA3Label, s);
      ma->setLabel(s);
      
      s = "3";
      ind.setLine(s, ma);
      ind.addPlotOrder(s);
    }
  }

  return 0;
}

int CANDLES::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionCANDLES f;
  return f.script(set, ind, data);
}

int CANDLES::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  _settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Candle Color"), d);

  _settings.getData(MethodColor, d);
  dialog->addColorItem(MethodColor, page, QObject::tr("Method Color"), d);

  _settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d, QString());

  FunctionCANDLES f;
  QStringList methodList = f.list();
  
  _settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), methodList, d);

  dialog->addDoubleItem(Penetration, page, QObject::tr("Penetration"), _settings.getDouble(Penetration), 0, 100);

  PlotStyleFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  page++;
  k = QObject::tr("MA 1");
  dialog->addPage(page, k);

  _settings.getData(MAColor, d);
  dialog->addColorItem(MAColor, page, QObject::tr("Color"), d);

  _settings.getData(MAPlot, d);
  dialog->addComboItem(MAPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MALabel, d);
  dialog->addTextItem(MALabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(MAPeriod, page, QObject::tr("Period"), _settings.getInt(MAPeriod), 1, 100000);

  FunctionMA mau;
  QStringList maList = mau.list();

  _settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("Type"), maList, d);

  page++;
  k = QObject::tr("MA 2");
  dialog->addPage(page, k);

  _settings.getData(MA2Color, d);
  dialog->addColorItem(MA2Color, page, QObject::tr("Color"), d);

  _settings.getData(MA2Plot, d);
  dialog->addComboItem(MA2Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MA2Label, d);
  dialog->addTextItem(MA2Label, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(MA2Period, page, QObject::tr("Period"), _settings.getInt(MA2Period), 1, 100000);

  _settings.getData(MA2Type, d);
  dialog->addComboItem(MA2Type, page, QObject::tr("Type"), maList, d);

  page++;
  k = QObject::tr("MA 3");
  dialog->addPage(page, k);

  _settings.getData(MA3Color, d);
  dialog->addColorItem(MA3Color, page, QObject::tr("Color"), d);

  _settings.getData(MA3Plot, d);
  dialog->addComboItem(MA3Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MA3Label, d);
  dialog->addTextItem(MA3Label, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(MA3Period, page, QObject::tr("Period"), _settings.getInt(MA3Period), 1, 100000);

  _settings.getData(MA3Type, d);
  dialog->addComboItem(MA3Type, page, QObject::tr("Type"), maList, d);

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
  CANDLES *o = new CANDLES;
  return ((IndicatorPlugin *) o);
}


