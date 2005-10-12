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

#include "VIDYA.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <math.h>
#include <qmemarray.h>

#define PI 3.14159265

VIDYA::VIDYA ()
{
  pluginName = "VIDYA";
  helpFile = "vidya.html";
    
  setDefaults();
}

VIDYA::~VIDYA ()
{
}

void VIDYA::setDefaults ()
{
  color.setNamedColor("cyan");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 14;
  volPeriod = 10;
}

void VIDYA::calculate ()
{
  PlotLine *in = data->getInput (BarData::Close);
	
  if ( in->getSize() < period )
  {
    qDebug("VIDYA::calculate: insufficient data");
    return;
  }
	
  PlotLine *out = new PlotLine;

  calcVidya ( out, in, volPeriod, period );

  out->setColor(color);
  out->setType(lineType);
  out->setLabel(pluginName);
  output->addLine(out);
}

void VIDYA::calcVidya ( PlotLine *outSignal, PlotLine *inSignal, int iCmoPeriod, int iVidyaPeriod )
{
  PlotLine *cmo = new PlotLine;
	
  calcCMO(cmo, inSignal, iCmoPeriod);
		
  int i = 0;
  int loop = (int)inSignal->getSize();
		
  QMemArray<double> *inSeries = new QMemArray<double>(loop);
  inSeries->fill(0.0);
  QMemArray<double> *offset = new QMemArray<double>(loop);
  offset->fill(0.0);
  QMemArray<double> *absCmo = new QMemArray<double>(loop);
  absCmo->fill(0.0);
  QMemArray<double> *vidya = new QMemArray<double>(loop);
  vidya->fill(0.0);
		
  double c = 2 / (double)iVidyaPeriod +1;
		
  for ( i = 0; i < loop; i++)
    inSeries->at(i) = inSignal->getData(i);
		
  int index = inSeries->size() -1;
  for (i = cmo->getSize() -1; i >= 0; i--)
  {
    absCmo->at(index) = fabs(cmo->getData(i) / 100);
    index--;
  }
		
  for (i = iCmoPeriod + iVidyaPeriod; i < (int)inSeries->size(); i++)		// period safty
  {
    vidya->at(i) = ( inSeries->at(i) * c * absCmo->at(i) ) + ( (1 - absCmo->at(i) * c) * vidya->at(i-1) );
    //!  (Price*Const*AbsCMO) + ((1-AbsCMO*Const)*VIDYA[1]),Price);
    outSignal->append(vidya->at(i));
  }
		
  delete inSeries;
  delete offset;
  delete absCmo;
  delete vidya;
  delete cmo;
}

void VIDYA::calcCMO ( PlotLine *outSignal, PlotLine *inSignal, int iPeriod)
{
  //!  Chande Momentum Oscillator
  //!  Raw VIDYA 
	
  int loop = (int)inSignal->getSize();
	
  QMemArray<double> *inSeries = new QMemArray<double>(loop);
  inSeries->fill(0.0);
  QMemArray<double> *offset = new QMemArray<double>(loop);
  offset->fill(0.0);
  QMemArray<double> *mom = new QMemArray<double>(loop);
  mom->fill(0.0);
  QMemArray<double> *posSeries = new QMemArray<double>(loop);
  posSeries->fill(0.0);
  QMemArray<double> *negSeries = new QMemArray<double>(loop);
  negSeries->fill(0.0);
  QMemArray<double> *sumPos = new QMemArray<double>(loop);
  sumPos->fill(0.0);
  QMemArray<double> *sumNeg = new QMemArray<double>(loop);
  sumNeg->fill(0.0);
  QMemArray<double> *cmoUp = new QMemArray<double>(loop);
  cmoUp->fill(0.0);
  QMemArray<double> *cmoDown = new QMemArray<double>(loop);
  cmoDown->fill(0.0);
  QMemArray<double> *rawCmo = new QMemArray<double>(loop);
  rawCmo->fill(0.0);
	
  int i = 0;
	
  for (i = 0; i < loop; i++)
    inSeries->at(i) = inSignal->getData(i);
	
  for (i = iPeriod - 1; i < loop; i++)
  {
    offset->at(i) = inSeries->at(i-1);
		
    mom->at(i) = inSeries->at(i) - offset->at(i);
		
    if (mom->at(i) > 0)
      posSeries->at(i) = mom->at(i);
    else
      posSeries->at(i) = 0;
		
    if (mom->at(i) < 0)
      negSeries->at(i) = fabs(mom->at(i));
    else
      negSeries->at(i) = 0;
		
    int j = 0;
    double sumUp = 0;
    double sumDown = 0;
		
    for (j = 0; j < iPeriod; j++)
    {
      sumUp += posSeries->at(i-j);
      sumDown += negSeries->at(i-j);
    }
		
    sumPos->at(i) = sumUp;
    sumNeg->at(i) = sumDown;
		
    cmoUp->at(i) = 100 * ((sumPos->at(i) - sumNeg->at(i)));
	
    cmoDown->at(i) = sumPos->at(i) + sumNeg->at(i);
		
    rawCmo->at(i) = cmoUp->at(i) / cmoDown->at(i);
		
    if (i > iPeriod -1)
      outSignal->append(rawCmo->at(i));
  }
			
  delete inSeries;
  delete offset;
  delete mom;
  delete posSeries;
  delete negSeries;
  delete sumPos;
  delete sumNeg;
  delete cmoUp;
  delete cmoDown;
  delete rawCmo;
}

