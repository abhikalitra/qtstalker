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

#include "AD.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

AD::AD ()
{
  _indicator = "AD";
  
  _methodList << "AD" << "ADOSC";
}

int AD::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QString s;
  settings.getData(_Method, s);
  int method = _methodList.indexOf(s);

  switch ((AD::Method) method)
  {
    case AD::_ADOSC:
    {
      int fp = settings.getInt(_FastPeriod);
      int sp = settings.getInt(_SlowPeriod);

      Curve *line = getADOSC(fp, sp, data);
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
      Curve *line = getAD(data);
      if (! line)
	return 1;
      
      settings.getData(_ADPlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(_ADColor, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(_ADLabel, s);
      line->setLabel(s);

      line->setZ(0);
      ind.setLine(s, line);
      break;
    }
  }

  return 0;
}

int AD::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,AD,<METHOD>,*
  //      0      1     2    3

  int method = _methodList.indexOf(set[3]);

  int rc = 1;
  switch ((Method) method)
  {
    case _AD:
      rc = CUSAD(set, ind, data);
      break;
    case _ADOSC:
      rc = CUSADOSC(set, ind, data);
      break;
    default:
      break;
  }

  return rc;
}

IndicatorPluginDialog * AD::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  dialog->setHelpFile("AD.html");

  Setting _settings = i.settings();

  // general page
  int tab = dialog->addTab(tr("General"));
  
  QString d;
  _settings.getData(_Method, d);
  dialog->addCombo(tab, _Method, tr("Method"), list(), d);

  // AD page
  tab = dialog->addTab("AD");
  
  _settings.getData(_ADColor, d);
  dialog->addColor(tab, _ADColor, tr("Color"), d);

  _settings.getData(_ADPlot, d);
  dialog->addPlot(tab, _ADPlot, tr("Plot"), d);

  _settings.getData(_ADLabel, d);
  dialog->addText(tab, _ADLabel, tr("Label"), d);
  
  // OSC page
  tab = dialog->addTab("OSC");

  _settings.getData(AD::_OSCColor, d);
  dialog->addColor(tab, _OSCColor, tr("Color"), d);

  _settings.getData(_OSCPlot, d);
  dialog->addPlot(tab, _OSCPlot, tr("Plot"), d);

  _settings.getData(_OSCLabel, d);
  dialog->addText(tab, _OSCLabel, tr("Label"), d);

  dialog->addInt(tab, _FastPeriod, tr("Fast Period"), _settings.getInt(_FastPeriod), 100000, 1);

  dialog->addInt(tab, _SlowPeriod, tr("Slow Period"), _settings.getInt(_SlowPeriod), 100000, 1);
  
  return dialog;
}

void AD::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Method, QString("AD"));
  set.setData(_ADColor, QString("red"));
  set.setData(_ADPlot, QString("Line"));
  set.setData(_ADLabel, QString("AD"));
  set.setData(_FastPeriod, 3);
  set.setData(_SlowPeriod, 10);
  set.setData(_OSCColor, QString("red"));
  set.setData(_OSCPlot, QString("Histogram Bar"));
  set.setData(_OSCLabel, QString("ADOSC"));
  i.setSettings(set);
}

void AD::plotNames (Indicator &i, QStringList &l)
{
  l.clear();
  
  Setting settings = i.settings();

  QString s;
  settings.getData(_Method, s);
  int method = _methodList.indexOf(s);

  switch ((AD::Method) method)
  {
    case AD::_ADOSC:
    {
      settings.getData(_OSCLabel, s);
      l.append(s);
      break;
    }
    default:
    {
      settings.getData(_ADLabel, s);
      l.append(s);
      break;
    }
  }
}

int AD::CUSAD (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,AD,AD,<NAME>
  //      0      1    2  3    4

  if (set.count() != 5)
  {
    qDebug() << "AD::CUSAD: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set.at(4));
  if (tl)
  {
    qDebug() << "AD::CUSAD: duplicate name" << set.at(4);
    return 1;
  }

  Curve *line = getAD(data);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set.at(4), line);

  return 0;
}

int AD::CUSADOSC (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,AD,ADOSC,<NAME>,<FAST_PERIOD>,<SLOW_PERIOD>
  //      0      1    2    3      4        5             6

  if (set.count() != 7)
  {
    qDebug() <<  "AD::CUSADOSC: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set.at(4));
  if (tl)
  {
    qDebug() << "AD::CUSADOSC: duplicate name" << set.at(4);
    return 1;
  }

  bool ok;
  int fast = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "AD::CUSADOSC: invalid fast period" << set[5];
    return 1;
  }

  int slow = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "AD::CUSADOSC: invalid slow period" << set[6];
    return 1;
  }

  Curve *line = getADOSC(fast, slow, data);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set.at(4), line);

  return 0;
}

Curve * AD::getAD (BarData &data)
{
  if (data.count() < 1)
    return 0;

  TA_Real out[data.count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_AD(0,
                        data.count() - 1,
                        data.getTAData(BarData::High),
                        data.getTAData(BarData::Low),
                        data.getTAData(BarData::Close),
                        data.getTAData(BarData::Volume),
                        &outBeg,
                        &outNb,
                        &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionAD::getAD: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int dataLoop = data.count() - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  return line;
}

Curve * AD::getADOSC (int fast, int slow, BarData &data)
{
  if (data.count() < 1)
    return 0;

  TA_Real out[data.count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_ADOSC(0,
                           data.count() - 1,
                           data.getTAData(BarData::High),
                           data.getTAData(BarData::Low),
                           data.getTAData(BarData::Close),
                           data.getTAData(BarData::Volume),
                           fast,
                           slow,
                           &outBeg,
                           &outNb,
                           &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionAD::getADOSC: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int dataLoop = data.count() - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  return line;
}

QStringList & AD::list ()
{
  return _methodList;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  AD *o = new AD;
  return ((IndicatorPlugin *) o);
}

