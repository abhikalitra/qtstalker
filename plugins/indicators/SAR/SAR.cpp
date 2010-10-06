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

#include "SAR.h"
#include "FunctionBARS.h"
#include "SARDialog.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

SAR::SAR ()
{
  _indicator = "SAR";
}

int SAR::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  double tinit = settings.getDouble(_Init);
  double tmax = settings.getDouble(_Max);

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  FunctionBARS b;
  Curve *bars = b.getBARS(up, down, neutral, data);
  if (bars)
  {
    bars->setZ(0);
    ind.setLine(0, bars);
  }

  Curve *line = calculate(tinit, tmax, data);
  if (! line)
    return 1;

  line->setType((Curve::Dot));
  
  QString s;
  settings.getData(_Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(_Label, s);
  line->setLabel(s);
  
  line->setZ(1);
  ind.setLine(s, line);

  return 0;
}

int SAR::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,SAR,<NAME>,<INITIAL_STEP>,<MAX_STEP>
  //      0       1    2     3         4            5

  if (set.count() != 8)
  {
    qDebug() << "SAR::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "SAR::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  double init = set[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAR::getCUS: invalid init" << set[4];
    return 1;
  }

  double max = set[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAR::getCUS: invalid max" << set[5];
    return 1;
  }

  Curve *line = calculate(init, max, data);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * SAR::dialog (Indicator &i)
{
  return new SARDialog(i);
}

void SAR::defaults (Indicator &i)
{
  Setting set;
  set.setData(SAR::_Color, "yellow");
  set.setData(SAR::_Label, _indicator);
  set.setData(SAR::_Init, 0.02);
  set.setData(SAR::_Max, 0.2);
  i.setSettings(set);
}

void SAR::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * SAR::calculate (double _init, double _max, BarData &data)
{
  int size = data.count();

  if (size < 1)
    return 0;

  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_SAR(0,
                         size - 1,
                         data.getTAData(BarData::High),
                         data.getTAData(BarData::Low),
                         _init,
                         _max,
                         &outBeg,
                         &outNb,
                         &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "SAR::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve(Curve::Dot);

  int loop = 0;
  for (; loop < outNb; loop++)
    line->setBar(loop + 1, new CurveBar(out[loop]));

  return line;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  SAR *o = new SAR;
  return ((IndicatorPlugin *) o);
}


