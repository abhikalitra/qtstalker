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

#include "QSMath.h"
#include <math.h>
#include <qobject.h>

QSMath::QSMath()
{
  data = 0;
  customLines.setAutoDelete(FALSE);
}

QSMath::QSMath(BarData *d)
{
  data = d;
}

QSMath::~QSMath()
{
}

QStringList QSMath::getMATypes ()
{
  QStringList l;
  l.append(QObject::tr("EMA"));
  l.append(QObject::tr("SMA"));
  l.append(QObject::tr("WMA"));
  l.append(QObject::tr("Wilder"));
  return l;
}

QStringList QSMath::getOperatorTypes ()
{
  QStringList l;
  l.append("==");
  l.append("<");
  l.append("<=");
  l.append(">");
  l.append(">=");
  return l;
}

QSMath::MAType QSMath::getMAType (QString d)
{
  MAType type = SMA;
  
  while (1)
  {
    if (! d.compare(QObject::tr("EMA")))
    {
      type = EMA;
      break;
    }
    
    if (! d.compare(QObject::tr("SMA")))
    {
      type = SMA;
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

QSMath::Operator QSMath::getOperator (QString d)
{
  Operator op = NoOp;
  
  while (1)
  {
    if (! d.compare("=="))
    {
      op = Equal;
      break;
    }
    
    if (! d.compare("<"))
    {
      op = LessThan;
      break;
    }
  
    if (! d.compare("<="))
    {
      op = LessThanEqual;
      break;
    }
      
    if (! d.compare(">"))
    {
      op = GreaterThan;
      break;
    }
    
    if (! d.compare(">="))
      op = GreaterThanEqual;
    
    break;
  }
  
  return op;
}

PlotLine * QSMath::getMA (PlotLine *d, MAType type, int period)
{
  return getMA(d, type, period, 0);
}

PlotLine * QSMath::getMA (PlotLine *d, MAType type, int period, int displace)
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

//********************************************************************

PlotLine * QSMath::getEMA (PlotLine *d, int period)
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

QString QSMath::getMA2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 7)
  {
    rc = QObject::tr("MA: missing field");
    return rc;
  }

  MAType type = getMAType(l[1]);
    
  PlotLine *input = getInputLine(l[2]);
  if (! input)
  {
    rc = QObject::tr("MA: invalid input field");
    return rc;
  }

  int period = l[3].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("MA: invalid period field");
    return rc;
  }
  
  PlotLine *output = getMA(input, type, period);
  
  output->setColor(l[4]);
  output->setType(l[5]);
  output->setLabel(l[6]);
  
  customLines.replace(QString::number(i), output);

  l[2].toInt(&ok, 10);
  if (! ok)
    delete input;
    
  return rc;
}

//********************************************************************

// ORIGINAL SMA CODE
/*
PlotLine * QSMath::getSMA (PlotLine *d, int period)
{
  PlotLine *sma = new PlotLine;

  if (period >= (int) d->getSize())
    return sma;

  if (period < 1)
    return sma;

  int loop;
  for (loop = period - 1; loop < (int) d->getSize(); loop++)
  {
    double total = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
      total = total + d->getData(loop - loop2);

    sma->append(total / period);
  }

  return sma;
}
*/

// NEW CODE - SINGLE PASS SMA
PlotLine * QSMath::getSMA (PlotLine *d, int period)
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

//********************************************************************

PlotLine * QSMath::getWMA (PlotLine *d, int period)
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

//********************************************************************

PlotLine * QSMath::getWilderMA (PlotLine *d, int period)
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

//********************************************************************

