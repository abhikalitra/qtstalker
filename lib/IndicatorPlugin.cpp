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
#include "Config.h"
#include <qfile.h>
#include <qtextstream.h>

IndicatorPlugin::IndicatorPlugin()
{
  output.setAutoDelete(TRUE);
  pluginType = IndicatorPlug;
  saveFlag = FALSE;
  customFlag = FALSE;
  plotType = 1;
  
  PlotLine *pl = new PlotLine;
  lineTypes = pl->getLineTypes();
  delete pl;

  maTypeList = getMATypes();
      
  BarData *it = new BarData;
  inputTypeList = it->getInputFields();
  delete it;
}

IndicatorPlugin::~IndicatorPlugin()
{
}

void IndicatorPlugin::setIndicatorInput (BarData *d)
{
  data = d;
  output.clear();
}

void IndicatorPlugin::setCustomFlag (bool d)
{
  customFlag = d;
}

void IndicatorPlugin::clearOutput ()
{
  output.clear();
}

Setting IndicatorPlugin::loadFile (QString file)
{
  output.clear();

  Setting dict;
  
  QFile f(file);
  if (! f.open(IO_ReadOnly))
  {
    qDebug("IndicatorPlugin:can't read file %s", file.latin1());
    return dict;
  }
  QTextStream stream(&f);
  
  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;
      
    QStringList l = QStringList::split("=", s, FALSE);
    
    if (l.count() < 2)
      continue;
      
    if (l.count() > 2)
    {
      QString k = l[0];
      s = s.remove(0, k.length() + 1);
      dict.setData(k, s);  
    }
    else
      dict.setData(l[0], l[1]);  
  }
  
  f.close();
  
  plotType = dict.getData("plotType").toInt();

  return dict;
}

void IndicatorPlugin::saveFile (QString file, Setting dict)
{
  QFile f(file);
  if (! f.open(IO_WriteOnly))
  {
    qDebug("IndicatorPlugin:can't save file %s", file.latin1());
    return;
  }
  QTextStream stream(&f);
  
  dict.setData("plotType", QString::number(plotType));
  
  QStringList key = dict.getKeyList();
  
  int loop;
  for(loop = 0; loop < (int) key.count(); loop++)
    stream << key[loop] << "=" << dict.getData(key[loop]) << "\n";
  
  f.close();
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

PlotLine * IndicatorPlugin::getInputLine (QString d)
{
  PlotLine *in = 0;
  
  if (d.contains("#"))
    return in;
      
  bool ok;
  d.toInt(&ok, 10);
  if (ok)
    in = customLines->find(d);

  return in;
}

PlotLine * IndicatorPlugin::getEMA (PlotLine *d, int period)
{
  PlotLine *ema = new PlotLine;

  if (period >= (int) d->getSize())
    return ema;

  if (period < 1)
    return ema;

  double smoother = 2.0 / (period + 1);

  double t = 0;
  int loop;
  for (loop = 0; loop < period; loop++)
    t = t + d->getData(loop);

  double yesterday = t / period;
  ema->append(yesterday);

  for (; loop < (int) d->getSize(); loop++)
  {
    double t  = (smoother * (d->getData(loop) - yesterday)) + yesterday;
    yesterday = t;
    ema->append(t);
  }

  return ema;
}

// NEW CODE - SINGLE PASS SMA
PlotLine * IndicatorPlugin::getSMA (PlotLine *d, int period)
{
  PlotLine *sma = new PlotLine;

  int size = d->getSize();

  // weed out degenerate cases
  
  if (period < 1 || period >= size)	// STEVE: should be period > size
    return sma;				// left this way to keep behaviour

  // create the circular buffer and its running total
  
  double *values = new double[period];
  double total = 0.0;
  
  // fill buffer first time around, keeping its running total

  int loop = -1;
  while (++loop < period) {
    double val = d->getData(loop);
    total += val;
    values[loop] = val;
  }

  // buffer filled with first period values, output first sma value
  
  sma->append(total / period);

  // loop over the rest, each time replacing oldest value in buffer
 
  --loop;
  while (++loop < size) 
  {
    int index = loop % period;
    double newval = d->getData(loop);
    
    total += newval;
    total -= values[index];
    values[index] = newval;

    sma->append(total / period);
  }
 
  // clean up 
  
  delete values;
	
  return sma;
}

PlotLine * IndicatorPlugin::getWMA (PlotLine *d, int period)
{
  PlotLine *wma = new PlotLine;

  if (period >= (int) d->getSize())
    return wma;

  if (period < 1)
    return wma;

  int loop;
  for (loop = period - 1; loop < (int) d->getSize(); loop++)
  {
    int loop2;
    int weight;
    int divider;
    double total;
    for (loop2 = period - 1, weight = 1, divider = 0, total = 0; loop2 >= 0; loop2--, weight++)
    {
      total = total + (d->getData(loop - loop2) * weight);
      divider = divider + weight;
    }

    wma->append(total / divider);
  }

  return wma;
}

PlotLine * IndicatorPlugin::getWilderMA (PlotLine *d, int period)
{
  PlotLine *wilderma = new PlotLine;

  if (period >= (int) d->getSize())
    return wilderma;

  if (period < 1)
    return wilderma;

  double t = 0;
  int loop;
  for (loop = 0; loop < period; loop++)
    t = t + d->getData(loop);

  double yesterday = t / period;

  wilderma->append(yesterday);

  for (; loop < (int) d->getSize(); loop++)
  {
    double t  = (yesterday * (period - 1) + d->getData(loop))/period;
    yesterday = t;
    wilderma->append(t);
  }

  return wilderma;
}

QStringList IndicatorPlugin::getMATypes ()
{
  QStringList l;
  l.append(QObject::tr("EMA"));
  l.append(QObject::tr("SMA"));
  l.append(QObject::tr("WMA"));
  l.append(QObject::tr("Wilder"));
  return l;
}

IndicatorPlugin::MAType IndicatorPlugin::getMAType (QString d)
{
  MAType type = SMA;
  
  while (1)
  {
    if (! d.compare(QObject::tr("EMA")))
    {
      type = EMA;
      break;
    }
    
    if (! d.compare(QObject::tr("WMA")))
    {
      type = WMA;
      break;
    }
  
    if (! d.compare(QObject::tr("Wilder")))
      type = Wilder;
      
    break;
  }
  
  return type;
}

PlotLine * IndicatorPlugin::getMA (PlotLine *in, IndicatorPlugin::MAType type, int period)
{
  PlotLine *ma = 0;
  
  switch (type)
  {
    case EMA:  
      ma = getEMA(in, period);
      break;
    case WMA:  
      ma = getWMA(in, period);
      break;
    case Wilder:  
      ma = getWilderMA(in, period);
      break;
    default:
      ma = getSMA(in, period);
      break;
  }

  return ma;  
}

void IndicatorPlugin::setPlotType (int d)
{
  plotType = d;
}

