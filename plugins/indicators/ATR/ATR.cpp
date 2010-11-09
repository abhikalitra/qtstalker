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

#include "ATR.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

ATR::ATR ()
{
  _indicator = "ATR";

  _methodList << "ATR" << "NATR";
}

int ATR::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  int period = settings.getInt(_Period);

  QString s;
  settings.getData(_Method, s);
  int method = _methodList.indexOf(s);

  Curve *line = calculate(period, method, data);
  if (! line)
    return 1;

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

int ATR::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,ATR,<METHOD>,<NAME>,<PERIOD>
  //     0       1     2     3       4       5

  if (set.count() != 6)
  {
    qDebug() << "ATR::getCUS: invalid parm count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set.at(3));
  if (method == -1)
  {
    qDebug() << "ATR::getCUS: invalid method" << set.at(4);
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "ATR::getCUS: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ATR::getCUS: invalid period" << set[5];
    return 1;
  }

  Curve *line = calculate(period, method, data);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

IndicatorPluginDialog * ATR::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  dialog->setHelpFile("ATR.html");

  Setting _settings = i.settings();

  // general tab
  int tab = dialog->addTab(tr("General"));

  QString d;
  _settings.getData(_Method, d);
  dialog->addCombo(tab, _Method, tr("Method"), list(), d);

  dialog->addInt(tab, _Period, tr("Period"), _settings.getInt(_Period), 100000, 1);

  _settings.getData(_Color, d);
  dialog->addColor(tab, _Color, tr("Color"), d);

  _settings.getData(_Plot, d);
  dialog->addPlot(tab, _Plot, tr("Plot"), d);

  _settings.getData(_Label, d);
  dialog->addText(tab, _Label, tr("Label"), d);

  return dialog;
}

void ATR::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Method, "ATR");
  set.setData(_Period, 14);
  set.setData(_Color, "red");
  set.setData(_Plot, "Line");
  set.setData(_Label, _indicator);
  i.setSettings(set);
}

void ATR::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * ATR::calculate (int period, int method, BarData &data)
{
  int size = data.count();

  if (size < period)
    return 0;

  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) method)
  {
    case _ATR:
      rc = TA_ATR(0,
                  size - 1,
                  data.getTAData(BarData::High),
                  data.getTAData(BarData::Low),
                  data.getTAData(BarData::Close),
                  period,
                  &outBeg,
                  &outNb,
                  &out[0]);
      break;
    case _NATR:
      rc = TA_NATR(0,
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
    qDebug() << "ATR::calculate: TA-Lib error" << rc;
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

QStringList & ATR::list ()
{
  return _methodList;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  ATR *o = new ATR;
  return ((IndicatorPlugin *) o);
}


