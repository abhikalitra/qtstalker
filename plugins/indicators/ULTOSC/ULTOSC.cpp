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

#include "ULTOSC.h"
#include "ULTOSCDialog.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

ULTOSC::ULTOSC ()
{
  _indicator = "ULTOSC";
}

int ULTOSC::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  // 30 ref line
  QString s;
  Curve *line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(_Ref1Color, s);
  QColor color(s);

  line->setBar(0, new CurveBar(color, settings.getDouble(_Ref1)));
  
  line->setZ(0);
  ind.setLine(0, line);

  // 50 ref line
  line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(_Ref2Color, s);
  color.setNamedColor(s);

  line->setBar(0, new CurveBar(color, settings.getDouble(_Ref2)));
  
  line->setZ(1);
  ind.setLine(1, line);

  // 70 ref line
  line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(_Ref3Color, s);
  color.setNamedColor(s);

  line->setBar(0, new CurveBar(color, settings.getDouble(_Ref3)));
  
  line->setZ(2);
  ind.setLine(2, line);

  // ultosc line
  int sp = settings.getInt(_ShortPeriod);
  int mp = settings.getInt(_MidPeriod);
  int lp = settings.getInt(_LongPeriod);

  line = calculate(sp, mp, lp, data);
  if (! line)
    return 1;

  settings.getData(_Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_Color, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(_Label, s);
  line->setLabel(s);
  
  line->setZ(3);
  ind.setLine(3, line);

  return 0;
}

int ULTOSC::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,ULTOSC,<NAME>,<SHORT PERIOD>,<MED PERIOD>,<LONG PERIOD>
  //     0       1       2     3          4             5             6

  if (set.count() != 7)
  {
    qDebug() << "ULTOSC::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "ULTOSC::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int sp = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ULTOSC::getCUS: invalid short period" << set[4];
    return 1;
  }

  int mp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ULTOSC::getCUS: invalid med period" << set[5];
    return 1;
  }

  int lp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ULTOSC::getCUS: invalid long period" << set[6];
    return 1;
  }

  Curve *line = calculate(sp, mp, lp, data);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * ULTOSC::dialog (Indicator &i)
{
  return new ULTOSCDialog(i);
}

void ULTOSC::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Plot, "Line");
  set.setData(_Label, _indicator);
  set.setData(_ShortPeriod, 7);
  set.setData(_MidPeriod, 14);
  set.setData(_LongPeriod, 28);
  set.setData(_Ref1Color, "white");
  set.setData(_Ref2Color, "white");
  set.setData(_Ref3Color, "white");
  set.setData(_Ref1, 30);
  set.setData(_Ref2, 50);
  set.setData(_Ref3, 70);
  i.setSettings(set);
}

void ULTOSC::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * ULTOSC::calculate (int sp, int mp, int lp, BarData &data)
{
  int size = data.count();

  if (size < sp || size < mp || size < lp)
    return 0;

  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_ULTOSC(0,
                            size - 1,
                            data.getTAData(BarData::High),
                            data.getTAData(BarData::Low),
                            data.getTAData(BarData::Close),
                            sp,
                            mp,
                            lp,
                            &outBeg,
                            &outNb,
                            &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ULTOSC::calculate: TA-Lib error" << rc;
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

  return line;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  ULTOSC *o = new ULTOSC;
  return ((IndicatorPlugin *) o);
}

