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

#include "MAMA.h"
#include "FunctionMAMA.h"
#include "BARSUtils.h"
#include "PlotStyleFactory.h"

#include <QtDebug>

MAMA::MAMA ()
{
  _indicator = "MAMA";

  _settings.setData(OSC, 1);
  _settings.setData(OSCColor, "red");
  _settings.setData(MAMAColor, "red");
  _settings.setData(FAMAColor, "yellow");
  _settings.setData(MAMAPlot, "Line");
  _settings.setData(FAMAPlot, "Line");
  _settings.setData(MAMALabel, "MAMA");
  _settings.setData(FAMALabel, "FAMA");
  _settings.setData(OSCLabel, "MAMAOSC");
  _settings.setData(FastLimit, 0.5);
  _settings.setData(SlowLimit, 0.05);
  _settings.setData(Input, "Close");
}

int MAMA::getIndicator (Indicator &ind, BarData &data)
{
  QString s;
  _settings.getData(Input, s);
  PlotLine *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  double fast = _settings.getDouble(FastLimit);
  double slow = _settings.getDouble(SlowLimit);

  FunctionMAMA f;
  QList<PlotLine *> l;
  if (f.calculate(in, fast, slow, l))
  {
    delete in;
    return 1;
  }

  PlotLine *mama = l.at(0);
  PlotLine *fama = l.at(1);

  int osc = _settings.getInt(OSC);
  if (osc)
  {
    s = "Histogram Bar";
    PlotLine *line = new PlotLine;
    line->setType(s);

    _settings.getData(OSCLabel, s);
    line->setLabel(s);

    QList<int> keys;
    mama->keys(keys);

    int loop = 0;
    for (; loop < keys.count(); loop++)
    {
      PlotLineBar *mbar = mama->data(keys.at(loop));
      if (! mbar)
        continue;

      PlotLineBar *fbar = fama->data(keys.at(loop));
      if (! fbar)
        continue;

      line->setData(keys.at(loop), mbar->data() - fbar->data());
    }

    _settings.getData(OSCColor, s);
    line->setColor(s);

    s = "0";
    ind.setLine(s, line);
    ind.addPlotOrder(s);
  }
  else
  {
    QColor up("green");
    QColor down("red");
    QColor neutral("blue");
    BARSUtils b;
    PlotLine *bars = b.getBARS(data, up, down, neutral);
    if (bars)
    {
      s = "1";
      ind.setLine(s, bars);
      ind.addPlotOrder(s);
    }

    _settings.getData(MAMAPlot, s);
    mama->setType(s);

    _settings.getData(MAMAColor, s);
    mama->setColor(s);

    _settings.getData(MAMALabel, s);
    mama->setLabel(s);
    
    s = "2";
    ind.setLine(s, mama);
    ind.addPlotOrder(s);

    // fama line
    _settings.getData(FAMAPlot, s);
    fama->setType(s);

    _settings.getData(FAMAColor, s);
    fama->setColor(s);

    _settings.getData(FAMALabel, s);
    fama->setLabel(s);
    
    s = "3";
    ind.setLine(s, fama);
    ind.addPlotOrder(s);
  }

  delete in;

  return 0;
}

int MAMA::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionMAMA f;
  return f.script(set, ind, data);
}

int MAMA::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  _settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addDoubleItem(FastLimit, page, QObject::tr("Fast Limit"), _settings.getDouble(FastLimit), 0.01, 0.99);

  dialog->addDoubleItem(SlowLimit, page, QObject::tr("Slow Limit"), _settings.getDouble(SlowLimit), 0.01, 0.99);

  dialog->addCheckItem(OSC, page, QObject::tr("Oscillator"), _settings.getInt(OSC));

  page++;
  k = QObject::tr("MAMA");
  dialog->addPage(page, k);

  _settings.getData(MAMAColor, d);
  dialog->addColorItem(MAMAColor, page, QObject::tr("Color"), d);

  PlotStyleFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(MAMAPlot, d);
  dialog->addComboItem(MAMAPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MAMALabel, d);
  dialog->addTextItem(MAMALabel, page, QObject::tr("Label"), d, QString());

  page++;
  k = QObject::tr("FAMA");
  dialog->addPage(page, k);

  _settings.getData(FAMAColor, d);
  dialog->addColorItem(FAMAColor, page, QObject::tr("Color"), d);

  _settings.getData(FAMAPlot, d);
  dialog->addComboItem(FAMAPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(FAMALabel, d);
  dialog->addTextItem(FAMALabel, page, QObject::tr("Label"), d, QString());

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
  MAMA *o = new MAMA;
  return ((IndicatorPlugin *) o);
}


