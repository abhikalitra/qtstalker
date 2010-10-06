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

#include "ADX.h"
#include "Curve.h"
#include "ADXDialog.h"
#include "ta_libc.h"

#include <QtDebug>

ADX::ADX ()
{
  _indicator = "ADX";
  
  _methodList << "ADX" << "ADXR" << "+DI" << "-DI" << "DX";
}

int ADX::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  int period = settings.getInt(_Period);

  if (settings.getInt(_MDICheck))
  {
    Curve *line = calculate(period, (int) _MDI, data);
    if (! line)
      return 1;

    QString s;
    settings.getData(_MDIPlot, s);
    line->setType((Curve::Type) line->typeFromString(s));

    settings.getData(_MDIColor, s);
    QColor c(s);
    line->setColor(c);

    settings.getData(_MDILabel, s);
    line->setLabel(s);

    line->setZ(0);
    ind.setLine(s, line);
  }

  if (settings.getInt(_PDICheck))
  {
    Curve *line = calculate(period, (int) _PDI, data);
    if (! line)
      return 1;

    QString s;
    settings.getData(_PDIPlot, s);
    line->setType((Curve::Type) line->typeFromString(s));

    settings.getData(_PDIColor, s);
    QColor c(s);
    line->setColor(c);

    settings.getData(_PDILabel, s);
    line->setLabel(s);
    
    line->setZ(1);
    ind.setLine(s, line);
  }

  if (settings.getInt(_ADXCheck))
  {
    Curve *line = calculate(period, (int) _ADX, data);
    if (! line)
      return 1;

    QString s;
    settings.getData(_ADXPlot, s);
    line->setType((Curve::Type) line->typeFromString(s));

    settings.getData(_ADXColor, s);
    QColor c(s);
    line->setColor(c);

    settings.getData(_ADXLabel, s);
    line->setLabel(s);
    
    line->setZ(2);
    ind.setLine(s, line);
  }

  if (settings.getInt(_ADXRCheck))
  {
    Curve *line = calculate(period, (int) _ADXR, data);
    if (! line)
      return 1;

    QString s;
    settings.getData(_ADXRPlot, s);
    line->setType((Curve::Type) line->typeFromString(s));

    settings.getData(_ADXRColor, s);
    QColor c(s);
    line->setColor(c);

    settings.getData(_ADXRLabel, s);
    line->setLabel(s);
    
    line->setZ(3);
    ind.setLine(s, line);
  }

  return 0;
}

int ADX::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,ADX,<METHOD>,<NAME>,<PERIOD>
  //     0       1     2     3       4       5

  if (set.count() != 6)
  {
    qDebug() << "ADX::getCUS: invalid settings count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set.at(3));
  if (method == -1)
  {
    qDebug() << "ADX::getCUS: invalid method" << set.at(3);
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "ADX::getCUS: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ADX::getCUS: invalid period" << set[5];
    return 1;
  }

  Curve *line = calculate(period, method, data);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

IndicatorPluginDialog * ADX::dialog (Indicator &i)
{
  return new ADXDialog(i);
}

void ADX::defaults (Indicator &i)
{
  Setting set;
  set.setData(_ADXColor, "blue");
  set.setData(_ADXRColor, "yellow");
  set.setData(_PDIColor, "green");
  set.setData(_MDIColor, "red");
  set.setData(_ADXPlot, "Line");
  set.setData(_ADXRPlot, "Line");
  set.setData(_PDIPlot, "Line");
  set.setData(_MDIPlot, "Line");
  set.setData(_ADXLabel, "ADX");
  set.setData(_ADXRLabel, "ADXR");
  set.setData(_PDILabel, "+DI");
  set.setData(_MDILabel, "-DI");
  set.setData(_ADXCheck, 1);
  set.setData(_ADXRCheck, 1);
  set.setData(_PDICheck, 1);
  set.setData(_MDICheck, 1);
  set.setData(_Period, 14);
  i.setSettings(set);
}

void ADX::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;

  settings.getData(_ADXLabel, s);
  l.append(s);

  settings.getData(_ADXRLabel, s);
  l.append(s);

  settings.getData(_PDILabel, s);
  l.append(s);

  settings.getData(_MDILabel, s);
  l.append(s);
}

Curve * ADX::calculate (int period, int method, BarData &data)
{
  if (data.count() < period)
    return 0;

  int size = data.count();
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) method)
  {
    case _ADX:
      rc = TA_ADX(0,
                  size - 1,
                  data.getTAData(BarData::High),
                  data.getTAData(BarData::Low),
                  data.getTAData(BarData::Close),
                  period,
                  &outBeg,
                  &outNb,
                  &out[0]);
      break;
    case _ADXR:
      rc = TA_ADXR(0,
                   size - 1,
                   data.getTAData(BarData::High),
                   data.getTAData(BarData::Low),
                   data.getTAData(BarData::Close),
                   period,
                   &outBeg,
                   &outNb,
                   &out[0]);
      break;
    case _PDI:
      rc = TA_PLUS_DI(0,
                      size - 1,
                      data.getTAData(BarData::High),
                      data.getTAData(BarData::Low),
                      data.getTAData(BarData::Close),
                      period,
                      &outBeg,
                      &outNb,
                      &out[0]);
      break;
    case _MDI:
      rc = TA_MINUS_DI(0,
                       size - 1,
                       data.getTAData(BarData::High),
                       data.getTAData(BarData::Low),
                       data.getTAData(BarData::Close),
                       period,
                       &outBeg,
                       &outNb,
                       &out[0]);
      break;
    case _DX:
      rc = TA_DX(0,
                 size - 1,
                 data.getTAData(BarData::High),
                 data.getTAData(BarData::Low),
                 data.getTAData(BarData::Close),
                 period,
                 &outBeg,
                 &outNb,
                 &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "ADX::calculate: TA-Lib error" << rc;
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
  ADX *o = new ADX;
  return ((IndicatorPlugin *) o);
}


