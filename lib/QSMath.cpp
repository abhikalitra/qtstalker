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

PlotLine * QSMath::getEMA (PlotLine *d, int period)
{
  PlotLine *ema = new PlotLine;

  if (period >= (int) d->getSize())
    return ema;

  if (period < 1)
    return ema;

  double smoother = 2.0 / (period + 1);

  int count;
  double t = 0;
  int loop = period - 1;
  for (count = 0; count < period; count++)
    t = t + d->getData(loop - count);

  double yesterday = t / period;

  for (; loop < (int) d->getSize(); loop++)
  {
    double t  = (smoother * (d->getData(loop) - yesterday)) + yesterday;
    yesterday = t;
    ema->append(t);
  }

  return ema;
}

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
    for (loop2 = period, weight = 1, divider = 0, total = 0; loop2 >= 0; loop2--, weight++)
    {
      total = total + (d->getData(loop - loop2) * weight);
      divider = divider + weight;
    }

    wma->append(total / divider);
  }

  return wma;
}

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
      bb->prepend(sma->getData(smaLoop) - (deviation * t)); // lower band
    else
      bb->prepend(sma->getData(smaLoop) + (deviation * t)); // upper band

    inputLoop--;
    smaLoop--;
  }

  delete in;
  delete sma;
  return bb;
}

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

PlotLine * QSMath::getMOM (PlotLine *in, int period)
{
  PlotLine *mom = new PlotLine();
  if (! data)
    return mom;

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
    mom->append(in->getData(loop) - in->getData(loop - period));

  return mom;    
}

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

PlotLine * QSMath::getPER (PlotLine *in)
{
  PlotLine *per = new PlotLine();
  if (! data)
    return per;

  double base = in->getData(0);
  int loop;
  for (loop = 1; loop < (int) in->getSize(); loop++)
    per->append(((in->getData(loop) - base) / base) * 100);

  return per;
}

PlotLine * QSMath::getROC (PlotLine *in, int period)
{
  PlotLine *roc = new PlotLine();
  if (! data)
    return roc;

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
    roc->append(((in->getData(loop) / in->getData(loop - period)) * 100) - 100);
    
  return roc;
}

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

PlotLine * QSMath::getSD (PlotLine *in, int period)
{
  PlotLine *sd = new PlotLine();
  if (! data)
    return sd;

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


