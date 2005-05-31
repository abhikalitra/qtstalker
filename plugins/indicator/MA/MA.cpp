/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#include "MA.h"
#include "PrefDialog.h"
#include "MADialog.h"
#include <qdict.h>
#include <qobject.h>
#include <math.h>

#define MAXNUM 2147483647

#if ! defined ( PI )
#define PI 3.141592653589793
#endif

MA::MA ()
{
  pluginName = "MA";
  helpFile = "ma.html";
  
  setDefaults();
}

MA::~MA ()
{
}

void MA::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 10;
  maType = (int) SMA;  
  input = BarData::Close;
  customInput = "1";
  maTypeList = getMATypes();
  
  // lowpass stuff
  freq = 0.05;		// values between 0.0 and 0.5
  width = 0.2;		//values between 0.0001 and 0.2
}

void MA::calculate ()
{
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("MA::calculate: no input");
    return;
  }
  
  PlotLine *ma = getMA(in, maType, period, freq, width);
  
  if (! customFlag)
    delete in;

  ma->setColor(color);
  ma->setType(lineType);
  ma->setLabel(label);
  output->addLine(ma);
}

int MA::indicatorPrefDialog (QWidget *w)
{
/*
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString stl = QObject::tr("MA Type");
  QString il = QObject::tr("Input");
  QString fl = QObject::tr("Freq");
  QString wl = QObject::tr("Width");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("MA Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  dialog->addComboItem(stl, pl, maTypeList, maType);
  if (customFlag)
    dialog->addFormulaInputItem(il, pl, FALSE, customInput);
  else
    dialog->addComboItem(il, pl, inputTypeList, input);

  // lowpass stuff
  dialog->addFloatItem(fl, pl, freq, .009, 99999999);
  dialog->addFloatItem(wl, pl, width, .009, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    period = dialog->getInt(perl);
    label = dialog->getText(ll);
    maType = dialog->getComboIndex(stl);
    if (customFlag)
      customInput = dialog->getFormulaInput(il);
    else
      input = (BarData::InputType) dialog->getComboIndex(il);
      
    // lowpass stuff
    freq = dialog->getFloat(fl);
    width = dialog->getFloat(wl);
    if (freq < 0.0)
      freq = 0.0;
    if (freq > 0.5)
      freq = 0.5;
    if (width < 0.0001)
      width = 0.0001;
    if (width > 0.2)
      width = 0.2;
      
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
*/

  MADialog *dialog = new MADialog(helpFile, customFlag);
  dialog->setCaption(QObject::tr("MA Indicator"));
  dialog->setColor(color);
  dialog->setLineType(lineTypes, lineType);
  dialog->setLabel(label);
  dialog->setPeriod(period);
  dialog->setMAType(maTypeList, maType);
  if (customFlag)
    dialog->setCustomInput(customInput);
  else
    dialog->setInput(inputTypeList, input);
  dialog->setFreq(freq);
  dialog->setWidth(width);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor();
    lineType = (PlotLine::LineType) dialog->getLineType();
    period = dialog->getPeriod();
    label = dialog->getLabel();
    maType = dialog->getMAType();
    if (customFlag)
      customInput = dialog->getCustomInput();
    else
      input = (BarData::InputType) dialog->getInput();
    freq = dialog->getFreq();
    width = dialog->getWidth();
    if (freq < 0.0)
      freq = 0.0;
    if (freq > 0.5)
      freq = 0.5;
    if (width < 0.0001)
      width = 0.0001;
    if (width > 0.2)
      width = 0.2;
      
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MA::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("maType");
  if (s.length())
    maType = s.toInt();
    
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
    
  s = dict.getData("freq");
  if (s.length())
    freq = s.toFloat();
    
  s = dict.getData("width");
  if (s.length())
    width = s.toFloat();
}

void MA::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("maType", QString::number(maType));
  dict.setData("input", QString::number(input));
  dict.setData("plugin", pluginName);
  dict.setData("customInput", customInput);
  dict.setData("freq", QString::number(freq));
  dict.setData("width", QString::number(width));
}

PlotLine * MA::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  if (output->getLines())
    return output->getLine(0);
  else
    return 0;
}

int MA::getMinBars ()
{
  int t = minBars;
  if (maType == Lowpass)
    t = t + 20;
  else
    t = t + period;
  return t;
}

PlotLine * MA::getEMA (PlotLine *d, int period)
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