PlotLine * QSMath::getTR ()
{
  PlotLine *tr = new PlotLine;
  if (! data)
    return tr;

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

QString QSMath::getTR2 (int i, QStringList l)
{
  QString rc;
  
  if (l.count() != 4)
  {
    rc = QObject::tr("TR: missing field");
    return rc;
  }
  
  PlotLine *output = getTR();
  
  output->setColor(l[1]);
  output->setType(l[2]);
  output->setLabel(l[3]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getTP ()
{
  PlotLine *tp = new PlotLine();
  if (! data)
    return tp;

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
    tp->append((data->getHigh(loop) + data->getLow(loop) + data->getClose(loop)) / 3);

  return tp;
}

QString QSMath::getTP2 (int i, QStringList l)
{
  QString rc;
  
  if (l.count() != 4)
  {
    rc = QObject::tr("TP: missing field");
    return rc;
  }
  
  PlotLine *output = getTP();
  
  output->setColor(l[1]);
  output->setType(l[2]);
  output->setLabel(l[3]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getAD ()
{
  PlotLine *line = new PlotLine();
  if (! data)
    return line;
  
  int loop;
  double accum = 0;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    double volume = data->getVolume(loop);
    if (volume > 0)
    {
      double high = data->getHigh(loop);
      double low = data->getLow(loop);

      double t = high - low;

      if (t != 0)
      {
        double close = data->getClose(loop);
        double t2 = (close - low) - (high - close);
        accum = accum + ((t2 / t) * volume);
      }
    }

    line->append(accum);
  }
  
  return line;
}

QString QSMath::getAD2 (int i, QStringList l)
{
  QString rc;
  
  if (l.count() != 4)
  {
    rc = QObject::tr("AD: missing field");
    return rc;
  }
  
  PlotLine *output = getAD();
  
  output->setColor(l[1]);
  output->setType(l[2]);
  output->setLabel(l[3]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getBB (MAType maType, int period, double deviation, int flag)
{
  PlotLine *bb = new PlotLine();
  if (! data)
    return bb;

  PlotLine *in = getTP();

  PlotLine *sma = getMA(in, maType, period);
  int smaLoop = sma->getSize() - 1;

  if ((int) sma->getSize() < period * 2)
    return bb;

  int inputLoop = in->getSize() - 1;

  while (inputLoop >= period && smaLoop >= period)
  {
    int count;
    double t2 = 0;
    for (count = 0, t2 = 0; count < period; count++)
    {
      double t = in->getData(inputLoop - count) - sma->getData(smaLoop - count);
      t2 = t2 + (t * t);
    }

    double t = sqrt(t2 / period);

    if (flag)
      bb->prepend(sma->getData(smaLoop) + (deviation * t)); // upper band
    else
      bb->prepend(sma->getData(smaLoop) - (deviation * t)); // lower band

    inputLoop--;
    smaLoop--;
  }

  delete in;
  delete sma;
  return bb;
}

QString QSMath::getBB2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 8)
  {
    rc = QObject::tr("BB: missing field");
    return rc;
  }
  
  MAType type = getMAType(l[1]);
  
  int period = l[2].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("BB: invalid period field");
    return rc;
  }
  
  double deviation = l[3].toDouble(&ok);
  if (! ok)
  {
    rc = QObject::tr("BB: invalid deviation field");
    return rc;
  }
  
  int flag = l[4].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("BB: invalid flag field");
    return rc;
  }
  
  PlotLine *output = getBB(type, period, deviation, flag);
  
  output->setColor(l[5]);
  output->setType(l[6]);
  output->setLabel(l[7]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getCCI (int period)
{
  PlotLine *cci = new PlotLine();
  if (! data)
    return cci;

  PlotLine *tp = getTP();
  int tpLoop = tp->getSize() - 1;

  PlotLine *sma = getSMA(tp, period);
  int smaLoop = sma->getSize() - 1;

  while (tpLoop >= period && smaLoop >= period)
  {
    double md = 0;
    int loop;
    for (loop = 0; loop < period; loop++)
      md = md + fabs(tp->getData(tpLoop - loop) - sma->getData(smaLoop - loop));
    md = md / period;

    double t = (tp->getData(tpLoop) - sma->getData(smaLoop)) / (0.015 * md);
    cci->prepend(t);

    tpLoop--;
    smaLoop--;
  }

  delete tp;
  delete sma;
  return cci;
}

QString QSMath::getCCI2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 5)
  {
    rc = QObject::tr("CCI: missing field");
    return rc;
  }
  
  int period = l[1].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("CCI: invalid period field");
    return rc;
  }
  
  PlotLine *output = getCCI(period);
  
  output->setColor(l[2]);
  output->setType(l[3]);
  output->setLabel(l[4]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getMDI (int period)
{
  PlotLine *mdm = new PlotLine();
  if (! data)
    return mdm;

  int loop;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double high = data->getHigh(loop);
    double low = data->getLow(loop);
    double yhigh = data->getHigh(loop - 1);
    double ylow = data->getLow(loop - 1);
    double t = 0;

    if (high > yhigh)
      t = 0;
    else
    {
      if (low < ylow)
        t = ylow - low;
      else
	t = 0;
    }

    mdm->append(t);
  }

  PlotLine *tr = getTR();

  PlotLine *smamdm = getSMA(mdm, period);
  int mdmLoop = smamdm->getSize() - 1;

  PlotLine *smatr = getSMA(tr, period);
  int trLoop = smatr->getSize() - 1;

  PlotLine *mdi = new PlotLine();

  while (mdmLoop > -1 && trLoop > -1)
  {
    int t = (int) ((smamdm->getData(mdmLoop) / smatr->getData(trLoop)) * 100);
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    mdi->prepend(t);

    mdmLoop--;
    trLoop--;
  }

  delete mdm;
  delete tr;
  delete smamdm;
  delete smatr;
  return mdi;
}

QString QSMath::getMDI2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 5)
  {
    rc = QObject::tr("MDI: missing field");
    return rc;
  }
  
  int period = l[1].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("MDI: invalid period field");
    return rc;
  }
  
  PlotLine *output = getMDI(period);
  
  output->setColor(l[2]);
  output->setType(l[3]);
  output->setLabel(l[4]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getPDI (int period)
{
  PlotLine *pdm = new PlotLine();
  if (! data)
    return pdm;

  int loop;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double high = data->getHigh(loop);
    double low = data->getLow(loop);
    double yhigh = data->getHigh(loop - 1);
    double ylow = data->getLow(loop - 1);
    double t = 0;

    if (high > yhigh)
      t = high - yhigh;
    else
    {
      if (low < ylow)
	t = 0;
      else
      	t = 0;
    }

    pdm->append(t);
  }

  PlotLine *tr = getTR();

  PlotLine *smapdm = getSMA(pdm, period);
  int pdmLoop = smapdm->getSize() - 1;

  PlotLine *smatr = getSMA(tr, period);
  int trLoop = smatr->getSize() - 1;

  PlotLine *pdi = new PlotLine();

  while (pdmLoop > -1 && trLoop > -1)
  {
    int t = (int) ((smapdm->getData(pdmLoop) / smatr->getData(trLoop)) * 100);
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    pdi->prepend(t);

    pdmLoop--;
    trLoop--;
  }

  delete pdm;
  delete tr;
  delete smapdm;
  delete smatr;
  return pdi;
}

QString QSMath::getPDI2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 5)
  {
    rc = QObject::tr("PDI: missing field");
    return rc;
  }
  
  int period = l[1].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("PDI: invalid period field");
    return rc;
  }
  
  PlotLine *output = getPDI(period);
  
  output->setColor(l[2]);
  output->setType(l[3]);
  output->setLabel(l[4]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getADX (PlotLine *mdi, PlotLine *pdi, MAType maType, int period)
{
  int mdiLoop = mdi->getSize() - 1;
  int pdiLoop = pdi->getSize() - 1;

  PlotLine *disum = new PlotLine;
  PlotLine *didiff = new PlotLine;

  while (pdiLoop > -1 && mdiLoop > -1)
  {
    disum->prepend(pdi->getData(pdiLoop) + mdi->getData(mdiLoop));
    didiff->prepend(fabs(pdi->getData(pdiLoop) - mdi->getData(mdiLoop)));
    pdiLoop--;
    mdiLoop--;
  }

  int sumLoop = disum->getSize() - 1;
  int diffLoop = didiff->getSize() - 1;

  PlotLine *dx = new PlotLine;

  while (sumLoop > -1 && diffLoop > -1)
  {
    int t = (int) ((didiff->getData(diffLoop) / disum->getData(sumLoop)) * 100);
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    dx->prepend(t);

    sumLoop--;
    diffLoop--;
  }

  PlotLine *adx = getMA(dx, maType, period);

  delete disum;
  delete didiff;
  delete dx;
  return adx;
}

QString QSMath::getADX2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 6)
  {
    rc = QObject::tr("ADX: missing field");
    return rc;
  }
  
  MAType type = getMAType(l[1]);
  
  int period = l[2].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("ADX: invalid period field");
    return rc;
  }
  
  PlotLine *mdi = getMDI(period);
  PlotLine *pdi = getPDI(period);
  
  PlotLine *output = getADX(mdi, pdi, type, period);
  
  output->setColor(l[3]);
  output->setType(l[4]);
  output->setLabel(l[5]);
  
  customLines.replace(QString::number(i), output);

  delete mdi;
  delete pdi;
  return rc;
}

