/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

/* The "SafeZone Stop" indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.173 */

#include "SZ.h"
#include <math.h>
#include <stdio.h>
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

SZ::SZ ()
{
  pluginName = "SZ";
  setDefaults();
  methodList.append(QObject::tr("Long"));
  methodList.append(QObject::tr("Short"));
  helpFile = "sz.html";
}

SZ::~SZ ()
{
}

void SZ::setDefaults ()
{
  color.setNamedColor("white");
  lineType = PlotLine::Line;
  coefficient = 2.5;
  period = 10;
  no_decline_period = 2;
  method = QObject::tr("Long");
  label = pluginName;
}

void SZ::calculate ()
{
  if (period < 1)
    period = 1;

  int display_uptrend = 0;
  int display_dntrend = 0;
  int position = 1;
  if (! method.compare(QObject::tr("Long")))
    position = 1;
  else
    position = 2;
  if (position & 1) // long
    display_uptrend = 1;
  if (position & 2) // short
    display_dntrend = 1;

  PlotLine *sz_uptrend = new PlotLine();
  PlotLine *sz_dntrend = new PlotLine();

  double uptrend_stop = 0;
  double dntrend_stop = 0;

  if (no_decline_period < 0)
    no_decline_period = 0;
  if (no_decline_period > 365)
    no_decline_period = 365;

  double old_uptrend_stops[no_decline_period];
  double old_dntrend_stops[no_decline_period];

  int loop;
  for (loop = 0; loop < no_decline_period; loop++)
  {
    old_uptrend_stops[loop] = 0;
    old_dntrend_stops[loop] = 0;
  }

  int start = period + 1;
  for (loop = start; loop < (int) data->count(); loop++)
  {
    // calculate downside/upside penetration for lookback period
    int lbloop;
    int lbstart = loop - period;
    if (lbstart < 2)
      lbstart = 2;
    double uptrend_noise_avg = 0;
    double uptrend_noise_cnt = 0;
    double dntrend_noise_avg = 0;
    double dntrend_noise_cnt = 0;
    for (lbloop = lbstart; lbloop < loop; lbloop++)
    {
      double lo_curr = data->getLow(lbloop);
      double lo_last = data->getLow(lbloop - 1);
      double hi_curr = data->getHigh(lbloop);
      double hi_last = data->getHigh(lbloop - 1);
      if (lo_last > lo_curr)
      {
	uptrend_noise_avg += lo_last - lo_curr;
	uptrend_noise_cnt++;
      }
      if (hi_last < hi_curr)
      {
	dntrend_noise_avg += hi_curr - hi_last;
	dntrend_noise_cnt++;
      }
    }
    // make *_avg into actual averages
    if (uptrend_noise_cnt > 0)
      uptrend_noise_avg /= uptrend_noise_cnt;
    if (dntrend_noise_cnt > 0)
      dntrend_noise_avg /= dntrend_noise_cnt;

    double lo_last = data->getLow(loop - 1);
    double hi_last = data->getHigh(loop - 1);
    uptrend_stop = lo_last - coefficient * uptrend_noise_avg;
    dntrend_stop = hi_last + coefficient * dntrend_noise_avg;

    double adjusted_uptrend_stop = uptrend_stop;
    double adjusted_dntrend_stop = dntrend_stop;

    int backloop;
    for (backloop = no_decline_period - 1; backloop >= 0; backloop--)
    {
      if (loop - backloop > start)
      {
	if (old_uptrend_stops[backloop] > adjusted_uptrend_stop)
	  adjusted_uptrend_stop = old_uptrend_stops[backloop];
	if (old_dntrend_stops[backloop] < adjusted_dntrend_stop)
	  adjusted_dntrend_stop = old_dntrend_stops[backloop];
      }
      if (backloop > 0)
      {
	old_uptrend_stops[backloop] = old_uptrend_stops[backloop-1];
	old_dntrend_stops[backloop] = old_dntrend_stops[backloop-1];
      }
    }

    old_uptrend_stops[0] = uptrend_stop;
    old_dntrend_stops[0] = dntrend_stop;

    sz_uptrend->append(adjusted_uptrend_stop);
    sz_dntrend->append(adjusted_dntrend_stop);
  }

  if (display_uptrend)
  {
    sz_uptrend->setColor(color);
    sz_uptrend->setType(lineType);
    sz_uptrend->setLabel(QObject::tr("SZ LONG"));
    output->addLine(sz_uptrend);
  }

  if (display_dntrend)
  {
    sz_dntrend->setColor(color);
    sz_dntrend->setType(lineType);
    sz_dntrend->setLabel(QObject::tr("SZ SHORT"));
    output->addLine(sz_dntrend);
  }
}

int SZ::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("SZ Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addComboItem(QObject::tr("Position"), QObject::tr("Parms"), methodList, method);
  dialog->addIntItem(QObject::tr("Lookback Period"), QObject::tr("Parms"), period, 1, 99999999);
  dialog->addIntItem(QObject::tr("No Decline Period"), QObject::tr("Parms"), no_decline_period, 1, 99999999);
  dialog->addFloatItem(QObject::tr("Coefficient"), QObject::tr("Parms"), coefficient, 0, 99999999);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    period = dialog->getInt(QObject::tr("Lookback Period"));
    no_decline_period = dialog->getInt(QObject::tr("No Decline Period"));
    coefficient = dialog->getFloat(QObject::tr("Coefficient"));
    method = dialog->getCombo(QObject::tr("Position"));
    label = dialog->getText(QObject::tr("Label"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void SZ::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void SZ::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void SZ::setIndicatorSettings (Setting dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("period");
  if (s.length())
    period = s.toInt();

  s = dict.getData("noDeclinePeriod");
  if (s.length())
    no_decline_period = s.toInt();

  s = dict.getData("coefficient");
  if (s.length())
    coefficient = s.toFloat();

  s = dict.getData("method");
  if (s.length())
    method = s;

  s = dict.getData("label");
  if (s.length())
    label = s;
}

Setting SZ::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("noDeclinePeriod", QString::number(no_decline_period));
  dict.setData("coefficient", QString::number(coefficient));
  dict.setData("method", method);
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * SZ::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output->getLine(0);
}

int SZ::getMinBars ()
{
  int t = minBars + period + no_decline_period;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  SZ *o = new SZ;
  return ((IndicatorPlugin *) o);
}


