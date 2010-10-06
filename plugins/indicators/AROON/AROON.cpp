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

#include "AROON.h"
#include "Curve.h"
#include "AROONDialog.h"
#include "ta_libc.h"

#include <QtDebug>

AROON::AROON ()
{
  _indicator = "AROON";

  _methodList << "AROON" << "OSC";
}

int AROON::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QString s;
  settings.getData(_Method, s);
  int method = _methodList.indexOf(s);

  int period = settings.getInt(_Period);

  switch ((AROON::Method) method)
  {
    case AROON::_AROONOSC:
    {
      Curve *line = getAROONOSC(period, data);
      if (! line)
        return 1;

      settings.getData(_OSCPlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(_OSCColor, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(_OSCLabel, s);
      line->setLabel(s);

      line->setZ(0);
      ind.setLine(s, line);
      break;
    }
    default:
    {
      QList<Curve *> pl;
      if (getAROON(period, pl, data))
        return 1;

      Curve *line = pl.at(0);

      settings.getData(_UpPlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(_UpColor, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(_UpLabel, s);
      line->setLabel(s);
      
      line->setZ(1);
      ind.setLine(s, line);

      line = pl.at(1);

      settings.getData(_DownPlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(_DownColor, s);
      c.setNamedColor(s);
      line->setColor(c);

      settings.getData(_DownLabel, s);
      line->setLabel(s);
      
      line->setZ(2);
      ind.setLine(s, line);
      break;
    }
  }

  return 0;
}

int AROON::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,AROON,<METHOD>,*
  //     0       1      2      3

  int method = _methodList.indexOf(set[3]);

  int rc = 1;
  switch ((Method) method)
  {
    case _AROON:
      rc = scriptAROON(set, ind, data);
      break;
    case _AROONOSC:
      rc = scriptAROONOSC(set, ind, data);
      break;
    default:
      break;
  }

  return rc;
}

IndicatorPluginDialog * AROON::dialog (Indicator &i)
{
  return new AROONDialog(i);
}

void AROON::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Method, "AROON");
  set.setData(_DownColor, "red");
  set.setData(_UpColor, "green");
  set.setData(_DownPlot, "Line");
  set.setData(_UpPlot, "Line");
  set.setData(_DownLabel, "AROOND");
  set.setData(_UpLabel, "AROONU");
  set.setData(_OSCColor, "red");
  set.setData(_OSCPlot, "Histogram Bar");
  set.setData(_OSCLabel, "AROONOSC");
  set.setData(_Period, 14);
  i.setSettings(set);
}

void AROON::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();

  QString s;
  settings.getData(_Method, s);
  int method = _methodList.indexOf(s);

  switch ((AROON::Method) method)
  {
    case AROON::_AROONOSC:
      settings.getData(_OSCLabel, s);
      l.append(s);
      break;
    default:
      settings.getData(_UpLabel, s);
      l.append(s);
      settings.getData(_DownLabel, s);
      l.append(s);
      break;
  }
}

int AROON::scriptAROON (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,AROON,AROON,<UPPER NAME>,<LOWER NAME>,<PERIOD>
  //     0       1      2     3         4           5          6

  if (set.count() != 7)
  {
    qDebug() << "AROON::scriptAROON: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "AROON::scriptAROON: duplicate upper name" << set[4];
    return 1;
  }

  tl = ind.line(set[5]);
  if (tl)
  {
    qDebug() << "AROON::scriptAROON: duplicate lower name" << set[5];
    return 1;
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "AROON::scriptAROON: invalid period" << set[6];
    return 1;
  }

  QList<Curve *> pl;
  if (getAROON(period, pl, data))
    return 1;

  pl.at(0)->setLabel(set[4]);
  pl.at(1)->setLabel(set[5]);

  ind.setLine(set[4], pl.at(0));
  ind.setLine(set[5], pl.at(1));

  return 0;
}

int AROON::scriptAROONOSC (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,AROON,AROONOSC,<NAME>,<PERIOD>
  //     0       1      2       3       4      5

  if (set.count() != 6)
  {
    qDebug() << "AROONOSC::scriptAROONOSC: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "AROONOSC::scriptAROONOSC: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "AROONOSC::scriptAROONOSC: invalid period" << set[5];
    return 1;
  }

  Curve *line = getAROONOSC(period, data);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}


int AROON::getAROON (int period, QList<Curve *> &pl, BarData &data)
{
  int size = data.count();
  if (size < period)
    return 1;

  TA_Real dout[size];
  TA_Real uout[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_AROON(0,
                           size - 1,
                           data.getTAData(BarData::High),
                           data.getTAData(BarData::Low),
                           period,
                           &outBeg,
                           &outNb,
                           &dout[0],
                           &uout[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "AROON::getAROON: TA-Lib error" << rc;
    return 1;
  }

  Curve *upper = new Curve;
  Curve *lower = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    upper->setBar(dataLoop, new CurveBar(uout[outLoop]));
    lower->setBar(dataLoop, new CurveBar(dout[outLoop]));

    dataLoop--;
    outLoop--;
  }

  pl.append(upper);
  pl.append(lower);

  return 0;
}

Curve * AROON::getAROONOSC (int period, BarData &data)
{
  int size = data.count();

  if (size < period)
    return 0;

  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_AROONOSC(0,
                              size - 1,
                              data.getTAData(BarData::High),
                              data.getTAData(BarData::Low),
                              period,
                              &outBeg,
                              &outNb,
                              &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "AROON::getAROONOSC: TA-Lib error" << rc;
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

QStringList & AROON::list ()
{
  return _methodList;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  AROON *o = new AROON;
  return ((IndicatorPlugin *) o);
}