//********************************************************************

PlotLine * QSMath::getOSC (PlotLine *in, MAType fastMaType, MAType slowMaType,
                           int fastPeriod, int slowPeriod)
{
  PlotLine *fma = getMA(in, fastMaType, fastPeriod);
  int fmaLoop = fma->getSize() - 1;

  PlotLine *sma = getMA(in, slowMaType, slowPeriod);
  int smaLoop = sma->getSize() - 1;

  PlotLine *osc = new PlotLine();
  
  while (fmaLoop > -1 && smaLoop > -1)
  {
    osc->prepend(fma->getData(fmaLoop) - sma->getData(smaLoop));
    fmaLoop--;
    smaLoop--;
  }

  delete fma;
  delete sma;
  return osc;
}

QString QSMath::getOSC2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 9)
  {
    rc = QObject::tr("OSC: missing field");
    return rc;
  }
  
  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("OSC: invalid input field");
    return rc;
  }

  MAType ftype = getMAType(l[2]);
  
  MAType stype = getMAType(l[3]);
    
  int fperiod = l[4].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("OSC: invalid period field");
    return rc;
  }
  
  int speriod = l[5].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("OSC: invalid period field");
    return rc;
  }
  
  PlotLine *output = getOSC(input, ftype, stype, fperiod, speriod);
  
  output->setColor(l[6]);
  output->setType(l[7]);
  output->setLabel(l[8]);
  
  customLines.replace(QString::number(i), output);
      
  l[1].toInt(&ok, 10);
  if (! ok)
    delete input;
  
  return rc;
}

//********************************************************************

PlotLine * QSMath::getMF (int period)
{
  PlotLine *d = new PlotLine();
  if (! data)
    return d;

  int loop;
  for (loop = period; loop < (int) data->count(); loop++)
  {
    double pmf = 0;
    double nmf = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
    {
      double high = data->getHigh(loop - loop2);
      double low = data->getLow(loop - loop2);
      double close = data->getClose(loop - loop2);
      double volume = data->getVolume(loop - loop2);

      double ttp = (high + low + close) / 3;
      double mf = ttp * volume;

      high = data->getHigh(loop - loop2 - 1);
      low = data->getLow(loop - loop2 - 1);
      close = data->getClose(loop - loop2 - 1);

      double ytp = (high + low + close) / 3;

      if (ttp > ytp)
        pmf = pmf + mf;

      if (ytp > ttp)
        nmf = nmf + mf;
    }

    double mfi = 100 - (100 / (1 + (pmf / nmf)));

    if (mfi > 100)
      mfi = 100;
    if (mfi < 0)
      mfi = 0;

    d->append(mfi);
  }

  return d;
}

QString QSMath::getMF2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 5)
  {
    rc = QObject::tr("MF: missing field");
    return rc;
  }
  
  int period = l[1].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("MF: invalid period field");
    return rc;
  }
  
  PlotLine *output = getMF(period);
  
  output->setColor(l[2]);
  output->setType(l[3]);
  output->setLabel(l[4]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getMOM (PlotLine *in, int period)
{
  PlotLine *mom = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
    mom->append(in->getData(loop) - in->getData(loop - period));

  return mom;    
}

QString QSMath::getMOM2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 6)
  {
    rc = QObject::tr("MOM: missing field");
    return rc;
  }
  
  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("MOM: invalid input field");
    return rc;
  }

  int period = l[2].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("MOM: invalid period field");
    return rc;
  }
  
  PlotLine *output = getMOM(input, period);
  
  output->setColor(l[3]);
  output->setType(l[4]);
  output->setLabel(l[5]);
  
  customLines.replace(QString::number(i), output);
      
  l[1].toInt(&ok, 10);
  if (! ok)
    delete input;
  
  return rc;
}

//********************************************************************

PlotLine * QSMath::getNVI ()
{
  PlotLine *nvi = new PlotLine();
  if (! data)
    return nvi;

  int loop;
  double nv = 1000;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double volume = data->getVolume(loop);
    double close = data->getClose(loop);
    double yvolume = data->getVolume(loop - 1);
    double yclose = data->getClose(loop - 1);

    if (volume < yvolume)
      nv = nv + ((close - yclose) / yclose) * nv;

    nvi->append(nv);
  }
  
  return nvi;
}

QString QSMath::getNVI2 (int i, QStringList l)
{
  QString rc;
  
  if (l.count() != 4)
  {
    rc = QObject::tr("NVI: missing field");
    return rc;
  }
  
  PlotLine *output = getNVI();
  
  output->setColor(l[1]);
  output->setType(l[2]);
  output->setLabel(l[3]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getOBV ()
{
  PlotLine *obv = new PlotLine();
  if (! data)
    return obv;

  int loop;
  double t = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double close = data->getClose(loop);
    double volume = data->getVolume(loop);
    double yclose = data->getClose(loop - 1);

    if (close > yclose)
      t = t + volume;
    else
    {
      if (close < yclose)
      	t = t - volume;
    }

    obv->append(t);
  }
  
  return obv;
}

QString QSMath::getOBV2 (int i, QStringList l)
{
  QString rc;
  
  if (l.count() != 4)
  {
    rc = QObject::tr("OBV: missing field");
    return rc;
  }
  
  PlotLine *output = getOBV();
  
  output->setColor(l[1]);
  output->setType(l[2]);
  output->setLabel(l[3]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getPC (PlotLine *in, int period)
{
  PlotLine *pc = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
  {
    double t = in->getData(loop) - in->getData(loop - period);
    double t2 = (t / in->getData(loop - period)) * 100;
    pc->append(t2);
  }
  
  return pc;
}

QString QSMath::getPC2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 6)
  {
    rc = QObject::tr("PC: missing field");
    return rc;
  }
  
  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("PC: invalid input field");
    return rc;
  }

  int period = l[2].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("PC: invalid period field");
    return rc;
  }
  
  PlotLine *output = getPC(input, period);
  
  output->setColor(l[3]);
  output->setType(l[4]);
  output->setLabel(l[5]);
  
  customLines.replace(QString::number(i), output);
      
  l[1].toInt(&ok, 10);
  if (! ok)
    delete input;
  
  return rc;
}

