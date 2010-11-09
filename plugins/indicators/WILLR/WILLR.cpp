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

#include "WILLR.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

WILLR::WILLR ()
{
  _indicator = "WILLR";
}

int WILLR::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  int period = settings.getInt(_Period);

  Curve *line = calculate(period, data);
  if (! line)
    return 1;

  QString s;
  settings.getData(_Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(_Label, s);
  line->setLabel(s);
  
  line->setZ(0);
  ind.setLine(s, line);

  return 0;
}

int WILLR::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,WILLR,<NAME>,<PERIOD>
  //     0       1      2     3       4

  if (set.count() != 5)
  {
    qDebug() << "WILLR::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "WILLR::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "WILLR::getCUS: invalid period settings" << set[4];
    return 1;
  }

  Curve *line = calculate(period, data);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * WILLR::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  dialog->setHelpFile("WILLR.html");

  Setting _settings = i.settings();

  // general tab
  int tab = dialog->addTab(tr("General"));

  QString d;
  dialog->addInt(tab, _Period, tr("Period"), _settings.getInt(_Period), 100000, 1);

  _settings.getData(_Color, d);
  dialog->addColor(tab, _Color, tr("Color"), d);

  _settings.getData(_Plot, d);
  dialog->addPlot(tab, _Plot, tr("Plot"), d);

  _settings.getData(_Label, d);
  dialog->addText(tab, _Label, tr("Label"), d);

  return dialog;
}

void WILLR::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Plot, "Line");
  set.setData(_Label, _indicator);
  set.setData(_Period, 14);
  i.setSettings(set);
}

void WILLR::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * WILLR::calculate (int period, BarData &data)
{
  int size = data.count();

  if (size < period)
    return 0;

  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_WILLR(0,
                           size - 1,
                           data.getTAData(BarData::High),
                           data.getTAData(BarData::Low),
                           data.getTAData(BarData::Close),
                           period,
                           &outBeg,
                           &outNb,
                           &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "WILLR::calculate: TA-Lib error" << rc;
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
  WILLR *o = new WILLR;
  return ((IndicatorPlugin *) o);
}

