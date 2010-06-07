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

#include "MA.h"
#include "MAFactory.h"
#include "BARSUtils.h"
#include "PlotFactory.h"

#include <QtDebug>
#include <cmath>

MA::MA ()
{
  _indicator = "MA";

  _settings.setData(Color, "red");
  _settings.setData(Plot, "Line");
  _settings.setData(Label, _indicator);
  _settings.setData(Input, "Close");
  _settings.setData(Period, 14);
  _settings.setData(Method, "SMA");
}

int MA::getIndicator (Indicator &ind, BarData *data)
{
  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  BARSUtils b;
  PlotLine *bars = b.getBARS(data, up, down, neutral);
  if (bars)
  {
    QString s = "0";
    ind.setLine(s, bars);
    ind.addPlotOrder(s);
  }

  QString s;
  _settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  int period = _settings.getInt(Period);

  _settings.getData(Method, s);
  MAFactory mau;
  int method = mau.typeFromString(s);

  _settings.getData(Color, s);
  QColor color(s);

  PlotFactory fac;
  _settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  PlotLine *line = mau.ma(in, period, method, lineType, color);
  if (! line)
  {
    delete in;
    return 1;
  }

  _settings.getData(Label, s);
  line->setLabel(s);
  
  s = "1";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  delete in;

  return 0;
}

int MA::getCUS (QStringList &set, Indicator &ind, BarData *data)
{
  // INDICATOR,PLUGIN,MA,<METHOD>,<NAME>,<INPUT>,<PERIOD>,<PLOT TYPE>,<COLOR>
  //     0       1    2     3       4       5       6          7         8

  if (set.count() != 9)
  {
    qDebug() << _indicator << "::getCUS: invalid parm count" << set.count();
    return 1;
  }

  MAFactory mau;
  int method = mau.typeFromString(set[3]);
  if (method == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid method" << set[3];
    return 1;
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[4];
    return 1;
  }

  PlotLine *in = ind.line(set[5]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[5]));
    if (! in)
    {
      qDebug() << _indicator << "::getCUS: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid period" << set[6];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[7]);
  if (lineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid plot type" << set[7];
    return 1;
  }

  QColor color(set[8]);
  if (! color.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid color" << set[8];
    return 1;
  }

  PlotLine *line = mau.ma(in, period, method, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

int MA::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  _settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  PlotFactory fac;
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

  dialog->addIntItem(Period, page, QObject::tr("Period"), _settings.getInt(Period), 2, 100000);

  MAFactory mau;
  QStringList methodList = mau.list();
  
  _settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), methodList, d);

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
  MA *o = new MA;
  return ((IndicatorPlugin *) o);
}