//********************************************************************

PlotLine * QSMath::getPER (PlotLine *in)
{
  PlotLine *per = new PlotLine();
  if (in->getSize() < 1)
    return per;

  double base = in->getData(0);
  int loop;
  for (loop = 1; loop < (int) in->getSize(); loop++)
    per->append(((in->getData(loop) - base) / base) * 100);

  return per;
}

QString QSMath::getPER2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 5)
  {
    rc = QObject::tr("PER: missing field");
    return rc;
  }
  
  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("PER: invalid input field");
    return rc;
  }

  PlotLine *output = getPER(input);
  
  output->setColor(l[2]);
  output->setType(l[3]);
  output->setLabel(l[4]);
  
  customLines.replace(QString::number(i), output);
      
  l[1].toInt(&ok, 10);
  if (! ok)
    delete input;
  
  return rc;
}

//********************************************************************

double QSMath::getPPFR (double high, double low, double close)
{
  double pp = (high + low + close) / 3;
  double t = (2 * pp) - low;
  return t;
}

double QSMath::getPPSR (double high, double low, double close)
{
  double pp = (high + low + close) / 3;
  double t = pp + (high - low);
  return t;
}

double QSMath::getPPTR (double high, double low, double close)
{
  double pp = (high + low + close) / 3;
  double t = (2 * pp) + (high - (2 * low));
  return t;
}

double QSMath::getPPFS (double high, double low, double close)
{
  double pp = (high + low + close) / 3;
  double t = (2 * pp) - high;
  return t;
}

double QSMath::getPPSS (double high, double low, double close)
{
  double pp = (high + low + close) / 3;
  double t = pp - (high - low);
  return t;
}

double QSMath::getPPTS (double high, double low, double close)
{
  double pp = (high + low + close) / 3;
  double t = (2 * pp) - ((2 * high) - low);
  return t;
}

//********************************************************************

PlotLine * QSMath::getPVI ()
{
  PlotLine *pvi = new PlotLine();
  if (! data)
    return pvi;

  int loop = 0;
  double pv = 1000;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double volume = data->getVolume(loop);
    double close = data->getClose(loop);
    double yvolume = data->getVolume(loop - 1);
    double yclose = data->getClose(loop - 1);

    if (volume > yvolume)
      pv = pv + ((close - yclose) / yclose) * pv;

    pvi->append(pv);
  }

  return pvi;
}

QString QSMath::getPVI2 (int i, QStringList l)
{
  QString rc;
  
  if (l.count() != 4)
  {
    rc = QObject::tr("PVI: missing field");
    return rc;
  }
  
  PlotLine *output = getPVI();
  
  output->setColor(l[1]);
  output->setType(l[2]);
  output->setLabel(l[3]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getPVT ()
{
  PlotLine *pvt = new PlotLine();
  if (! data)
    return pvt;

  int loop = 0;
  double pv = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double close = data->getClose(loop);
    double volume = data->getVolume(loop);
    double yclose = data->getClose(loop - 1);

    pv = pv + (((close - yclose) / yclose) * volume);
    pvt->append(pv);
  }

  return pvt;
}

QString QSMath::getPVT2 (int i, QStringList l)
{
  QString rc;
  
  if (l.count() != 4)
  {
    rc = QObject::tr("PVT: missing field");
    return rc;
  }
  
  PlotLine *output = getPVT();
  
  output->setColor(l[1]);
  output->setType(l[2]);
  output->setLabel(l[3]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getROC (PlotLine *in, int period)
{
  PlotLine *roc = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
    roc->append(((in->getData(loop) / in->getData(loop - period)) * 100) - 100);
    
  return roc;
}

QString QSMath::getROC2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 6)
  {
    rc = QObject::tr("ROC: missing field");
    return rc;
  }
  
  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("ROC: invalid input field");
    return rc;
  }

  int period = l[2].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("ROC: invalid period field");
    return rc;
  }
  
  PlotLine *output = getROC(input, period);
  
  output->setColor(l[3]);
  output->setType(l[4]);
  output->setLabel(l[5]);
  
  customLines.replace(QString::number(i), output);
      
  l[1].toInt(&ok, 10);
  if (! ok)
    delete input;
  
  return rc;
}

//********************************************************************

PlotLine * QSMath::getRSI (PlotLine *in, int period)
{
  PlotLine *rsi = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
  {
    double loss = 0;
    double gain = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
    {
      double t = in->getData(loop - loop2) - in->getData(loop - loop2 - 1);
      if (t > 0)
        gain = gain + t;
      if (t < 0)
        loss = loss + fabs(t);
    }

    double again = gain / period;
    double aloss = loss / period;
    double rs = again / aloss;
    double t = 100 - (100 / (1 + rs));
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    rsi->append(t);
  }

  return rsi;
}

QString QSMath::getRSI2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 6)
  {
    rc = QObject::tr("RSI: missing field");
    return rc;
  }
  
  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("RSI: invalid input field");
    return rc;
  }

  int period = l[2].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("RSI: invalid period field");
    return rc;
  }
  
  PlotLine *output = getRSI(input, period);
  
  output->setColor(l[3]);
  output->setType(l[4]);
  output->setLabel(l[5]);
  
  customLines.replace(QString::number(i), output);
      
  l[1].toInt(&ok, 10);
  if (! ok)
    delete input;
  
  return rc;
}

//********************************************************************

