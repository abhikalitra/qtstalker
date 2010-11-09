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

#include "MFI.h"
#include "FunctionMA.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

MFI::MFI ()
{
  _indicator = "MFI";
}

int MFI::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  // Ref1 line
  QString s;
  Curve *line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(_Ref1Color, s);
  QColor color(s);
  
  line->setBar(0, new CurveBar(color, (double) settings.getInt(_Ref1)));
  
  line->setZ(0);
  ind.setLine(0, line);

  // Ref2 line
  line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(_Ref2Color, s);
  color.setNamedColor(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(_Ref2)));
  
  line->setZ(1);
  ind.setLine(1, line);

  // mfi plot
  int period = settings.getInt(_Period);
  int smoothing = settings.getInt(_Smoothing);

  FunctionMA mau;
  settings.getData(_SmoothingType, s);
  int type = mau.typeFromString(s);

  line = calculate(period, smoothing, type, data);
  if (! line)
    return 1;

  settings.getData(_Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_Color, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(_Label, s);
  line->setLabel(s);
  
  line->setZ(2);
  ind.setLine(s, line);

  return 0;
}

int MFI::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,MFI,<NAME>,<PERIOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>
  //     0       1     2    3       4             5                 6

  if (set.count() != 7)
  {
    qDebug() << "MFI::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "MFI::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MFI::getCUS: invalid period settings" << set[4];
    return 1;
  }

  int smoothing = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MFI::getCUS: invalid smoothing period" << set[5];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[6]);
  if (ma == -1)
  {
    qDebug() << "MFI::getCUS: invalid smoothing type" << set[6];
    return 1;
  }

  Curve *line = calculate(period, smoothing, ma, data);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * MFI::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  dialog->setHelpFile("MFI.html");

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

  dialog->addInt(tab, _Period, tr("Period"), _settings.getInt(_Period), 100000, 2);

  dialog->addInt(tab, _Smoothing, tr("Smoothing"), _settings.getInt(_Smoothing), 100000, 1);

  _settings.getData(_SmoothingType, d);
  dialog->addMA(tab, _SmoothingType, tr("Smoothing Type"), d);

  // ref1 tab
  tab = dialog->addTab("Ref 1");

  _settings.getData(_Ref1Color, d);
  dialog->addColor(tab, _Ref1Color, tr("Color"), d);

  dialog->addDouble(tab, _Ref1, tr("Value"), _settings.getDouble(_Ref1), 100000, -100000);

  // ref2 tab
  tab = dialog->addTab("Ref 2");

  _settings.getData(_Ref2Color, d);
  dialog->addColor(tab, _Ref2Color, tr("Color"), d);

  dialog->addDouble(tab, _Ref2, tr("Value"), _settings.getDouble(_Ref2), 100000, -100000);

  return dialog;
}

void MFI::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Plot, "Line");
  set.setData(_Label, _indicator);
  set.setData(_Period, 14);
  set.setData(_Smoothing, 10);
  set.setData(_SmoothingType, "SMA");
  set.setData(_Ref1Color, "white");
  set.setData(_Ref2Color, "white");
  set.setData(_Ref1, 20);
  set.setData(_Ref2, 80);
  i.setSettings(set);
}

void MFI::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * MFI::calculate (int period, int smoothing, int type, BarData &data)
{
  int size = data.count();

  if (size < period || size < smoothing)
    return 0;

  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_MFI(0,
                         size - 1,
                         data.getTAData(BarData::High),
                         data.getTAData(BarData::Low),
                         data.getTAData(BarData::Close),
                         data.getTAData(BarData::Volume),
                         period,
                         &outBeg,
                         &outNb,
                         &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "MFI::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
    outLoop--;
  }

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
  MFI *o = new MFI;
  return ((IndicatorPlugin *) o);
}


