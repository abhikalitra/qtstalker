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
#include "IndicatorSettings.h"

#include <QtDebug>

VOL::VOL ()
{
  _indicator = "VOL";
}

int VOL::getIndicator (Indicator &ind, BarData &data)
{
  IndicatorSettings settings = ind.settings();
  
  IndicatorSetting set = settings.setting(_UpColor);
  QColor up(set.value());

  set = settings.setting(_DownColor);
  QColor down(set.value());

  set = settings.setting(_NeutralColor);
  QColor neutral(set.value());

  Curve *line = getVOL(up, down, neutral, data);
  if (! line)
    return 1;

  set = settings.setting(_Plot);
  line->setType((Curve::Type) line->typeFromString(set.value()));

  set = settings.setting(_Label);
  line->setLabel(set.value());

  line->setZ(0);
  ind.setLine(set.value(), line);

  // vol ma
  set = settings.setting(_MAPeriod);
  int period = set.value().toInt();

  set = settings.setting(_MAType);
  FunctionMA mau;
  int type = mau.typeFromString(set.value());

  Curve *ma = mau.calculate(line, period, type);
  if (ma)
  {
    set = settings.setting(_MAPlot);
    ma->setType((Curve::Type) ma->typeFromString(set.value()));

    set = settings.setting(_MAColor);
    QColor c(set.value());
    ma->setColor(c);

    set = settings.setting(_MALabel);
    ma->setLabel(set.value());
    
    ma->setZ(1);
    ind.setLine(set.value(), ma);
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
  dialog->setHelpFile("VOL.html");
  return dialog;
}

void VOL::defaults (Indicator &i)
{
  IndicatorSettings set;

  QStringList l;
  l << tr("General") << "MA";
  set.setTabs(l);

  int page = 0;
  set.setColor(_UpColor, page, tr("Up Color"), QString("green"));
  set.setColor(_DownColor, page, tr("Down Color"), QString("red"));
  set.setColor(_NeutralColor, page, tr("Neutral Color"), QString("blue"));
  set.setPlot(_Plot, page, tr("Plot"), QString("Histogram Bar"));
  set.setText(_Label, page, tr("Label"), _indicator);

  page++;
  set.setColor(_MAColor, page, tr("Color"), QString("yellow"));
  set.setPlot(_MAPlot, page, tr("Plot"), QString("Line"));
  set.setText(_MALabel, page, tr("Label"), QString("VOLMA"));
  set.setInteger(_MAPeriod, page, tr("Period"), QString("10"));
  set.setMA(_MAType, page, tr("Type"), QString("SMA"));

  i.setSettings(set);
}

void VOL::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  IndicatorSettings settings = i.settings();

  IndicatorSetting set = settings.setting(_Label);
  l.append(set.value());

  set = settings.setting(_MALabel);
  l.append(set.value());
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  VOL *o = new VOL;
  return ((IndicatorPlugin *) o);
}