PlotLine * QSMath::getSAR (double initial, double add, double limit)
{
  PlotLine *d = new PlotLine();
  if (! data)
    return d;

  double high = data->getHigh(1);
  double low = data->getLow(1);
  double yhigh = data->getHigh(0);
  double ylow = data->getLow(0);

  int flag = 0;
  double ep = 0;
  double sar = 0;
  double psar = 0;
  double a = initial;
  double t = yhigh;
  double t2 = high;
  if (t2 > t)
  {
    // we are long
    flag = 0;
    t = ylow;
    t2 = low;
    if (t < t2)
      ep = t;
    else
      ep = t2;

    sar = ep;
    psar = ep;
  }
  else
  {
    // we are short
    flag = 1;
    t = yhigh;
    t2 = high;
    if (t > t2)
      ep = t;
    else
      ep = t2;

    sar = ep;
    psar = ep;
  }

  d->append(sar);

  int loop;
  for (loop = 2; loop < (int) data->count(); loop++)
  {
    high = data->getHigh(loop);
    low = data->getLow(loop);
    yhigh = data->getHigh(loop - 1);
    ylow = data->getLow(loop - 1);

    // are we short?
    if (flag)
    {
      // check for a switch
      t = high;
      if (t >= sar)
      {
        sar = ep;
        psar = sar;
        ep = t;
        flag = 0;
        a = initial;
      }
      else
      {
        t = low;
        if (t  < ep)
        {
          ep = t;
          a = a + add;
          if (a > limit)
            a = limit;
        }

        t = psar + (a * (ep - psar));
        t2 = high;
        if (t < t2)
        {
          double t3 = yhigh;
          if (t3 > t2)
            t = t3;
          else
            t = t2;
        }
        psar = sar;
        sar = t;
      }
    }
    else
    {
      // we are long
      // check for a switch
      t = low;
      if (t <= sar)
      {
        sar = ep;
        psar = sar;
        ep = t;
        flag = 1;
        a = initial;
      }
      else
      {
        t = high;
        if (t  > ep)
        {
          ep = t;
          a = a + add;
          if (a > limit)
            a = limit;
        }

        t = psar + (a * (ep - psar));
        t2 = low;
        if (t > t2)
        {
          double t3 = ylow;
          if (t3 < t2)
            t = t3;
          else
            t = t2;
        }
        psar = sar;
        sar = t;
      }
    }

    d->append(sar);
  }

  return d;
}

QString QSMath::getSAR2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 7)
  {
    rc = QObject::tr("SAR: missing field");
    return rc;
  }
  
  double initial = l[1].toDouble(&ok);
  if (! ok)
  {
    rc = QObject::tr("SAR: invalid initial field");
    return rc;
  }
  
  double add = l[2].toDouble(&ok);
  if (! ok)
  {
    rc = QObject::tr("SAR: invalid add field");
    return rc;
  }
  
  double limit = l[3].toDouble(&ok);
  if (! ok)
  {
    rc = QObject::tr("SAR: invalid limit field");
    return rc;
  }
  
  PlotLine *output = getSAR(initial, add, limit);
  
  output->setColor(l[4]);
  output->setType(l[5]);
  output->setLabel(l[6]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getSD (PlotLine *in, int period)
{
  PlotLine *sd = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
  {
    double mean = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
      mean = mean + in->getData(loop - loop2);
    mean = mean / period;

    double ds = 0;
    for (loop2 = 0; loop2 < period; loop2++)
    {
      double t = in->getData(loop - loop2) - mean;
      ds = ds + (t * t);
    }
    ds = ds / period;

    sd->append(ds);
  }

  return sd;
}

QString QSMath::getSD2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 6)
  {
    rc = QObject::tr("SD: missing field");
    return rc;
  }
  
  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("SD: invalid input field");
    return rc;
  }

  int period = l[2].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("SD: invalid period field");
    return rc;
  }
  
  PlotLine *output = getSD(input, period);
  
  output->setColor(l[3]);
  output->setType(l[4]);
  output->setLabel(l[5]);
  
  customLines.replace(QString::number(i), output);
      
  l[1].toInt(&ok, 10);
  if (! ok)
    delete input;
  
  return rc;
}

//********************************************************************

PlotLine * QSMath::getSTOCH (MAType maType, int period, int kperiod)
{
  PlotLine *k = new PlotLine();

  int loop;
  for (loop = period; loop < (int) data->count(); loop++)
  {
    int loop2;
    double l;
    double h;
    for (loop2 = 0, l = 9999999, h = 0; loop2 < period; loop2++)
    {
      double high = data->getHigh(loop - loop2);
      double low = data->getLow(loop - loop2);

      double t = high;
      if (t > h)
        h = t;

      t = low;
      if (t < l)
        l = t;
    }

    double close = data->getClose(loop);
    double t = ((close - l) / (h - l)) * 100;
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    k->append(t);
  }

  if (kperiod > 1)
  {
    PlotLine *k2 = getMA(k, maType, kperiod);
    delete k;
    return k2;
  }
  else
    return k;
}

