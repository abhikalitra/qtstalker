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

#include "BARS.h"
#include "Curve.h"
#include "FunctionBARS.h"
#include "FunctionMA.h"
#include "IndicatorSettings.h"

#include <QtDebug>

BARS::BARS ()
{
  _indicator = "BARS";
}

int BARS::getIndicator (Indicator &ind, BarData &data)
{
  IndicatorSettings settings = ind.settings();
  
  IndicatorSetting set = settings.setting(_UpColor);
  QColor up(set.value());

  set = settings.setting(_DownColor);
  QColor down(set.value());

  set = settings.setting(_NeutralColor);
  QColor neutral(set.value());

  FunctionBARS fbars;
  Curve *line = fbars.getBARS(up, down, neutral, data);
  if (! line)
    return 1;

  set = settings.setting(_BarsLabel);
  line->setLabel(set.value());

  line->setZ(0);
  ind.setLine(set.value(), line);

  FunctionMA fma;
  
  set = settings.setting(_MAPeriod);
  int period = set.value().toInt();
  if (period > 1)
  {
    set = settings.setting(_MAType);
    int type = fma.typeFromString(set.value());

    Curve *ma = fma.calculate(line, period, type);
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
  }

  set = settings.setting(_MA2Period);
  period = set.value().toInt();
  if (period > 1)
  {
    set = settings.setting(_MA2Type);
    int type = fma.typeFromString(set.value());

    Curve *ma = fma.calculate(line, period, type);
    if (ma)
    {
      set = settings.setting(_MA2Plot);
      ma->setType((Curve::Type) ma->typeFromString(set.value()));

      set = settings.setting(_MA2Color);
      QColor c(set.value());
      ma->setColor(c);

      set = settings.setting(_MA2Label);
      ma->setLabel(set.value());
      
      ma->setZ(2);
      ind.setLine(set.value(), ma);
    }
  }

  set = settings.setting(_MA3Period);
  period = set.value().toInt();
  if (period > 1)
  {
    set = settings.setting(_MA3Type);
    int type = fma.typeFromString(set.value());

    Curve *ma = fma.calculate(line, period, type);
    if (ma)
    {
      set = settings.setting(_MA3Plot);
      ma->setType((Curve::Type) ma->typeFromString(set.value()));

      set = settings.setting(_MA3Color);
      QColor c(set.value());
      ma->setColor(c);

      set = settings.setting(_MA3Label);
      ma->setLabel(set.value());
      
      ma->setZ(3);
      ind.setLine(set.value(), ma);
    }
  }
  
  return 0;
}

int BARS::getCUS (QStringList &l, Indicator &ind, BarData &data)
{
  IndicatorSettings settings = ind.settings();
  if (settings.setCUS(l))
    return 1;

  IndicatorSetting set = settings.setting(_BarsLabel);
  QString name = set.value();
  Curve *tl = ind.line(name);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << name;
    return 1;
  }

  set = settings.setting(_UpColor);
  QColor barUpColor(set.value());

  set = settings.setting(_DownColor);
  QColor barDownColor(set.value());

  set = settings.setting(_NeutralColor);
  QColor barNeutralColor(set.value());

  FunctionBARS fbars;
  Curve *line = fbars.getBARS(barUpColor, barDownColor, barNeutralColor, data);

  line->setLabel(name);

  ind.setLine(name, line);

  return 0;
}

IndicatorPluginDialog * BARS::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);
  dialog->setHelpFile("BARS.html");
  return dialog;
}

void BARS::defaults (Indicator &i)
{
  IndicatorSettings set;

  QStringList l;
  l << tr("General") << "MA 1" << "MA 2" << "MA 3";
  set.setTabs(l);

  l.clear();
  l << "INDICATOR" << "PLUGIN" << "BARS" << QString::number(_BarsLabel);
  l << QString::number(_UpColor) << QString::number(_DownColor) << QString::number(_NeutralColor);
  set.setCUSFormat(l);
  
  int page = 0;
  set.setColor(_UpColor, page, tr("Up Color"), QString("green"));
  set.setColor(_DownColor, page, tr("Down Color"), QString("red"));
  set.setColor(_NeutralColor, page, tr("Neutral Color"), QString("blue"));
  set.setText(_BarsLabel, page, tr("Label"), _indicator);

  page++;
  set.setColor(_MAColor, page, tr("Color"), QString("red"));
  set.setPlot(_MAPlot, page, tr("Plot"), QString("Line"));
  set.setText(_MALabel, page, tr("Label"), QString("MA1"));
  set.setInteger(_MAPeriod, page, tr("Period"), QString("20"));
  set.setMA(_MAType, page, tr("Type"), QString("SMA"));
  
  page++;
  set.setColor(_MA2Color, page, tr("Color"), QString("yellow"));
  set.setPlot(_MA2Plot, page, tr("Plot"), QString("Line"));
  set.setText(_MA2Label, page, tr("Label"), QString("MA2"));
  set.setInteger(_MA2Period, page, tr("Period"), QString("50"));
  set.setMA(_MA2Type, page, tr("Type"), QString("SMA"));

  page++;
  set.setColor(_MA3Color, page, tr("Color"), QString("blue"));
  set.setPlot(_MA3Plot, page, tr("Plot"), QString("Line"));
  set.setText(_MA3Label, page, tr("Label"), QString("MA3"));
  set.setInteger(_MA3Period, page, tr("Period"), QString("200"));
  set.setMA(_MA3Type, page, tr("Type"), QString("SMA"));

  i.setSettings(set);
}

void BARS::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  IndicatorSettings settings = i.settings();
  
  IndicatorSetting set = settings.setting(_BarsLabel);
  l.append(set.value());

  set = settings.setting(_MALabel);
  l.append(set.value());
  
  set = settings.setting(_MA2Label);
  l.append(set.value());

  set = settings.setting(_MA3Label);
  l.append(set.value());
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  BARS *o = new BARS;
  return ((IndicatorPlugin *) o);
}
