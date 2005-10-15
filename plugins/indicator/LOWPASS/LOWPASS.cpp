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

#include "LOWPASS.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <math.h>

#define MAXNUM 2147483647

#if ! defined ( PI )
#define PI 3.141592653589793
#endif

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
  label = pluginName;
  freq = 0.05;		// values between 0.0 and 0.5
  width = 0.2;		//values between 0.0001 and 0.2
  input = BarData::Close;
}

void LOWPASS::calculate ()
{
  PlotLine *in = data->getInput(input);
  if (! in)
  {
    qDebug("LOWPASS::calculate: no input");
    return;
  }

  calculate2(in, freq, width);
  delete in;
}

void LOWPASS::calculate2 (PlotLine *in, double fre, double wid)
{
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
  
  out->setColor(color);
  out->setType(lineType);
  out->setLabel(label);
  output->addLine(out);
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


int LOWPASS::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString fl = QObject::tr("Freq");
  QString wl = QObject::tr("Width");
  QString il = QObject::tr("Input");
  
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("LOWPASS Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addComboItem(il, pl, inputTypeList, input);
  dialog->addDoubleItem(fl, pl, freq, 0, 0.5);
  dialog->addDoubleItem(wl, pl, width, 0.0001, 0.2);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    input = (BarData::InputType) dialog->getComboIndex(il);

    freq = dialog->getDouble(fl);
    if (freq < 0.0)
      freq = 0.0;
    if (freq > 0.5)
      freq = 0.5;

    width = dialog->getDouble(wl);
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

PlotLine * LOWPASS::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format: DATA_ARRAY, FREQ, WIDTH

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 3)
    ;
  else
  {
    qDebug("LOWPASS::calculateCustom: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("LOWPASS::calculateCustom: no input");
    return 0;
  }

  bool ok;
  double t2 = l[1].toDouble(&ok);
  if (ok)
  {
    if (t2 < 0.0)
      t2 = 0.0;
    if (t2 > 0.5)
      t2 = 0.5;
    freq = t2;
  }
  else
  {
    qDebug("LOWPASS::calculateCustom: invalid FREQ parm");
    return 0;
  }

  t2 = l[2].toDouble(&ok);
  if (ok)
  {
    if (t2 < 0.0001)
      t2 = 0.0001;
    if (t2 > 0.2)
      t2 = 0.2;
    width = t2;
  }
  else
  {
    qDebug("LOWPASS::calculateCustom: invalid WIDTH parm");
    return 0;
  }

  clearOutput();
  calculate2(d.at(0), freq, width);
  return output->getLine(0);
}

void LOWPASS::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("label", label);
  dict.setData("input", QString::number(input));
  dict.setData("plugin", pluginName);
  dict.setData("freq", QString::number(freq));
  dict.setData("width", QString::number(width));
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
      
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();

  s = dict.getData("freq");
  if (s.length())
    freq = s.toFloat();
    
  s = dict.getData("width");
  if (s.length())
    width = s.toFloat();
}

int LOWPASS::getMinBars ()
{
  int t = minBars;
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