QString QSMath::getSTOCH2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 7)
  {
    rc = QObject::tr("STOCH: missing field");
    return rc;
  }

  MAType type = getMAType(l[1]);
    
  int period = l[2].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("STOCH: invalid period field");
    return rc;
  }
  
  int kperiod = l[3].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("STOCH: invalid k period field");
    return rc;
  }
  
  PlotLine *output = getSTOCH(type, period, kperiod);
  
  output->setColor(l[4]);
  output->setType(l[5]);
  output->setLabel(l[6]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getVFI (int period)
{
  PlotLine *vfi = new PlotLine();
  if (! data)
    return vfi;

  int loop;
  for (loop = period; loop < (int) data->count(); loop++)
  {
    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    double close = data->getClose(loop-period);
    double high = data->getHigh(loop-period);
    double low = data->getLow(loop-period);
    double typical = (high+low+close)/3.0;
    for(i=loop-period+1; i<=loop; i++) {
      double ytypical = typical;
      close = data->getClose(i);
      high = data->getHigh(i);
      low = data->getLow(i);
      typical = (high+low+close)/3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta*delta;
      sma_vol += data->getVolume(i);
      }
    inter = 0.2*sqrt(inter/(double)period)*typical;
    sma_vol /= (double)period;

    close = data->getClose(loop-period);
    high = data->getHigh(loop-period);
    low = data->getLow(loop-period);
    typical = (high+low+close)/3.0;
    double t = 0;
    for(i=loop-period+1; i<=loop; i++) {
      double ytypical = typical;
      double volume = data->getVolume(i);
      close = data->getClose(i);
      high = data->getHigh(i);
      low = data->getLow(i);
      typical = (high+low+close)/3.0;

      if (typical > ytypical+inter)
        t = t + log(1.0 + volume/sma_vol);
      else
      {
        if (typical < ytypical-inter)
          t = t - log(1.0 + volume/sma_vol);
      }
    }

    vfi->append(t);
  }
  
  return vfi;
}

QString QSMath::getVFI2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 5)
  {
    rc = QObject::tr("VFI: missing field");
    return rc;
  }
  
  int period = l[1].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("VFI: invalid period field");
    return rc;
  }
  
  PlotLine *output = getVFI(period);
  
  output->setColor(l[2]);
  output->setType(l[3]);
  output->setLabel(l[4]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getVOLR (int period)
{
  PlotLine *volr = new PlotLine();
  if (! data)
    return volr;

  PlotLine *trg = getTR();
  int trgLoop = trg->getSize() - 1;

  PlotLine *ma = getEMA(trg, period);
  int maLoop = ma->getSize() - 1;

  while (maLoop > -1 && trgLoop > -1)
  {
    volr->prepend(trg->getData(trgLoop) / ma->getData(maLoop));
    maLoop--;
    trgLoop--;
  }

  delete trg;
  delete ma;
  return volr;
}

QString QSMath::getVOLR2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 5)
  {
    rc = QObject::tr("VOLR: missing field");
    return rc;
  }
  
  int period = l[1].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("VOLR: invalid period field");
    return rc;
  }
  
  PlotLine *output = getVOLR(period);
  
  output->setColor(l[2]);
  output->setType(l[3]);
  output->setLabel(l[4]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getWAD ()
{
  PlotLine *wad = new PlotLine();
  if (! data)
    return wad;

  int loop;
  double accum = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double high = data->getHigh(loop);
    double low = data->getLow(loop);
    double close = data->getClose(loop);
    double yclose = data->getClose(loop - 1);

    double h = high;
    if (yclose > h)
      h = yclose;

    double l = low;
    if (yclose < l)
      l = yclose;

    if (close > yclose)
      accum = accum + (close - l);
    else
    {
      if (yclose == close)
        ;
      else
        accum = accum - (h - close);
    }

    wad->append(accum);
  }
  
  return wad;
}

QString QSMath::getWAD2 (int i, QStringList l)
{
  QString rc;
  
  if (l.count() != 4)
  {
    rc = QObject::tr("WAD: missing field");
    return rc;
  }
  
  PlotLine *output = getWAD();
  
  output->setColor(l[1]);
  output->setType(l[2]);
  output->setLabel(l[3]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//********************************************************************

PlotLine * QSMath::getWILLR (int period)
{
  PlotLine *willr = new PlotLine();
  if (! data)
    return willr;

  int loop;
  for (loop = period; loop < (int) data->count(); loop++)
  {
    int loop2;
    double lw;
    double hg;
    for (loop2 = 0, lw = 9999999, hg = 0; loop2 < period; loop2++)
    {
      double high = data->getHigh(loop - loop2);
      double low = data->getLow(loop - loop2);

      if (high > hg)
        hg = high;

      if (low < lw)
        lw = low;
    }

    double t = ((hg - data->getClose(loop)) / (hg - lw)) * 100;
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;
    t *= -1;

    willr->append(t);
  }

  return willr;
}

QString QSMath::getWILLR2 (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 5)
  {
    rc = QObject::tr("WILLR: missing field");
    return rc;
  }
  
  int period = l[1].toInt(&ok, 10);
  if (! ok)
  {
    rc = QObject::tr("WILLR: invalid period field");
    return rc;
  }
  
  PlotLine *output = getWILLR(period);
  
  output->setColor(l[2]);
  output->setType(l[3]);
  output->setLabel(l[4]);
  
  customLines.replace(QString::number(i), output);
      
  return rc;
}

//**************************************************

QString QSMath::getADD (int i, QStringList l)
{
  QString rc;
  bool ok;
  int loop = 0;
  int loop2 = 0;
  
  if (l.count() != 6)
  {
    rc = QObject::tr("ADD: missing field");
    return rc;
  }

  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("ADD: invalid input field");
    return rc;
  }
  else
    loop = input->getSize() - 1;
    
  PlotLine *input2 = getInputLine(l[2]);
  double inputNum = 0;
  if (! input2)
  {
    if (l[2].contains("#"))
    {
      l[2].remove(0, 1);
      inputNum = l[2].toDouble(&ok);
      if (! ok)
      {
        delete input;
        rc = QObject::tr("ADD: invalid input2 field");
        return rc;
      }
    }
    else
    {
      rc = QObject::tr("ADD: invalid input2 field");
      return rc;
    }
  }
  else
    loop2 = input2->getSize() - 1;
    
  PlotLine *output = new PlotLine;
  
  while (loop > -1)
  {
    if (input2)
    {
      if (loop2 < 0)
        break;
    }
    
    double t = 0;
    
    if (! input2)
      t = inputNum;
    else
      t = input2->getData(loop2);
      
    output->prepend(input->getData(loop) + t);
    
    loop--;
    if (input2)
      loop2--;
  }
  
  output->setColor(l[3]);
  output->setType(l[4]);
  output->setLabel(l[5]);
  
  customLines.replace(QString::number(i), output);

  l[1].toInt(&ok, 10);
  if (! ok)
    delete input;
  
  if (input2)
  {
    l[2].toInt(&ok, 10);
    if (! ok)
      delete input2;
  }
          
  return rc;
}

//**************************************************