PlotLine * MA::getSMA (PlotLine *d, int period)
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
  while (++loop < period)
  {
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

PlotLine * MA::getWMA (PlotLine *d, int period)
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

PlotLine * MA::getWilderMA (PlotLine *d, int period)
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

PlotLine * MA::getMA (PlotLine *in, int type, int period, double fre, double wid)
{
  PlotLine *ma = 0;
  
  switch (type)
  {
    case SMA:
      ma = getSMA(in, period);
      break;
    case EMA:
      ma = getEMA(in, period);
      break;
    case WMA:
      ma = getWMA(in, period);
      break;
    case Wilder:
      ma = getWilderMA(in, period);
      break;
    case Lowpass:
      ma = getLowpass(in, fre, wid);
      break;
    default:
      break;    
  }
  
  return ma;  
}

QStringList MA::getMATypes ()
{
  QStringList l;
  l.append("EMA");
  l.append("SMA");
  l.append("WMA");
  l.append("Wilder");
  l.append("Lowpass");
  return l;
}

int MA::getMAType (QString d)
{
  int type = (int) SMA;
  
  while (1)
  {
    if (! d.compare("EMA"))
    {
      type = (int) EMA;
      break;
    }
    
    if (! d.compare("WMA"))
    {
      type = (int) WMA;
      break;
    }
  
    if (! d.compare("Wilder"))
    {
      type = (int) Wilder;
      break;
    }
    
    if (! d.compare("Lowpass"))
      type = (int) Lowpass;
      
    break;
  }
  
  return type;
}

//*************************************************************************
//************************* LOWPASS ***************************************
//*************************************************************************

PlotLine * MA::getLowpass (PlotLine *in, double fre, double wid)
{
  PlotLine *out = new PlotLine;
  
  if (in->getSize() == 0)
    return out;
    
// ----------------------------------------------------------------------
  double slope = 0;       // will be modified on call to detrend
  double intercept = 0;
  int length = 0;      // original caller size
  int n = 0;          // size raised to next power of 2 for fft
  int i = 0;

  length = in->getSize();

  // Detrend input series
  PlotLine *series = detrend(in, slope, intercept, true);

  // Raise length to next power of 2, pad with zero
  PlotLine *series2 = raise2Power(series, 0);

  n = series2->getSize();

  //qtsFFT fft(n);        // construct fft object
  fft = new qtsFFT(n);
 
  // do fft
  PlotLine * fftFreq = fft->do_FFTqts(series2);
  //PlotLine * fftFreq = fft.do_FFTqts(series2);

  // apply low pass filter
  double f = 0; 
  double dist = 0; 
  double wt = 0;
  int halfn = n/2;

  double freqSave = fftFreq->getData(halfn);

  for (i = 0 ; i < halfn ; i++)
  {
    f = (double) i / (double) n ;  // Frequency
    if (f <= fre)                 // Flat response
      wt = 1.0 ;
    else
    {
      dist = (f - fre) / wid;
      wt = exp ( -dist * dist ) ;
    }

    fftFreq->setData(i, fftFreq->getData(i) * wt) ;
    fftFreq->setData(halfn + i, fftFreq->getData(halfn + i) * wt) ;
  }

  dist = (0.5 - fre) / wid;     // Do Nyquist in fftFreq[0]
  fftFreq->setData(halfn, freqSave * exp ( -dist * dist )) ;

  // Do inverse FFT to recover real domain
  PlotLine *fftReal = fft->do_iFFTqts(fftFreq);
  //PlotLine *fftReal = fft.do_iFFTqts(fftFreq);

  // Retrend input series, n.b. original length
  PlotLine *series3 = detrend(fftReal, slope, intercept, false);

  for (i = 0; i < length; i++)
    out->append(series3->getData(i));

  delete series;
  delete series2;
  delete series3;
  delete fftReal;
  delete fftFreq;
  delete fft;
  
  return out;
}

PlotLine * MA::detrend(PlotLine *x, double &slope, double &intercept, bool detrend)
{
// detrend = true (default) = detrebd x  , return slope/intercept
// detrend = false = undo detrend using slope/intercept provided

  int length = x->getSize();
  int i = 0;

  PlotLine *result = new PlotLine;

  if (detrend)
  {
    intercept = x->getData(0) ;
    slope = (x->getData(length-1) - intercept) / (length-1) ;

    for(i = 0; i < length; i++)
      result->append(x->getData(i)  - intercept - slope * i) ;
  }
  else         //retrend
  {
    for(i = 0; i < length; i++)
      result->append(x->getData(i)   + intercept + slope * i ) ;
  }

  return result;
}

PlotLine * MA::raise2Power(PlotLine *x, double pad)
{
//   Raise the caller's n up to the next power of two
//   pad remainder with pad, default = 0;

  PlotLine *result = new PlotLine;

  int length = x->getSize();
  int n = 0;
  int i = 0;

  for (n = 2 ; n < MAXNUM / 2 ; n *= 2)
  {
    if (n >= length)
      break ;
  }

  for (i = 0; i < n; i++)
  {
    if (i < length)
      result->append(x->getData(i));
    else
      result->append(pad);      // pad with zero
  }

  return result;
}

//*************************************************************************
//*************************************************************************
//*************************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MA *o = new MA;
  return ((IndicatorPlugin *) o);
}


