/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "IndicatorPlugin.h"
#include <math.h>

IndicatorPlugin::IndicatorPlugin()
{
  output.setAutoDelete(TRUE);
  pluginType = "Indicator";
}

IndicatorPlugin::~IndicatorPlugin()
{
}

void IndicatorPlugin::setIndicatorInput (QList<Setting> d)
{
  data = d;
}

QMemArray<int> IndicatorPlugin::getAlerts ()
{
  return alerts;
}

Output * IndicatorPlugin::getInput (QString field)
{
  int f = -1;
  while (1)
  {
    if (! field.compare(tr("Open")))
    {
      f = 0;
      break;
    }

    if (! field.compare(tr("High")))
    {
      f = 1;
      break;
    }

    if (! field.compare(tr("Low")))
    {
      f = 2;
      break;
    }

    if (! field.compare(tr("Volume")))
    {
      f = 3;
      break;
    }

    if (! field.compare(tr("Open Interest")))
    {
      f = 4;
      break;
    }

    break;
  }

  Output *in = new Output();

  int loop;
  for (loop = 0; loop < (int) data.count(); loop++)
  {
    Setting *set = data.at(loop);

    switch(f)
    {
      case 0:
        in->append(set->getFloat("Open"));
	break;
      case 1:
        in->append(set->getFloat("High"));
	break;
      case 2:
        in->append(set->getFloat("Low"));
	break;
      case 3:
        in->append(set->getFloat("Volume"));
	break;
      case 4:
        in->append(set->getFloat("Open Interest"));
	break;
      default:
        in->append(set->getFloat("Close"));
        break;
    }
  }

  return in;
}

Output * IndicatorPlugin::getMA (Output *d, QString type, int period)
{
  Output *ma;
  if (! type.compare(tr("SMA")))
    ma = getSMA(d, period);
  else
  {
    if (! type.compare(tr("EMA")))
      ma = getEMA(d, period);
    else
      ma = getWMA(d, period);
  }

  return ma;
}

Output * IndicatorPlugin::getEMA (Output *data, int period)
{
  Output *ema = new Output;

  if (period >= (int) data->getSize())
    return ema;

  if (period < 1)
    return ema;

  double smoother = 2.0 / (period + 1);

  int count;
  double t = 0;
  int loop = period;
  for (count = 0; count < period; count++)
    t = t + data->getData(loop - count);

  double yesterday = t / period;

  for (; loop < (int) data->getSize(); loop++)
  {
    double t  = (smoother * (data->getData(loop) - yesterday)) + yesterday;
    yesterday = t;
    ema->append(t);
  }

  return ema;
}

Output * IndicatorPlugin::getSMA (Output *data, int period)
{
  Output *sma = new Output;

  if (period >= (int) data->getSize())
    return sma;

  if (period < 1)
    return sma;

  int loop;
  for (loop = period; loop < (int) data->getSize(); loop++)
  {
    double total = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
      total = total + data->getData(loop - loop2);

    sma->append(total / period);
  }

  return sma;
}

Output * IndicatorPlugin::getWMA (Output *data, int period)
{
  Output *wma = new Output;

  if (period >= (int) data->getSize())
    return wma;

  if (period < 1)
    return wma;

  int loop;
  for (loop = period; loop < (int) data->getSize(); loop++)
  {
    int loop2;
    int weight;
    int divider;
    double total;
    for (loop2 = period, weight = 1, divider = 0, total = 0; loop2 >= 0; loop2--, weight++)
    {
      total = total + (data->getData(loop - loop2) * weight);
      divider = divider + weight;
    }

    wma->append(total / divider);
  }

  return wma;
}

Output * IndicatorPlugin::getTR ()
{
  Output *tr = new Output;

  int loop;
  for (loop = 1; loop < (int) data.count(); loop++)
  {
    Setting *set = data.at(loop);
    double high = set->getFloat("High");
    double low = set->getFloat("Low");
    set = data.at(loop - 1);
    double close = set->getFloat("Close");

    double t = high - low;

    double t2 = fabs(high - close);
    if (t2 > t)
      t = t2;

    t2 = fabs(low - close);
    if (t2 > t)
      t = t2;

    tr->append(t);
  }

  return tr;
}

Output * IndicatorPlugin::getTP ()
{
  Output *tp = new Output();

  int loop;
  for (loop = 0; loop < (int) data.count(); loop++)
  {
    Setting *set = data.at(loop);
    tp->append((set->getFloat("High") + set->getFloat("Low") + set->getFloat("Close")) / 3);
  }

  return tp;
}

int IndicatorPlugin::getIndicatorLines ()
{
  return (int) output.count();
}

Setting * IndicatorPlugin::getIndicatorLineSettings (int d)
{
  Output *line = output.at(d);

  Setting *set = new Setting;
  set->set("Color", line->getColor(), Setting::None);
  set->set("Line Type", line->getType(), Setting::None);
  set->set("Label", line->getLabel(), Setting::None);
  return set;
}

QMemArray<double> IndicatorPlugin::getIndicatorLineArray (int d)
{
  Output *line = output.at(d);

  QMemArray<double> a(line->getSize());
  int loop;
  for (loop = 0; loop < (int) a.size(); loop++)
    a[loop] = line->getData(loop);

  return a;
}

//******************************************************************************
//*********************** OUTPUT ***********************************************
//******************************************************************************

Output::Output ()
{
  data.setAutoDelete(TRUE);
  color = QObject::tr("Color");
  lineType = QObject::tr("Line Type");
  label = QObject::tr("Label");
}

Output::~Output ()
{
}

void Output::setColor (QString d)
{
  color = d;
}

QString Output::getColor ()
{
  return color;
}

void Output::setType (QString d)
{
  lineType = d;
}

QString Output::getType ()
{
  return lineType;
}

void Output::setLabel (QString d)
{
  label = d;
}

QString Output::getLabel ()
{
  return label;
}

void Output::append (double d)
{
  Val *r = new Val;
  r->v  = d;
  data.append(r);
}

void Output::prepend (double d)
{
  Val *r = new Val;
  r->v = d;
  data.prepend(r);
}

double Output::getData (int d)
{
  Val *r = data.at(d);
  return r->v;
}

void Output::setData (int i, double d)
{
  Val *r = data.at(i);
  r->v = d;
}

int Output::getSize ()
{
  return (int) data.count();
}