QString QSMath::getSUB (int i, QStringList l)
{
  QString rc;
  bool ok;
  int loop = 0;
  int loop2 = 0;
  
  if (l.count() != 6)
  {
    rc = QObject::tr("SUB: missing field");
    return rc;
  }

  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("SUB: invalid input field");
    return rc;
  }
  else
    loop = input->getSize() - 1;
    
  PlotLine *input2 = getInputLine(l[2]);
  double inputNum = 0;
  if (! input2)
  {
    if (l[2].contains("#"))
    {
      l[2].remove(0, 1);
      inputNum = l[2].toDouble(&ok);
      if (! ok)
      {
        delete input;
        rc = QObject::tr("SUB: invalid input2 field");
        return rc;
      }
    }
    else
    {
      rc = QObject::tr("SUB: invalid input2 field");
      return rc;
    }
  }
  else
    loop2 = input2->getSize() - 1;
    
  PlotLine *output = new PlotLine;
  
  while (loop > -1)
  {
    if (input2)
    {
      if (loop2 < 0)
        break;
    }
    
    double t = 0;
    
    if (! input2)
      t = inputNum;
    else
      t = input2->getData(loop2);
      
    output->prepend(input->getData(loop) - t);
    
    loop--;
    if (input2)
      loop2--;
  }
  
  output->setColor(l[3]);
  output->setType(l[4]);
  output->setLabel(l[5]);
  
  customLines.replace(QString::number(i), output);

  l[1].toInt(&ok, 10);
  if (! ok)
    delete input;
  
  if (input2)
  {
    l[2].toInt(&ok, 10);
    if (! ok)
      delete input2;
  }
          
  return rc;
}

//**************************************************

QString QSMath::getMUL (int i, QStringList l)
{
  QString rc;
  bool ok;
  int loop = 0;
  int loop2 = 0;
  
  if (l.count() != 6)
  {
    rc = QObject::tr("MUL: missing field");
    return rc;
  }

  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("MUL: invalid input field");
    return rc;
  }
  else
    loop = input->getSize() - 1;
    
  PlotLine *input2 = getInputLine(l[2]);
  double inputNum = 0;
  if (! input2)
  {
    if (l[2].contains("#"))
    {
      l[2].remove(0, 1);
      inputNum = l[2].toDouble(&ok);
      if (! ok)
      {
        delete input;
        rc = QObject::tr("MUL: invalid input2 field");
        return rc;
      }
    }
    else
    {
      rc = QObject::tr("MUL: invalid input2 field");
      return rc;
    }
  }
  else
    loop2 = input2->getSize() - 1;
    
  PlotLine *output = new PlotLine;
  
  while (loop > -1)
  {
    if (input2)
    {
      if (loop2 < 0)
        break;
    }
    
    double t = 0;
    
    if (! input2)
      t = inputNum;
    else
      t = input2->getData(loop2);
      
    output->prepend(input->getData(loop) * t);
    
    loop--;
    if (input2)
      loop2--;
  }
  
  output->setColor(l[3]);
  output->setType(l[4]);
  output->setLabel(l[5]);
  
  customLines.replace(QString::number(i), output);

  l[1].toInt(&ok, 10);
  if (! ok)
    delete input;
  
  if (input2)
  {
    l[2].toInt(&ok, 10);
    if (! ok)
      delete input2;
  }
          
  return rc;
}

//**************************************************

QString QSMath::getDIV (int i, QStringList l)
{
  QString rc;
  bool ok;
  int loop = 0;
  int loop2 = 0;
  
  if (l.count() != 6)
  {
    rc = QObject::tr("DIV: missing field");
    return rc;
  }

  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("DIV: invalid input field");
    return rc;
  }
  else
    loop = input->getSize() - 1;
    
  PlotLine *input2 = getInputLine(l[2]);
  double inputNum = 0;
  if (! input2)
  {
    if (l[2].contains("#"))
    {
      l[2].remove(0, 1);
      inputNum = l[2].toDouble(&ok);
      if (! ok)
      {
        delete input;
        rc = QObject::tr("DIV: invalid input2 field");
        return rc;
      }
    }
    else
    {
      rc = QObject::tr("DIV: invalid input2 field");
      return rc;
    }
  }
  else
    loop2 = input2->getSize() - 1;
    
  PlotLine *output = new PlotLine;
  
  while (loop > -1)
  {
    if (input2)
    {
      if (loop2 < 0)
        break;
    }
    
    double t = 0;
    
    if (! input2)
      t = inputNum;
    else
      t = input2->getData(loop2);
      
    output->prepend(input->getData(loop) / t);
    
    loop--;
    if (input2)
      loop2--;
  }
  
  output->setColor(l[3]);
  output->setType(l[4]);
  output->setLabel(l[5]);
  
  customLines.replace(QString::number(i), output);

  l[1].toInt(&ok, 10);
  if (! ok)
    delete input;
  
  if (input2)
  {
    l[2].toInt(&ok, 10);
    if (! ok)
      delete input2;
  }
          
  return rc;
}

//**************************************************

QString QSMath::getREF (int i, QStringList l)
{
  QString rc;
  bool ok;
  
  if (l.count() != 6)
  {
    rc = QObject::tr("REF: missing field");
    return rc;
  }

  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("REF: invalid input field");
    return rc;
  }
  
  int period = l[2].toInt(&ok, 10);
  if (! ok || period < 0)
  {
    rc = QObject::tr("REF: invalid period field");
    return rc;
  }
  
  int loop = 0;
  PlotLine *output = new PlotLine;
  for (loop = 0; loop < input->getSize(); loop++)
  {
    if (loop - period < 0)
      continue;
      
    output->append(input->getData(loop - period));
  }
  
  output->setColor(l[3]);
  output->setType(l[4]);
  output->setLabel(l[5]);
  
  customLines.replace(QString::number(i), output);

  delete input;
  return rc;
}

//**************************************************

