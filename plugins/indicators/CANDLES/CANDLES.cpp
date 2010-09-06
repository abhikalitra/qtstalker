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
#include "FunctionCANDLES.h"
#include "CANDLESDialog.h"
#include "Curve.h"

#include <QtDebug>

CANDLES::CANDLES ()
{
  _indicator = "CANDLES";
}

int CANDLES::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  FunctionCANDLES f;
  Curve *line = f.candles(data);
  if (! line)
    return 1;

  QString s;
  settings.getData(Color, s);
  QColor c(s);
  line->setAllColor(c);

  settings.getData(Label, s);
  line->setLabel(s);

  QStringList methodList = f.list();
  settings.getData(Method, s);
  int method = methodList.indexOf(s);
  if (method != FunctionCANDLES::_NONE)
  {
    double pen = settings.getDouble(Penetration);

    Curve *line2 = f.getMethod(data, method, pen);
    if (line2)
    {
      settings.getData(MethodColor, s);
      QColor color(s);
      
      int loop = 0;
      for (; loop < line2->count(); loop++)
      {
        CurveBar *bar2 = line2->bar(loop);
        if (bar2->data() > 0)
        {
          CurveBar *bar = line->bar(loop);
          bar->setColor(color);
        }
      }
      
      delete line2;
    }
  }

  line->setZ(0);

  ind.setLine(0, line);

  int period = settings.getInt(MAPeriod);
  if (period > 1)
  {
    FunctionMA mau;
    settings.getData(MAType, s);
    int type = mau.typeFromString(s);

    Curve *ma = mau.calculate(line, period, type);
    if (ma)
    {
      settings.getData(MAPlot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(MAColor, s);
      c.setNamedColor(s);
      ma->setColor(c);

      settings.getData(MALabel, s);
      ma->setLabel(s);
      
      ind.setLine(1, ma);

      ma->setZ(1);
    }
  }

  period = settings.getInt(MA2Period);
  if (period > 1)
  {
    FunctionMA mau;
    settings.getData(MA2Type, s);
    int type = mau.typeFromString(s);

    Curve *ma = mau.calculate(line, period, type);
    if (ma)
    {
      settings.getData(MA2Plot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(MA2Color, s);
      c.setNamedColor(s);
      ma->setColor(c);

      settings.getData(MA2Label, s);
      ma->setLabel(s);
      
      ind.setLine(2, ma);
      
      ma->setZ(2);
    }
  }

  period = settings.getInt(MA3Period);
  if (period > 1)
  {
    FunctionMA mau;
    settings.getData(MA3Type, s);
    int type = mau.typeFromString(s);

    Curve *ma = mau.calculate(line, period, type);
    if (ma)
    {
      settings.getData(MA3Plot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(MA3Color, s);
      c.setNamedColor(s);
      ma->setColor(c);

      settings.getData(MA3Label, s);
      ma->setLabel(s);
      
      ind.setLine(3, ma);
      
      ma->setZ(3);
    }
  }

  return 0;
}

int CANDLES::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionCANDLES f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * CANDLES::dialog (Indicator &i)
{
  return new CANDLESDialog(i);

/*
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
*/
}

void CANDLES::defaults (Indicator &i)
{
  Setting set;
  set.setData(CANDLES::Penetration, 0.3);
  set.setData(CANDLES::Method, "NONE");
  set.setData(CANDLES::MethodColor, "cyan");
  set.setData(CANDLES::Color, "green");
  set.setData(CANDLES::Label, _indicator);
  set.setData(CANDLES::MAColor, "red");
  set.setData(CANDLES::MA2Color, "yellow");
  set.setData(CANDLES::MA3Color, "blue");
  set.setData(CANDLES::MAPlot, "Line");
  set.setData(CANDLES::MA2Plot, "Line");
  set.setData(CANDLES::MA3Plot, "Line");
  set.setData(CANDLES::MALabel, "MA1");
  set.setData(CANDLES::MA2Label, "MA2");
  set.setData(CANDLES::MA3Label, "MA3");
  set.setData(CANDLES::MAPeriod, 20);
  set.setData(CANDLES::MA2Period, 50);
  set.setData(CANDLES::MA3Period, 200);
  set.setData(CANDLES::MAType, "SMA");
  set.setData(CANDLES::MA2Type, "SMA");
  set.setData(CANDLES::MA3Type, "SMA");
  i.setSettings(set);
}


//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CANDLES *o = new CANDLES;
  return ((IndicatorPlugin *) o);
}