void VIDYA::calcAdaptCMO ( PlotLine *outSignal, PlotLine *inSignal, int iStdPeriod,
                           int iMinLook, int iMaxLook)
{
  //! Chande Momentum Oscillator
  //! Adaptaave VIDYA 
  //! Not used here, but it has possibilities....
	
  PlotLine *currentLookback = new PlotLine;
	
  getStdDev(currentLookback, inSignal, iStdPeriod);
	
  getNorm( currentLookback, iMinLook, iMaxLook );
	
  int i = 0;
	
  for (i = 0; i < currentLookback->getSize(); i++)
    currentLookback->setData( i, (int)currentLookback->getData(i) );
	
  int loop = (int)inSignal->getSize();
	
  QMemArray<double> *inSeries = new QMemArray<double>(loop);
  inSeries->fill(0.0);
  QMemArray<double> *offset = new QMemArray<double>(loop);
  offset->fill(0.0);
  QMemArray<double> *mom = new QMemArray<double>(loop);
  mom->fill(0.0);
  QMemArray<double> *posSeries = new QMemArray<double>(loop);
  posSeries->fill(0.0);
  QMemArray<double> *negSeries = new QMemArray<double>(loop);
  negSeries->fill(0.0);
  QMemArray<double> *sumPos = new QMemArray<double>(loop);
  sumPos->fill(0.0);
  QMemArray<double> *sumNeg = new QMemArray<double>(loop);
  sumNeg->fill(0.0);
  QMemArray<double> *cmoUp = new QMemArray<double>(loop);
  cmoUp->fill(0.0);
  QMemArray<double> *cmoDown = new QMemArray<double>(loop);
  cmoDown->fill(0.0);
  QMemArray<double> *currentLook = new QMemArray<double>(loop);
  currentLook->fill(0.0);
  QMemArray<double> *adaptCmo = new QMemArray<double>(loop);
  adaptCmo->fill(0.0);
	
  // line up data
  int index = currentLook->size() -1;
  for (i = currentLookback->getSize() -1; i >=0; i--)
  {
    currentLook->at(index) = currentLookback->getData(i);
    index--;
  }
	
  for (i = 0; i < loop; i++)
    inSeries->at(i) = inSignal->getData(i);
	
  for (i = iStdPeriod - 1; i < loop; i++)
  {
    offset->at(i) = inSeries->at(i-1);
		
    mom->at(i) = inSeries->at(i) - offset->at(i);
		
    if (mom->at(i) > 0)
      posSeries->at(i) = mom->at(i);
    else
      posSeries->at(i) = 0;
		
    if (mom->at(i) < 0)
      negSeries->at(i) = fabs(mom->at(i));
    else
      negSeries->at(i) = 0;
		
    int j = 0;
    double sumUp = 0;
    double sumDown = 0;
		
    for (j = 0; j < (int)currentLook->at(i); j++)
    {
      sumUp += posSeries->at(i-j);
      sumDown += negSeries->at(i-j);
    }
		
    sumPos->at(i) = sumUp;
    sumNeg->at(i) = sumDown;
		
    cmoUp->at(i) = 100 * ((sumPos->at(i) - sumNeg->at(i)));
	
    cmoDown->at(i) = sumPos->at(i) + sumNeg->at(i);
		
    adaptCmo->at(i) = cmoUp->at(i) / cmoDown->at(i);
		
    if (i > iStdPeriod -1)
      outSignal->append(adaptCmo->at(i));
  }
			
  delete inSeries;
  delete offset;
  delete mom;
  delete posSeries;
  delete negSeries;
  delete sumPos;
  delete sumNeg;
  delete cmoUp;
  delete cmoDown;
  delete adaptCmo;
  delete currentLook;
  delete currentLookback;
}