QString QSMath::getCOMPARE (int i, QStringList l)
{
  QString rc;
  bool ok;
  int loop = 0;
  int loop2 = 0;
  
  if (l.count() != 7)
  {
    rc = QObject::tr("COMPARE: missing field");
    return rc;
  }

  PlotLine *input = getInputLine(l[1]);
  if (! input)
  {
    rc = QObject::tr("COMPARE: invalid data1 field");
    return rc;
  }
  else
    loop = input->getSize() - 1;
    
  Operator op = getOperator(l[2]);
  if (op == NoOp)
  {
    rc = QObject::tr("COMPARE: invalid operator field");
    return rc;
  }
    
  PlotLine *input2 = getInputLine(l[3]);
  double inputNum = 0;
  if (! input2)
  {
    if (l[3].contains("#"))
    {
      l[3].remove(0, 1);
      inputNum = l[3].toDouble(&ok);
      if (! ok)
      {
        delete input;
        rc = QObject::tr("COMPARE: invalid data2 field");
        return rc;
      }
    }
    else
    {
      rc = QObject::tr("COMPARE: invalid data2 field");
      return rc;
    }
  }
  else
    loop2 = input2->getSize() - 1;
    
  PlotLine *output = new PlotLine;
  
  while (loop > -1)
  {
    if (input2)
    {
      if (loop2 < 0)
        break;
    }
    
    double t = 0;
    
    if (! input2)
      t = inputNum;
    else
      t = input2->getData(loop2);
      
    switch (op)
    {
      case Equal:
        if (input->getData(loop) == t)
          output->prepend(1);
	else
          output->prepend(0);
        break;
      case LessThan:
        if (input->getData(loop) < t)
          output->prepend(1);
	else
          output->prepend(0);
        break;
      case LessThanEqual:
        if (input->getData(loop) <= t)
          output->prepend(1);
	else
          output->prepend(0);
        break;
      case GreaterThan:
        if (input->getData(loop) > t)
          output->prepend(1);
	else
          output->prepend(0);
        break;
      case GreaterThanEqual:
        if (input->getData(loop) >= t)
          output->prepend(1);
	else
          output->prepend(0);
        break;
      default:
        break;
    }
      
    loop--;
    if (input2)
      loop2--;
  }
  
  output->setColor(l[4]);
  output->setType(l[5]);
  output->setLabel(l[6]);
  
  customLines.replace(QString::number(i), output);

  l[1].toInt(&ok, 10);
  if (! ok)
    delete input;
  
  if (input2)
  {
    l[3].toInt(&ok, 10);
    if (! ok)
      delete input2;
  }
          
  return rc;
}

//**************************************************
//********* custom functions ***********************
//**************************************************

void QSMath::clearCustomLines ()
{
  customLines.setAutoDelete(TRUE);
  customLines.clear();
  customLines.setAutoDelete(FALSE);
}

PlotLine * QSMath::getCustomLine (int i)
{
  return customLines[QString::number(i)];
}

QString QSMath::calculateCustomFormula (QStringList fl, QStringList pl)
{
  int loop;
  QString err;
  for (loop = 0; loop < (int) fl.count(); loop++)
  {
    if (err.length())
      break;
    
    QStringList l = QStringList::split(",", fl[loop], FALSE);
    int loop2;
    for (loop2 = 0; loop2 < (int) l.count(); loop2++)
      l[loop2] = l[loop2].stripWhiteSpace();

    if (! l[0].compare(QObject::tr("AD")))
    {
      err = getAD2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("ADD")))
    {
      err = getADD(loop + 1, l);
      continue;
    }
    
    if (! l[0].compare(QObject::tr("ADX")))
    {
      err = getADX2(loop + 1, l);
      continue;
    }
      
    if (! l[0].compare(QObject::tr("BB")))
    {
      err = getBB2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("CCI")))
    {
      err = getCCI2(loop + 1, l);
      continue;
    }

    if (! l[0].compare(QObject::tr("COMP")))
    {
      err = getCOMPARE(loop + 1, l);
      continue;
    }

    if (! l[0].compare(QObject::tr("DIV")))
    {
      err = getDIV(loop + 1, l);
      continue;
    }
    
    if (! l[0].compare(QObject::tr("MA")))
    {
      err = getMA2(loop + 1, l);
      continue;
    }
    
    if (! l[0].compare(QObject::tr("MDI")))
    {
      err = getMDI2(loop + 1, l);
      continue;
    }
      
    if (! l[0].compare(QObject::tr("MF")))
    {
      err = getMF2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("MOM")))
    {
      err = getMOM2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("MUL")))
    {
      err = getMUL(loop + 1, l);
      continue;
    }
    
    if (! l[0].compare(QObject::tr("NVI")))
    {
      err = getNVI2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("OBV")))
    {
      err = getOBV2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("OSC")))
    {
      err = getOSC2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("PC")))
    {
      err = getPC2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("PDI")))
    {
      err = getPDI2(loop + 1, l);
      continue;
    }

    if (! l[0].compare(QObject::tr("PER")))
    {
      err = getPER2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("PVI")))
    {
      err = getPVI2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("PVT")))
    {
      err = getPVT2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("REF")))
    {
      err = getREF(loop + 1, l);
      continue;
    }
    
    if (! l[0].compare(QObject::tr("ROC")))
    {
      err = getROC2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("RSI")))
    {
      err = getRSI2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("SAR")))
    {
      err = getSAR2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("SD")))
    {
      err = getSD2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("STOCH")))
    {
      err = getSTOCH2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("SUB")))
    {
      err = getSUB(loop + 1, l);
      continue;
    }
    
    if (! l[0].compare(QObject::tr("TP")))
    {
      err = getTP2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("TR")))
    {
      err = getTR2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("VFI")))
    {
      err = getVFI2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("VOLR")))
    {
      err = getVOLR2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("WAD")))
    {
      err = getWAD2(loop + 1, l);
      continue;
    }
  
    if (! l[0].compare(QObject::tr("WILLR")))
    {
      err = getWILLR2(loop + 1, l);
      continue;
    }
  }

  if (err.length())
  {
    err.prepend("Line ");
    err.prepend(QString::number(loop + 1));
    err.prepend(": ");
    qDebug(err);
    clearCustomLines();
  }
  else
  {
    customLines.setAutoDelete(TRUE);
  
    for (loop = 0; loop < (int) pl.count(); loop++)
    {
      if (! pl[loop].toInt())
        customLines.remove(QString::number(loop + 1));
    }
    
    customLines.setAutoDelete(FALSE);
  }
        
  return err;
}

PlotLine * QSMath::getInputLine (QString d)
{
  bool ok;
  PlotLine *input = 0;
  
  if (d.contains("#"))
    return input;
  
  d.toInt(&ok, 10);
  if (! ok)
    input = data->getInput(data->getInputType(d));
  else
    input = customLines[d];

  return input;
}

