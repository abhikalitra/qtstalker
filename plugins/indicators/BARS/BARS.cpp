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
#include "FunctionMA.h"
#include "FunctionBARS.h"
#include "BARSDialog.h"
#include "Curve.h"

#include <QtDebug>
#include <QObject>

BARS::BARS ()
{
  _indicator = "BARS";
}

int BARS::getIndicator (Indicator &ind)
{
  Setting settings = ind.settings();
  
  QString s;
  settings.getData(UpColor, s);
  QColor up(s);

  settings.getData(DownColor, s);
  QColor down(s);

  settings.getData(NeutralColor, s);
  QColor neutral(s);

  FunctionBARS b;
  Curve *line = b.getBARS(up, down, neutral);
  if (! line)
    return 1;

  settings.getData(BarsLabel, s);
  line->setLabel(s);

  line->setZ(0);
  ind.setLine(0, line);

  int period = settings.getInt(MAPeriod);
  if (period > 1)
  {
    settings.getData(MAType, s);
    FunctionMA mafac;
    int type = mafac.typeFromString(s);

    Curve *ma = mafac.calculate(line, period, type);
    if (ma)
    {
      settings.getData(MAPlot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(MAColor, s);
      QColor c(s);
      ma->setColor(c);

      settings.getData(MALabel, s);
      ma->setLabel(s);
      
      ma->setZ(1);
      
      ind.setLine(1, ma);
    }
  }

  period = settings.getInt(MA2Period);
  if (period > 1)
  {
    settings.getData(MA2Type, s);
    FunctionMA mafac;
    int type = mafac.typeFromString(s);

    Curve *ma = mafac.calculate(line, period, type);
    if (ma)
    {
      settings.getData(MA2Plot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(MA2Color, s);
      QColor c(s);
      ma->setColor(c);

      settings.getData(MA2Label, s);
      ma->setLabel(s);
      
      ma->setZ(2);
      
      ind.setLine(2, ma);
    }
  }

  period = settings.getInt(MA3Period);
  if (period > 1)
  {
    settings.getData(MA3Type, s);
    FunctionMA mafac;
    int type = mafac.typeFromString(s);

    Curve *ma = mafac.calculate(line, period, type);
    if (ma)
    {
      settings.getData(MA3Plot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(MA3Color, s);
      QColor c(s);
      ma->setColor(c);

      settings.getData(MA3Label, s);
      ma->setLabel(s);
      
      ma->setZ(3);
      
      ind.setLine(3, ma);
    }
  }
  
  return 0;
}

int BARS::getCUS (QStringList &set, Indicator &ind)
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

  FunctionBARS b;
  Curve *line = b.getBARS(barUpColor, barDownColor, barNeutralColor);

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * BARS::dialog (Indicator &i)
{
  return new BARSDialog(i);
}

void BARS::defaults (Indicator &i)
{
  Setting set;
  set.setData(BARS::UpColor, "green");
  set.setData(BARS::DownColor, "red");
  set.setData(BARS::NeutralColor, "blue");
  set.setData(BARS::BarsLabel, _indicator);
  set.setData(BARS::MAColor, "red");
  set.setData(BARS::MA2Color, "yellow");
  set.setData(BARS::MA3Color, "blue");
  set.setData(BARS::MAPlot, "Line");
  set.setData(BARS::MA2Plot, "Line");
  set.setData(BARS::MA3Plot, "Line");
  set.setData(BARS::MALabel, "MA1");
  set.setData(BARS::MA2Label, "MA2");
  set.setData(BARS::MA3Label, "MA3");
  set.setData(BARS::MAPeriod, 20);
  set.setData(BARS::MA2Period, 50);
  set.setData(BARS::MA3Period, 200);
  set.setData(BARS::MAType, "SMA");
  set.setData(BARS::MA2Type, "SMA");
  set.setData(BARS::MA3Type, "SMA");
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  BARS *o = new BARS;
  return ((IndicatorPlugin *) o);
}


