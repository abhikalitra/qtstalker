/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. SQObject::tratigakos
 *
 *  This program is free software; you can redisQObject::tribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is disQObject::tributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "SINWAV.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <math.h>
#include <qmemarray.h>

#define PI 3.14159265

SINWAV::SINWAV ()
{
  pluginName = "SINWAV";
  helpFile = "sinwav.html";
    
  setDefaults();
}

SINWAV::~SINWAV ()
{
}

void SINWAV::setDefaults ()
{
  colorSin.setNamedColor("red");
  colorLead.setNamedColor("blue");
  lineTypeSin = PlotLine::Line;
  lineTypeLead = PlotLine::Line;
  labelSin = pluginName;
  labelLead = "Lead";
}

void SINWAV::calculate ()
{
  // Ehler's sine wave 
  
  PlotLine *Price = new PlotLine;
	
  int i = 0;
	
  // price = (h + l) / 2
  for (i = 0; i < (int) data->count(); i++)
    Price->append((data->getHigh(i) + data->getLow(i)) / 2);
		
  //! VERSION 2
		
  QMemArray<double> *smooth = new QMemArray<double>(Price->getSize());
  smooth->fill(0.0);
  QMemArray<double> *detrender = new QMemArray<double>(Price->getSize());
  detrender->fill(0.0);
  QMemArray<double> *period = new QMemArray<double>(Price->getSize());
  period->fill(0.0);
  QMemArray<double> *Q1 = new QMemArray<double>(Price->getSize());
  Q1->fill(0.0);
  QMemArray<double> *I1 = new QMemArray<double>(Price->getSize());
  I1->fill(0.0);
  QMemArray<double> *jI = new QMemArray<double>(Price->getSize());
  jI->fill(0.0);
  QMemArray<double> *jQ = new QMemArray<double>(Price->getSize());
  jQ->fill(0.0);
  QMemArray<double> *I2 = new QMemArray<double>(Price->getSize());
  I2->fill(0.0);
  QMemArray<double> *Q2 = new QMemArray<double>(Price->getSize());
  Q2->fill(0.0);
  QMemArray<double> *Re = new QMemArray<double>(Price->getSize());
  Re->fill(0.0);
  QMemArray<double> *Im = new QMemArray<double>(Price->getSize());
  Im->fill(0.0);
  QMemArray<double> *SmoothPrice = new QMemArray<double>(Price->getSize());
  SmoothPrice->fill(0.0);
  QMemArray<double> *DCPhase = new QMemArray<double>(Price->getSize());
  DCPhase->fill(0.0);
	
  PlotLine *out1 = new PlotLine;
  PlotLine *out2 = new PlotLine;
	
  double SmoothPeriod = 0;
  double DCPeriod = 0;
	
  for (i = 6; i< Price->getSize(); i++)
  {
    // Weighted price
    smooth->at(i) = ( 4 * Price->getData(i) + 3 * Price->getData(i-1) + 
		      2 * Price->getData(i-2) +  Price->getData(i-3)) /10.0;
						
    detrender->at(i) = (0.0962 * smooth->at(i) + 0.5769 * smooth->at(i-2) - 
			0.5769 * smooth->at(i-4) - 0.0962 * smooth->at(i-6)) *
		        (0.075 * period->at(i-1) + 0.54);
		
    // Compute InPhase and Quadrature components
    Q1->at(i) = (0.0962 * detrender->at(i) + 0.5769 * detrender->at(i-2) - 
		 0.5769 * detrender->at(i-4) - 0.0962 * detrender->at(i-6)) *
		 (0.075 * period->at(i-1) + 0.54);
    I1->at(i) = detrender->at(i-3);
		
    //Advance the phase of I1 and Q1 by 90 degrees
    jI->at(i) = (0.0962 * I1->at(i) + 0.5769 * I1->at(i-2) - 
		 0.5769 * I1->at(i-4) - 0.0962 * I1->at(i-6)) *
		 (0.075 * period->at(i-1) + 0.54);
			 
    Q1->at(i) = (0.0962 * Q1->at(i) + 0.5769 * Q1->at(i-2) - 
		 0.5769 * Q1->at(i-4) - 0.0962 * Q1->at(i-6)) *
		 (0.075 * period->at(i-1) + 0.54);
					 
    // Phasor addition for 3-bar averaging	
    I2->at(i) = I1->at(i) - jQ->at(i);	 
    Q2->at(i) = Q1->at(i) + jI->at(i);	
		
    // Smooth the I and Q components before applying the discriminator
    I2->at(i) = 0.2 * I2->at(i) + 0.8 * I2->at(i-1);
    Q2->at(i) = 0.2 * Q2->at(i) + 0.8 * Q2->at(i-1);
		
    // Homodyne Discriminator
    Re->at(i) = I2->at(i) * I2->at(i-1) + Q2->at(i) * Q2->at(i-1);
    Im->at(i) = I2->at(i) * Q2->at(i-1) - Q2->at(i) * I2->at(i-1);
    Re->at(i) = 0.2 * Re->at(i) + 0.8 * Re->at(i-1);
    Im->at(i) = 0.2 * Im->at(i) + 0.8 * Im->at(i-1);
    if (Im->at(i) != 0 && Re->at(i) != 0 )
      period->at(i) = 360/(atan(Im->at(i) / Re->at(i)) * (180/PI));
    if (period->at(i) > 1.5 * period->at(i-1))
      period->at(i) = 1.5 * period->at(i-1);
    if (period->at(i) < 0.67 * period->at(i-1))
      period->at(i) = 0.67 * period->at(i-1);
    if (period->at(i) < 6)
      period->at(i) = 6;
    if (period->at(i) > 50)
      period->at(i) = 50;
	
    period->at(i) = 0.2 * period->at(i) + 0.8 * period->at(i-1);
		
    SmoothPeriod = 0.33 * period->at(i) + 0.67 * SmoothPeriod;
		
    // Compute Dominant CyclePhase
    SmoothPrice->at(i) = (4 * Price->getData(i) + 3 * Price->getData(i-1) + 
			  2 * Price->getData(i-2) +  Price->getData(i-3)) /10.0;
								  
    DCPeriod = (int)(SmoothPeriod + 0.5);
		
    double RealPart = 0;
    double ImagPart = 0;
    int j = 0;
		
    if (i >= (int)DCPeriod)
    {
      for (j = 0; j < (int)DCPeriod; j++)
      {
        RealPart = RealPart + sin((360 * (double)j / DCPeriod) * (PI/180)) * (SmoothPrice->at(i - j));
	ImagPart = ImagPart + cos((360 * (double)j / DCPeriod) * (PI/180)) * (SmoothPrice->at(i - j));
      }
    }
		
    if (fabs(ImagPart) > 0.001)
      DCPhase->at(i) = atan(RealPart / ImagPart) * (180/PI);
			
    if( fabs(ImagPart) <= 0.001 )
      DCPhase->at(i) = 90.0 * sin(RealPart * (PI/180));
			 
    DCPhase->at(i) = DCPhase->at(i) + 90.0;
			
    // Compensate for one bar lag of the Weighted Moving Average
    DCPhase->at(i) = DCPhase->at(i) + 360.0 / SmoothPeriod;
		
    if (ImagPart < 0) 
      DCPhase->at(i) = DCPhase->at(i) + 180.0;
			
    if (DCPhase->at(i) > 315.0)
      DCPhase->at(i) = DCPhase->at(i) - 360.0;
		
    // we're done....
    out1->append(sin(DCPhase->at(i) * (PI/180)));
    out2->append(sin((DCPhase->at(i) + 45.0) * (PI/180)));
  }
	
  delete smooth;
  delete detrender;
  delete period;
  delete Q1;
  delete I1;
  delete jI;
  delete jQ;
  delete I2;
  delete Q2;
  delete Re;
  delete Im;
  delete SmoothPrice;
  delete DCPhase;
  delete Price;
	
  out1->setColor(colorSin);
  out1->setLabel(labelSin);
  out1->setType(lineTypeSin);
  output->addLine(out1);
	
  out2->setColor(colorLead);
  out2->setLabel(labelLead);
  out2->setType(lineTypeLead);
  output->addLine(out2);
}

