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

#include "LOWPASS.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <qcolor.h>
#include <math.h>


LOWPASS::LOWPASS ()
{
  pluginName = "LOWPASS";
  helpFile = "lowpass.html";
  setDefaults();
}

LOWPASS::~LOWPASS ()
{
}

void LOWPASS::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  freq = 0.05;		// values between 0.0 and 0.5
  width = 0.2;		//values between 0.0001 and 0.2
  label = pluginName;
  customInput = "1";
}

void LOWPASS::calculate ()
{
  PlotLine *in = 0;
    
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("LOWPASS::calculate: no input");
    return;
  }

  if (in->getSize() == 0)
    return;
    
  PlotLine *out = new PlotLine;

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
    if (f <= freq)                 // Flat response
      wt = 1.0 ;
    else
    {
      dist = (f - freq) / width ;
      wt = exp ( -dist * dist ) ;
    }

    fftFreq->setData(i, fftFreq->getData(i) * wt) ;
    fftFreq->setData(halfn + i, fftFreq->getData(halfn + i) * wt) ;
  }

  dist = (0.5 - freq) / width ;     // Do Nyquist in fftFreq[0]
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

  if (! customFlag)
    delete in;
        
  delete fft;

  out->setColor(color);
  out->setType(lineType);
  out->setLabel(label);
  output->addLine(out);
}

int LOWPASS::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("LOWPASS Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  dialog->addFloatItem(QObject::tr("freq"), QObject::tr("Parms"), freq, .009, 99999999);
  dialog->addFloatItem(QObject::tr("width"), QObject::tr("Parms"), width, .009, 99999999);

  if (customFlag)
    dialog->addFormulaInputItem(QObject::tr("Input"), QObject::tr("Parms"), FALSE, customInput);

  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    freq = dialog->getFloat(QObject::tr("freq"));
    width = dialog->getFloat(QObject::tr("width"));
    label = dialog->getText(QObject::tr("Label"));

    if (customFlag)
      customInput = dialog->getFormulaInput(QObject::tr("Input"));
      
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  
  if (freq < 0.0)
    freq = 0.0;
  if (freq > 0.5)
    freq = 0.5;
  if (width < 0.0001)
    width = 0.0001;
  if (width > 0.2)
    width = 0.2;
  
  return rc;
}

void LOWPASS::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("freq");
  if (s.length())
    freq = s.toFloat();
    
  s = dict.getData("width");
  if (s.length())
    width = s.toFloat();

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
}

void LOWPASS::getIndicatorSettings(Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("label", label);
  dict.setData("freq", QString::number(freq));
  dict.setData("width", QString::number(width));
  dict.setData("plugin", pluginName);
  dict.setData("customInput", customInput);
}

PlotLine * LOWPASS::detrend(PlotLine *x, double &slope, double &intercept, bool detrend)
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

PlotLine * LOWPASS::raise2Power(PlotLine *x, double pad)
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

PlotLine * LOWPASS::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();

  if (output->getLines())
    return output->getLine(0);
  else
    return 0;
}

int LOWPASS::getMinBars ()
{
  int t = minBars + 20;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  LOWPASS *o = new LOWPASS;
  return ((IndicatorPlugin *) o);
}