void VIDYA::getStdDev( PlotLine *outLine, PlotLine *inLine,  int iPeriod )
{
  int loop;
  
  for (loop = iPeriod -1; loop < (int) inLine->getSize(); loop++)
  {
    double mean = 0;
    int loop2;
    for (loop2 = 0; loop2 < iPeriod; loop2++)
      mean += inLine->getData(loop - loop2);
		
    mean /= (double)iPeriod;
	
    double ds = 0;
    for (loop2 = 0; loop2 < iPeriod; loop2++)
    {
      double t = inLine->getData(loop - loop2) - mean;
      ds += (t * t);
    }
  
    ds = sqrt(ds / (double)period);
    outLine->append(ds);
  }
}

void VIDYA::getNorm( PlotLine *inSig, double iMin, double iMax)
{
  //	I = Imin + (Imax-Imin)*(V-Vmin)/(Vmax-Vmin)
	
  int i = 0;
  double range = 0;
  double max = -999999;
  double min = 999999;
  double norm = 0;
  for (i = 0; i < inSig->getSize(); i++)
  {
    if(inSig->getData(i) > max)
      max = inSig->getData(i);

    if(inSig->getData(i) < min)
      min = inSig->getData(i);
  }
	
  range = fabs(max) + fabs(min);
	
  for (i = 0; i < inSig->getSize(); i++)
  {	
    norm = iMin + (iMax - iMin) * ( (inSig->getData(i) - min) / (max - min) );
    inSig->setData(i, norm);
  }
}

int VIDYA::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Vidya period");
  QString per2 = QObject::tr("Volatility Period");
  QString il = QObject::tr("Input");
 
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("VIDYA Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addIntItem(perl, pl, period, 2, 50);
  dialog->addIntItem(per2, pl, volPeriod, 2, 50);
  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  
  pl = QObject::tr("Zones");
  dialog->createPage (pl);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    period = dialog->getInt(perl);	
    volPeriod = dialog->getInt(per2);	
	
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  
  return rc;
}

PlotLine * VIDYA::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, PERIOD, VOL_PERIOD

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 3)
    ;
  else
  {
    qDebug("VIDYA::calculateCustom: invalid parm count");
    return 0;
  }

  bool ok;
  int t = l[1].toInt(&ok);
  if (ok)
    period = t;
  else
  {
    qDebug("VIDYA::calculateCustom: invalid PERIOD parm");
    return 0;
  }

  t = l[2].toInt(&ok);
  if (ok)
    volPeriod = t;
  else
  {
    qDebug("VIDYA::calculateCustom: invalid VOL_PERIOD parm");
    return 0;
  }

  clearOutput();

  if (! d.count())
  {
    qDebug("VIDYA::calculateCustom: no input");
    return 0;
  }

  PlotLine *out = new PlotLine;
  calcVidya (out, d.at(0), volPeriod, period);
  output->addLine(out);

  return output->getLine(0);
}

void VIDYA::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("label", label);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("plugin", pluginName);
  dict.setData("period", QString::number(period));
  dict.setData("volPeriod", QString::number(volPeriod));
}

void VIDYA::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("label");
  if (s.length())
    label = s;
        
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();
	
  s = dict.getData("period");
  if (s.length())
    period = s.toInt();
	
  s = dict.getData("volPeriod");
  if (s.length())
    volPeriod = s.toInt();
}

int VIDYA::getMinBars ()
{
  int t = minBars + period;
  return t;
}

//****************************************************
//****************************************************
//****************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  VIDYA *o = new VIDYA;
  return ((IndicatorPlugin *) o);
}



