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

#include "BOP.h"
#include "FunctionMA.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

BOP::BOP ()
{
  _indicator = "BOP";
}

int BOP::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QString s;
  int smoothing = settings.getInt(_Smoothing);

  FunctionMA mau;
  settings.getData(_SmoothingType, s);
  int type = mau.typeFromString(s);

  Curve *line = calculate(smoothing, type, data);
  if (! line)
    return 1;

  settings.getData(_Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(_Label, s);
  line->setLabel(s);
  
  line->setZ(0);
  ind.setLine(s, line);

  return 0;
}

int BOP::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,BOP,<NAME>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>
  //     0       1     2    3             4                 5

  if (set.count() != 6)
  {
    qDebug() << "BOP::getCUS: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "BOP::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int smoothing = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "BOP::getCUS: invalid smoothing period" << set[4];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[5]);
  if (ma == -1)
  {
    qDebug() << "BOP::getCUS: invalid smoothing type" << set[5];
    return 1;
  }

  Curve *line = calculate(smoothing, ma, data);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * BOP::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  Setting _settings = i.settings();

  // general tab
  int tab = dialog->addTab(tr("General"));

  QString d;
  _settings.getData(_Color, d);
  dialog->addColor(tab, _Color, tr("Color"), d);

  _settings.getData(_Plot, d);
  dialog->addPlot(tab, _Plot, tr("Plot"), d);

  _settings.getData(_Label, d);
  dialog->addText(tab, _Label, tr("Label"), d);

  dialog->addInt(tab, _Smoothing, tr("Smoothing"), _settings.getInt(_Smoothing), 100000, 1);

  FunctionMA mau;
  QStringList l = mau.list();

  _settings.getData(_SmoothingType, d);
  dialog->addCombo(tab, _SmoothingType, tr("Smoothing Type"), l, d);

  return dialog;
}

void BOP::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Plot, "Histogram Bar");
  set.setData(_Label, _indicator);
  set.setData(_Smoothing, 10);
  set.setData(_SmoothingType, "SMA");
  i.setSettings(set);
}

void BOP::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * BOP::calculate (int smoothing, int type, BarData &data)
{
  int size = data.count();

  if (size < 1 || size < smoothing)
    return 0;

  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_BOP(0,
                         size - 1,
                         data.getTAData(BarData::Open),
                         data.getTAData(BarData::High),
                         data.getTAData(BarData::Low),
                         data.getTAData(BarData::Close),
                         &outBeg,
                         &outNb,
                         &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "BOP::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int loop = 0;
  for (; loop < size; loop++)
    line->setBar(loop, new CurveBar(out[loop]));

  if (smoothing > 1)
  {
    FunctionMA mau;
    Curve *ma = mau.calculate(line, smoothing, type);
    delete line;
    line = ma;
  }

  return line;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  BOP *o = new BOP;
  return ((IndicatorPlugin *) o);
}