int SINWAV::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Sine Color");
  QString c2 = QObject::tr("Lead Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Sine Line Type");
  QString lt2 = QObject::tr("Lead Line Type");
  
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("SINWAV Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, colorSin);
  dialog->addComboItem(ltl, pl, lineTypes, lineTypeSin);
  dialog->addColorItem(c2, pl, colorLead);
  dialog->addComboItem(lt2, pl, lineTypes, lineTypeLead);
  dialog->addTextItem(ll, pl, labelSin);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    colorSin = dialog->getColor(cl);
    colorLead = dialog->getColor(c2);
    lineTypeSin = (PlotLine::LineType) dialog->getComboIndex(ltl);
    lineTypeLead = (PlotLine::LineType) dialog->getComboIndex(lt2);
    labelSin = dialog->getText(ll);
   
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

PlotLine * SINWAV::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output->getLine(0);
}

void SINWAV::getIndicatorSettings (Setting &dict)
{
  dict.setData("colorSin", colorSin.name());
  dict.setData("colorLead", colorLead.name());
  dict.setData("labelSin", labelSin);
  dict.setData("labelLead", labelLead);
  dict.setData("lineTypeSin", QString::number(lineTypeSin));
  dict.setData("lineTypeLead", QString::number(lineTypeLead));
  dict.setData("plugin", pluginName);
}

void SINWAV::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("colorSin");
  if (s.length())
    colorSin.setNamedColor(s);
	
  s = dict.getData("colorLead");
  if (s.length())
    colorLead.setNamedColor(s);
    
  s = dict.getData("labelSin");
  if (s.length())
    labelSin = s;
	
   s = dict.getData("labelLead");
  if (s.length())
    labelLead = s;
        
  s = dict.getData("lineTypeSin");
  if (s.length())
    lineTypeSin = (PlotLine::LineType) s.toInt();
	
  s = dict.getData("lineTypeLead");
  if (s.length())
    lineTypeLead = (PlotLine::LineType) s.toInt();
}

IndicatorPlugin * createIndicatorPlugin ()
{
  SINWAV *o = new SINWAV;
  return ((IndicatorPlugin *) o);
}



