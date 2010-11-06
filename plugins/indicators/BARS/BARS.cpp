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

#include <QtDebug>
#include <QObject>

BARS::BARS ()
{
  _indicator = "BARS";
}

int BARS::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();
  
  QString s;
  settings.getData(_UpColor, s);
  QColor up(s);

  settings.getData(_DownColor, s);
  QColor down(s);

  settings.getData(_NeutralColor, s);
  QColor neutral(s);

  FunctionBARS fbars;
  Curve *line = fbars.getBARS(up, down, neutral, data);
  if (! line)
    return 1;

  settings.getData(_BarsLabel, s);
  line->setLabel(s);

  line->setZ(0);
  ind.setLine(s, line);

  FunctionMA fma;
  
  int period = settings.getInt(_MAPeriod);
  if (period > 1)
  {
    settings.getData(_MAType, s);
    int type = fma.typeFromString(s);

    Curve *ma = fma.calculate(line, period, type);
    if (ma)
    {
      settings.getData(_MAPlot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(_MAColor, s);
      QColor c(s);
      ma->setColor(c);

      settings.getData(_MALabel, s);
      ma->setLabel(s);
      
      ma->setZ(1);
      ind.setLine(s, ma);
    }
  }

  period = settings.getInt(_MA2Period);
  if (period > 1)
  {
    settings.getData(_MA2Type, s);
    int type = fma.typeFromString(s);

    Curve *ma = fma.calculate(line, period, type);
    if (ma)
    {
      settings.getData(_MA2Plot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(_MA2Color, s);
      QColor c(s);
      ma->setColor(c);

      settings.getData(_MA2Label, s);
      ma->setLabel(s);
      
      ma->setZ(2);
      ind.setLine(s, ma);
    }
  }

  period = settings.getInt(_MA3Period);
  if (period > 1)
  {
    settings.getData(_MA3Type, s);
    int type = fma.typeFromString(s);

    Curve *ma = fma.calculate(line, period, type);
    if (ma)
    {
      settings.getData(_MA3Plot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(_MA3Color, s);
      QColor c(s);
      ma->setColor(c);

      settings.getData(_MA3Label, s);
      ma->setLabel(s);
      
      ma->setZ(3);
      ind.setLine(s, ma);
    }
  }
  
  return 0;
}

int BARS::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,BARS,<NAME>,<BAR_UP_COLOR>,<BAR_DOWN_COLOR>,<BAR_NEUTRAL_COLOR>
  //     0       1     2     3          4              5                  6

  if (set.count() != 7)
  {
    qDebug() << _indicator << "::getCUS: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  QColor barUpColor(set[4]);
  if (! barUpColor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid bar up color" << set[4];
    return 1;
  }

  QColor barDownColor(set[5]);
  if (! barDownColor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid bar down color" << set[5];
    return 1;
  }

  QColor barNeutralColor(set[6]);
  if (! barNeutralColor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid bar neutral color" << set[6];
    return 1;
  }

  FunctionBARS fbars;
  Curve *line = fbars.getBARS(barUpColor, barDownColor, barNeutralColor, data);

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * BARS::dialog (Indicator &i)
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

  _settings.getData(_BarsLabel, d);
  dialog->addText(tab, _BarsLabel, tr("Label"), d);

  // MA1 tab
  tab = dialog->addTab("MA 1");

  _settings.getData(_MAColor, d);
  dialog->addColor(tab, _MAColor, tr("Color"), d);

  _settings.getData(_MAPlot, d);
  dialog->addPlot(tab, _MAPlot, tr("Plot"), d);

  _settings.getData(_MALabel, d);
  dialog->addText(tab, _MALabel, tr("Label"), d);

  dialog->addInt(tab, _MAPeriod, tr("Period"), _settings.getInt(_MAPeriod), 100000, 1);

  _settings.getData(_MAType, d);
  dialog->addMA(tab, _MAType, tr("Type"), d);

  // MA2 tab
  tab = dialog->addTab("MA 2");

  _settings.getData(_MA2Color, d);
  dialog->addColor(tab, _MA2Color, tr("Color"), d);

  _settings.getData(_MA2Plot, d);
  dialog->addPlot(tab, _MA2Plot, tr("Plot"), d);

  _settings.getData(_MA2Label, d);
  dialog->addText(tab, _MA2Label, tr("Label"), d);

  dialog->addInt(tab, _MA2Period, tr("Period"), _settings.getInt(_MA2Period), 100000, 1);

  _settings.getData(_MA2Type, d);
  dialog->addMA(tab, _MA2Type, tr("Type"), d);

  // MA3 tab
  tab = dialog->addTab("MA 3");

  _settings.getData(_MA3Color, d);
  dialog->addColor(tab, _MA3Color, tr("Color"), d);

  _settings.getData(_MA3Plot, d);
  dialog->addPlot(tab, _MA3Plot, tr("Plot"), d);

  _settings.getData(_MA3Label, d);
  dialog->addText(tab, _MA3Label, tr("Label"), d);

  dialog->addInt(tab, _MA3Period, tr("Period"), _settings.getInt(_MA3Period), 100000, 1);

  _settings.getData(_MA3Type, d);
  dialog->addMA(tab, _MA3Type, tr("Type"), d);

  return dialog;
}

void BARS::defaults (Indicator &i)
{
  Setting set;
  set.setData(BARS::_UpColor, "green");
  set.setData(BARS::_DownColor, "red");
  set.setData(BARS::_NeutralColor, "blue");
  set.setData(BARS::_BarsLabel, _indicator);
  set.setData(BARS::_MAColor, "red");
  set.setData(BARS::_MA2Color, "yellow");
  set.setData(BARS::_MA3Color, "blue");
  set.setData(BARS::_MAPlot, "Line");
  set.setData(BARS::_MA2Plot, "Line");
  set.setData(BARS::_MA3Plot, "Line");
  set.setData(BARS::_MALabel, "MA1");
  set.setData(BARS::_MA2Label, "MA2");
  set.setData(BARS::_MA3Label, "MA3");
  set.setData(BARS::_MAPeriod, 20);
  set.setData(BARS::_MA2Period, 50);
  set.setData(BARS::_MA3Period, 200);
  set.setData(BARS::_MAType, "SMA");
  set.setData(BARS::_MA2Type, "SMA");
  set.setData(BARS::_MA3Type, "SMA");
  i.setSettings(set);
}

void BARS::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  
  settings.getData(_BarsLabel, s);
  l.append(s);

  settings.getData(_MALabel, s);
  l.append(s);
  
  settings.getData(_MA2Label, s);
  l.append(s);

  settings.getData(_MA3Label, s);
  l.append(s);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  BARS *o = new BARS;
  return ((IndicatorPlugin *) o);
}


