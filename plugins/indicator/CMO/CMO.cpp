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

#include "CMO.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <math.h>
#include <qmemarray.h>
#include <qmessagebox.h>

#define PI 3.14159265

CMO::CMO ()
{
  pluginName = "CMO";
  helpFile = "cmo.html";
    
  setDefaults();
  
}

CMO::~CMO ()
{
}

void CMO::setDefaults ()
{
  color.setNamedColor("blue");
  buyColor.setNamedColor("gray");
  sellColor.setNamedColor("gray");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 14;
  buyLine = 50;
  sellLine = -50;
  adaptFlag = FALSE;
  minLookback = 7;
  maxLookback = 10;
}

void CMO::calculate ()
{
  PlotLine *in = data->getInput (BarData::Close);
  if (! in)
  {
    qDebug("CMO::calculate: no input");
    return;
  }

  calculate2(in);
  delete in;
}

void CMO::calculate2 (PlotLine *in)
{
  if ( in->getSize() < period )
  {
    qDebug("CMO::calculate: insufficient data");
    return;
  }
	
  PlotLine *out = new PlotLine;
	
  if ( !adaptFlag )
    calcCMO(out, in, period);
  else
    calcAdaptCMO(out, in, period, minLookback, maxLookback);
	
  out->setColor(color);
  out->setType(lineType);
	
  if (adaptFlag)
  {
    QString labelAdapt = "AdaptCMO";		
    out->setLabel(labelAdapt);
  }
  else
    out->setLabel(pluginName);
	
  output->addLine(out);
	
  if (buyLine)
  {
    PlotLine *bline = new PlotLine();
    bline->setColor(buyColor);
    bline->setType(PlotLine::Horizontal);
    bline->append(buyLine);
    output->addLine(bline);
  }

  if (sellLine)
  {
    PlotLine *sline = new PlotLine();
    sline->setColor(sellColor);
    sline->setType(PlotLine::Horizontal);
    sline->append(sellLine);
    output->addLine(sline);
  }
}

void CMO::calcCMO ( PlotLine *outSignal, PlotLine *inSignal, int iPeriod)
{
  //!  Chande Momentum Oscillator
  //!  Raw CMO 
	
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

void CMO::calcAdaptCMO (PlotLine *outSignal, PlotLine *inSignal, int iStdPeriod, int iMinLook, int                                  iMaxLook)
{
  //! Chande Momentum Oscillator
  //! Adaptaave CMO 
	
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

void CMO::getStdDev( PlotLine *outLine, PlotLine *inLine,  int iPeriod )
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

void CMO::getNorm( PlotLine *inSig, double iMin, double iMax)
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


int CMO::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString il = QObject::tr("Input");
  QString ck1 = QObject::tr("Adaptive CMO");
  QString per2 = QObject::tr("Min Lookback");
  QString per3 = QObject::tr("Max Lookback");
  
  QString bzc = QObject::tr("Buy Zone Color");
  QString szc = QObject::tr("Sell Zone Color");
  QString bz = QObject::tr("Buy Zone");
  QString sz = QObject::tr("Sell Zone");
 
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("CMO Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addIntItem(perl, pl, period, 2, 99999999);
  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addCheckItem(ck1, pl, adaptFlag);
  dialog->addIntItem(per2, pl, minLookback, 2, 100);
  dialog->addIntItem(per3, pl, maxLookback, 2, 100);
  
  pl = QObject::tr("Zones");
  dialog->createPage (pl);
  dialog->addColorItem(bzc, pl, buyColor);
  dialog->addColorItem(szc, pl, sellColor);
  dialog->addIntItem(bz, pl, buyLine, 0, 100);
  dialog->addIntItem(sz, pl, sellLine, -100, 0);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    period = dialog->getInt(perl);	
	
    buyColor = dialog->getColor(bzc);
    sellColor = dialog->getColor(szc);
    buyLine = dialog->getInt(bz);
    sellLine = dialog->getInt(sz);
	
    adaptFlag = dialog->getCheck(ck1);
    minLookback = dialog->getInt(per2);
    maxLookback = dialog->getInt(per3);
	
    if (minLookback > period || maxLookback > period)
    {
      //QString message = (tr("Adapive CMO"));
      QMessageBox::information( 0, QString("Adapive CMO"),
      QString("Both Max Lookback and Min Lookback must be less than or equal to Period.\n"
    	      "Default values will be used.") );
		
      period = 14;
      minLookback = 7;
      maxLookback = 10;
    }
	
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  
  return rc;
}

PlotLine * CMO::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, PERIOD, ADAPTFLAG, MIN_LOOKBACK, MAXLOOKBACK

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 5)
    ;
  else
  {
    qDebug("CMO::calculateCustom: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("CMO::calculateCustom: no input");
    return 0;
  }

  bool ok;
  int t = l[1].toInt(&ok);
  if (ok)
    period = t;
  else
  {
    qDebug("CMO::calculateCustom: invalid PERIOD parm");
    return 0;
  }

  if (! l[2].compare("TRUE"))
    adaptFlag = TRUE;
  else
  {
    if (! l[2].compare("FALSE"))
      adaptFlag = FALSE;
    else
    {
      qDebug("CMO::calculateCustom: invalid ADAPTFLAG parm");
      return 0;
    }
  }

  t = l[3].toInt(&ok);
  if (ok)
    minLookback = t;
  else
  {
    qDebug("CMO::calculateCustom: invalid MIN_LOOKBACK parm");
    return 0;
  }

  t = l[4].toInt(&ok);
  if (ok)
    maxLookback = t;
  else
  {
    qDebug("CMO::calculateCustom: invalid MAX_LOOKBACK parm");
    return 0;
  }

  clearOutput();
  calculate2(d.at(0));
  return output->getLine(0);
}

void CMO::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("buyColor", buyColor.name());
  dict.setData("sellColor", sellColor.name());
  dict.setData("label", label);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("plugin", pluginName);
  dict.setData("period", QString::number(period));
  dict.setData("adaptFlag", QString::number(adaptFlag));
  dict.setData("minLookback", QString::number(minLookback));
  dict.setData("maxLookback", QString::number(maxLookback));
}

void CMO::setIndicatorSettings (Setting &dict)
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
	
  s = dict.getData("minLookback");
  if (s.length())
    minLookback = s.toInt();
	
  s = dict.getData("maxLookback");
  if (s.length())
    maxLookback = s.toInt();
	
  s = dict.getData("adaptFlag");
  if (s.length())
    adaptFlag = s.toInt();
	
  s = dict.getData("buyColor");
  if (s.length())
    buyColor.setNamedColor(s);
  
  s = dict.getData("sellColor");
  if (s.length())
    sellColor.setNamedColor(s);
}

int CMO::getMinBars ()
{
  int t = minBars + period;
  return t;
}

//***************************************************
//***************************************************
//***************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CMO *o = new CMO;
  return ((IndicatorPlugin *) o);
}



