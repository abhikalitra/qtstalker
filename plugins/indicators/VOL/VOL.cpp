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

#include "VOL.h"
#include "FunctionMA.h"
#include "PlotOHLC.h"
#include "Curve.h"

#include <QtDebug>

VOL::VOL ()
{
  _indicator = "VOL";
}

int VOL::getIndicator (Indicator &ind, BarData &data)
{
  Setting set = ind.settings();
  
  QString s;
  set.getData(_UpColor, s);
  QColor up(s);

  set.getData(_DownColor, s);
  QColor down(s);

  set.getData(_NeutralColor, s);
  QColor neutral(s);

  Curve *line = getVOL(up, down, neutral, data);
  if (! line)
    return 1;

  set.getData(_Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  set.getData(_Label, s);
  line->setLabel(s);

  line->setZ(0);
  ind.setLine(s, line);

  // vol ma
  int period = set.getInt(_MAPeriod);

  set.getData(_MAType, s);
  FunctionMA mau;
  int type = mau.typeFromString(s);

  Curve *ma = mau.calculate(line, period, type);
  if (ma)
  {
    set.getData(_MAPlot, s);
    ma->setType((Curve::Type) ma->typeFromString(s));

    set.getData(_MAColor, s);
    QColor c(s);
    ma->setColor(c);

    set.getData(_MALabel, s);
    ma->setLabel(s);
    
    ma->setZ(1);
    ind.setLine(s, ma);
  }

  return 0;
}

Curve * VOL::getVOL (QColor &up, QColor &down, QColor &neutral, BarData &data)
{
  if (data.count() < 2)
    return 0;
  
  Curve *vol = new Curve;

  // set the first bar to neutral
  int loop = 0;
  Bar bar = data.getBar(loop);

  vol->setBar(loop, new CurveBar(neutral, bar.getVolume()));

  loop++;
  for (; loop < data.count(); loop++)
  {
    Bar bar = data.getBar(loop);
    Bar pbar = data.getBar(loop - 1);

    if (bar.getClose() < pbar.getClose())
      vol->setBar(loop, new CurveBar(down, bar.getVolume()));
    else
    {
      if (bar.getClose() > pbar.getClose())
        vol->setBar(loop, new CurveBar(up, bar.getVolume()));
      else
        vol->setBar(loop, new CurveBar(neutral, bar.getVolume()));
    }
  }

  return vol;
}

IndicatorPluginDialog * VOL::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  Setting _settings = i.settings();

  // general tab
  int tab = dialog->addTab(tr("General"));

  QString d;
  _settings.getData(_UpColor, d);
  dialog->addColor(tab, _UpColor, tr("Up Color"), d);

  _settings.getData(_DownColor, d);
  dialog->addColor(tab, _DownColor, tr("Down Color"), d);

  _settings.getData(_NeutralColor, d);
  dialog->addColor(tab, _NeutralColor, tr("Neutral Color"), d);

  _settings.getData(_Plot, d);
  dialog->addPlot(tab, _Plot, tr("Plot"), d);

  _settings.getData(_Label, d);
  dialog->addText(tab, _Label, tr("Label"), d);

  // MA tab
  tab = dialog->addTab("MA");

  _settings.getData(_MAColor, d);
  dialog->addColor(tab, _MAColor, tr("Color"), d);

  _settings.getData(_MAPlot, d);
  dialog->addPlot(tab, _MAPlot, tr("Plot"), d);

  _settings.getData(_MALabel, d);
  dialog->addText(tab, _MALabel, tr("Label"), d);

  dialog->addInt(tab, _MAPeriod, tr("Period"), _settings.getInt(_MAPeriod), 100000, 1);

  _settings.getData(_MAType, d);
  dialog->addMA(tab, _MAType, tr("Type"), d);

  return dialog;
}

void VOL::defaults (Indicator &i)
{
  Setting set;
  set.setData(_UpColor, "green");
  set.setData(_DownColor, "red");
  set.setData(_NeutralColor, "blue");
  set.setData(_Plot, "Histogram Bar");
  set.setData(_Label, _indicator);
  set.setData(_MAColor, "yellow");
  set.setData(_MAPlot, "Line");
  set.setData(_MALabel, "VOLMA");
  set.setData(_MAPeriod, 10);
  set.setData(_MAType, "SMA");
  i.setSettings(set);
}

void VOL::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  
  settings.getData(_Label, s);
  l.append(s);

  settings.getData(_MALabel, s);
  l.append(s);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  VOL *o = new VOL;
  return ((IndicatorPlugin *) o);
}


