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
#include <qfile.h>
#include <qtextstream.h>

IndicatorPlugin::IndicatorPlugin()
{
  output.setAutoDelete(TRUE);
  paintBars.setAutoDelete(TRUE);
  pluginType = IndicatorPlug;
  saveFlag = FALSE;
  plotFlag = FALSE;
  alertFlag = FALSE;
  
  PlotLine *pl = new PlotLine;
  lineTypes = pl->getLineTypes();
  delete pl;
}

IndicatorPlugin::~IndicatorPlugin()
{
}

void IndicatorPlugin::setIndicatorInput (BarData *d)
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

QStringList IndicatorPlugin::getMATypes ()
{
  QStringList l;
  l.append(QObject::tr("EMA"));
  l.append(QObject::tr("SMA"));
  l.append(QObject::tr("WMA"));
  l.append(QObject::tr("Wilder"));
  return l;
}

QStringList IndicatorPlugin::getInputFields ()
{
  QStringList l;
  l.append(QObject::tr("Open"));
  l.append(QObject::tr("High"));
  l.append(QObject::tr("Low"));
  l.append(QObject::tr("Close"));
  l.append(QObject::tr("Volume"));
  l.append(QObject::tr("Open Interest"));
  l.append(QObject::tr("Average Price"));
  l.append(QObject::tr("Typical Price"));
  l.append(QObject::tr("Weighted Price"));
  l.append(QObject::tr("HL Price"));
  l.append(QObject::tr("OC Price"));
  return l;
}

bool IndicatorPlugin::getPlotFlag ()
{
  return plotFlag;
}

bool IndicatorPlugin::getAlertFlag ()
{
  return alertFlag;
}

void IndicatorPlugin::clearOutput ()
{
  output.clear();
}

QDict<QString> IndicatorPlugin::loadFile (QString file)
{
  output.clear();

  QDict<QString>dict;
  dict.setAutoDelete(TRUE);
  
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
    if (l.count() != 2)
      continue;

    dict.replace(l[0], new QString(l[1]));      
  }
  
  f.close();
  
  return dict;
}

void IndicatorPlugin::saveFile (QString file, QDict<QString> dict)
{
  QFile f(file);
  if (! f.open(IO_WriteOnly))
  {
    qDebug("IndicatorPlugin:can't save file %s", file.latin1());
    return;
  }
  QTextStream stream(&f);
  
  QDictIterator<QString> it(dict);
  for(; it.current(); ++it)
  {
    QString *s = it.current();
    stream << it.currentKey() << "=" << s->left(s->length()) << "\n";
  }
  
  f.close();
}

PlotLine * IndicatorPlugin::getInput (IndicatorPlugin::InputType field)
{
  switch(field)
  {
    case AveragePrice:
      return getAP();
      break;
    case TypicalPrice:
      return getTP();
      break;
    case WeightedPrice:
      return getWP();
      break;
    case HLPrice:
      return getHL();
      break;
    case OCPrice:
      return getOC();
      break;
    default:
      break;
  }

  PlotLine *in = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    switch(field)
    {
      case Open:
        in->append(data->getOpen(loop));
	break;
      case High:
        in->append(data->getHigh(loop));
	break;
      case Low:
        in->append(data->getLow(loop));
	break;
      case Volume:
        in->append(data->getVolume(loop));
	break;
      case OpenInterest:
        in->append(data->getOI(loop));
	break;
      default:
        in->append(data->getClose(loop));
        break;
    }
  }

  return in;
}

PlotLine * IndicatorPlugin::getMA (PlotLine *d, MAType type, int period)
{
  return getMA(d, type, period, 0);
}

PlotLine * IndicatorPlugin::getMA (PlotLine *d, MAType type, int period, int displace)
{
  PlotLine *ma = 0;
  
  switch (type)
  {
    case SMA:
      ma = getSMA(d, period);
      break;
    case EMA:
      ma = getEMA(d, period);
      break;
    case WMA:
      ma = getWMA(d, period);
      break;
    case Wilder:
      ma = getWilderMA(d, period);
      break;
    default:
      break;
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
    double high = data->getHigh(loop);
    double low = data->getLow(loop);
    double close;
    if (loop > 0)
      close = data->getClose(loop - 1);
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
    tp->append((data->getHigh(loop) + data->getLow(loop) + data->getClose(loop)) / 3);

  return tp;
}

PlotLine * IndicatorPlugin::getAP ()
{
  PlotLine *ap = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
    ap->append((data->getOpen(loop) + data->getHigh(loop) + data->getLow(loop) + data->getClose(loop)) / 4);

  return ap;
}

PlotLine * IndicatorPlugin::getWP ()
{
  PlotLine *wp = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
    wp->append((data->getHigh(loop) + data->getLow(loop) + data->getClose(loop) + data->getClose(loop)) / 4);

  return wp;
}

PlotLine * IndicatorPlugin::getHL ()
{
  PlotLine *hl = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
    hl->append((data->getHigh(loop) + data->getLow(loop)) / 2);

  return hl;
}

PlotLine * IndicatorPlugin::getOC ()
{
  PlotLine *oc = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
    oc->append((data->getOpen(loop) + data->getClose(loop)) / 2);

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



