/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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
  paintBars.setAutoDelete(TRUE);
  pluginType = "Indicator";
}

IndicatorPlugin::~IndicatorPlugin()
{
}

void IndicatorPlugin::setIndicatorInput (QList<Setting> *d)
{
  data = d;
  output.clear();
}

QMemArray<int> IndicatorPlugin::getAlerts ()
{
  alerts.fill(0, data->count());
  return alerts;
}

QList<QColor> IndicatorPlugin::getColorBars (QString uc, QString dc, QString nc)
{
  paintBars.clear();
  
  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    switch(alerts[loop])
    {
      case -1:
        paintBars.append(new QColor(dc));
        break;
      case 1:
        paintBars.append(new QColor(uc));
        break;
      default:
        paintBars.append(new QColor(nc));
        break;
    }
  }

  return paintBars;
}

PlotLine * IndicatorPlugin::getInput (QString field)
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

    if (! field.compare(tr("Average Price")))
    {
      return getAP();
      break;
    }

    if (! field.compare(tr("Typical Price")))
    {
      return getTP();
      break;
    }

    if (! field.compare(tr("Weighted Price")))
    {
      return getWP();
      break;
    }

    if (! field.compare(tr("HL Price")))
    {
      return getHL();
      break;
    }

    if (! field.compare(tr("OC Price")))
    {
      return getOC();
      break;
    }

    break;
  }

  PlotLine *in = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    Setting *set = data->at(loop);

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

PlotLine * IndicatorPlugin::getMA (PlotLine *d, QString type, int period)
{
  return getMA(d, type, period, 0);
}

PlotLine * IndicatorPlugin::getMA (PlotLine *d, QString type, int period, int displace)
{
  PlotLine *ma = 0;
  if (! type.compare(tr("SMA")))
    ma = getSMA(d, period);
  else
  {
    if (! type.compare(tr("EMA")))
      ma = getEMA(d, period);
    else
    {
      if (! type.compare(tr("WMA")))
        ma = getWMA(d, period);
      else
        ma = getWilderMA(d, period);
    }
  }

  if (displace > 0)
  {
    PlotLine *dma = new PlotLine;
    int loop;
    for (loop = 0; loop < ma->getSize() - displace; loop++)
      dma->append(ma->getData(loop));
    delete ma;
    return dma;
  }
  else
    return ma;
}

PlotLine * IndicatorPlugin::getEMA (PlotLine *data, int period)
{
  PlotLine *ema = new PlotLine;

  if (period >= (int) data->getSize())
    return ema;

  if (period < 1)
    return ema;

  double smoother = 2.0 / (period + 1);

  int count;
  double t = 0;
  int loop = period - 1;
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

PlotLine * IndicatorPlugin::getSMA (PlotLine *data, int period)
{
  PlotLine *sma = new PlotLine;

  if (period >= (int) data->getSize())
    return sma;

  if (period < 1)
    return sma;

  int loop;
  for (loop = period - 1; loop < (int) data->getSize(); loop++)
  {
    double total = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
      total = total + data->getData(loop - loop2);

    sma->append(total / period);
  }

  return sma;
}

PlotLine * IndicatorPlugin::getWMA (PlotLine *data, int period)
{
  PlotLine *wma = new PlotLine;

  if (period >= (int) data->getSize())
    return wma;

  if (period < 1)
    return wma;

  int loop;
  for (loop = period - 1; loop < (int) data->getSize(); loop++)
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

PlotLine * IndicatorPlugin::getWilderMA (PlotLine *data, int period)
{
  PlotLine *wilderma = new PlotLine;

  if (period >= (int) data->getSize())
    return wilderma;

  if (period < 1)
    return wilderma;

  double t = 0;
  int loop;
  for (loop = 0; loop < period; loop++)
    t = t + data->getData(loop);

  double yesterday = t / period;

  wilderma->append(yesterday);

  for (; loop < (int) data->getSize(); loop++)
  {
    double t  = (yesterday * (period - 1) + data->getData(loop))/period;
    yesterday = t;
    wilderma->append(t);
  }

  return wilderma;
}

PlotLine * IndicatorPlugin::getTR ()
{
  PlotLine *tr = new PlotLine;

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    Setting *set = data->at(loop);
    double high = set->getFloat("High");
    double low = set->getFloat("Low");
    double close;
    if (loop > 0)
    {
      set = data->at(loop - 1);
      close = set->getFloat("Close");
    }
    else
      close = high;

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

PlotLine * IndicatorPlugin::getTP ()
{
  PlotLine *tp = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    Setting *set = data->at(loop);
    tp->append((set->getFloat("High") + set->getFloat("Low") + set->getFloat("Close")) / 3);
  }

  return tp;
}

PlotLine * IndicatorPlugin::getAP ()
{
  PlotLine *ap = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    Setting *set = data->at(loop);
    ap->append((set->getFloat("Open") + set->getFloat("High") + set->getFloat("Low") + set->getFloat("Close")) / 4);
  }

  return ap;
}

PlotLine * IndicatorPlugin::getWP ()
{
  PlotLine *wp = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    Setting *set = data->at(loop);
    wp->append((set->getFloat("High") + set->getFloat("Low") + set->getFloat("Close") + set->getFloat("Close")) / 4);
  }

  return wp;
}

PlotLine * IndicatorPlugin::getHL ()
{
  PlotLine *hl = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    Setting *set = data->at(loop);
    hl->append((set->getFloat("High") + set->getFloat("Low")) / 2);
  }

  return hl;
}

PlotLine * IndicatorPlugin::getOC ()
{
  PlotLine *oc = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    Setting *set = data->at(loop);
    oc->append((set->getFloat("Open") + set->getFloat("Close")) / 2);
  }

  return oc;
}

int IndicatorPlugin::getIndicatorLines ()
{
  return (int) output.count();
}

PlotLine * IndicatorPlugin::getIndicatorLine (int d)
{
  PlotLine *line = output.at(d);
  if (line)
    return line;
  else
    return 0;
}

void IndicatorPlugin::clearOutput ()
{
  output.clear();
}


