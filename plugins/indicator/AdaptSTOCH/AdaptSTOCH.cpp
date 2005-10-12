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

#include "AdaptSTOCH.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <math.h>


AdaptSTOCH::AdaptSTOCH ()
{
  pluginName = "AdaptSTOCH";
  helpFile = "adaptstoch.html";
  
  setDefaults();
}

AdaptSTOCH::~AdaptSTOCH ()
{
}

void AdaptSTOCH::setDefaults ()
{
  dcolor.setNamedColor("blue");
  kcolor.setNamedColor("red");
  buyColor.setNamedColor("gray");
  sellColor.setNamedColor("gray");
  dlineType = PlotLine::Line;
  dlabel = "%D";
  dperiod = 3;
  klineType = PlotLine::Line;
  klabel = "%K";
  kperiod = 0;
  
  minLookback = 5;
  maxLookback = 20;
  
  kMaType = 0;
  dMaType = 0;
  
  period = 14;
  buyLine = 20;
  sellLine = 80;
  label = pluginName;
}

void AdaptSTOCH::calculate ()
{
  PlotLine *in = data->getInput (BarData::Close);
  if (! in)
    return;

  calculate2(in);
  delete in;
}

void AdaptSTOCH::calculate2 (PlotLine *in)
{
  if ( in->getSize() < ( period + maxLookback + 5) )
  {
    qDebug("AdaptSTOCH::calculate: insufficient data");
    return;
  }
  
  //  Calculate 20-day std. Dev. And its 20-day range 
  PlotLine *v1 = getStdDev(in, period);
  PlotLine *v2 = getHighest(v1, period);
  PlotLine *v3 = getLowest(v1, period); 
	
  // Create v4: stochastic oscillator for 20-day std. dev.
  // if v1=v2 (highest level) => v4 = 1; if v1=v3 (lowest level) => v4=0 

  PlotLine *v4 = new PlotLine;
	
  int i = 0;
	
  for (i = 0; i < v2->getSize(); i++)
  {
    if ( (v2->getData(i) - v3->getData(i)) > 0)
      v4->append( (v1->getData(i) - v3->getData(i)) / (v2->getData(i) - v3->getData(i)) );
    else
      v4->append(0);
  }

  // Calculate current effective length; if v4 = 1, then length = mininum 

  PlotLine *currentLength = new PlotLine;;
	
  for (i = 0; i < v4->getSize(); i++)
    currentLength->append ( (int) ( minLookback + (maxLookback - minLookback) * (1 - v4->getData(i)) ) );
	
  // now build indicator
  double stoch = 0;

  PlotLine *aStoch = new PlotLine;
	
  // work backwards to insure alignment
  int index = in->getSize() -1;
  for (i = currentLength->getSize() - 1; i >= 0; i--)
  {
    double hh = -999999;
    double ll = 999999;
    int loop2;
		
    for (loop2 = 0; loop2 < (int)currentLength->getData(i); loop2++)		// hihest high
    {			
      if ( data->getHigh(index - loop2) > hh)
        hh = data->getHigh(index - loop2);
	
      if ( data->getLow(index - loop2) < ll)
        ll = data->getLow(index - loop2);
    }
				
    if ( (hh-ll) > 0 )
      stoch = ( ( (data->getClose(index) - ll)/(hh - ll)) * 100 );
    else
    {
      stoch = 0;
      qDebug("AdaptSTOCH::calculate: data error: high < low");
    }
		
    aStoch->prepend(stoch);
    index--;
  }
	
	
  if (kperiod > 1)
  {
    PlotLine *aStoch2 = getMA(aStoch, kMaType, kperiod, 0, 0);
    delete aStoch;
    aStoch = aStoch2;
  }
	
  aStoch->setColor(kcolor);
  aStoch->setType(klineType);
  aStoch->setLabel(klabel);
	
  output->addLine(aStoch);

  delete v1;
  delete v2;
  delete v3; 
  delete v4;
  delete currentLength;

  if (dperiod > 1)
  {
    PlotLine *d = getMA(aStoch, dMaType, dperiod, 0, 0);
    d->setColor(dcolor);
    d->setType(dlineType);
    d->setLabel(dlabel);
    output->addLine(d);
  }
  
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

PlotLine *AdaptSTOCH::getHighest( PlotLine *line,  int period)
{
  int loop;
  
  PlotLine *lineHigh = new PlotLine;
  
  for (loop = period -1; loop < (int) line->getSize(); loop++)
  {
  
    double highest = -999999;
    int loop2;
    
    for (loop2 = 0; loop2 < period; loop2++)
    {
      if ( line->getData(loop - loop2) > highest)
	highest = line->getData(loop - loop2);
    }
  
    lineHigh->append(highest);
  }
  return lineHigh;
}

PlotLine *AdaptSTOCH::getLowest( PlotLine *line,  int period)
{
  int loop;
  
  PlotLine *lineLow = new PlotLine;
  
  for (loop = period -1; loop < (int) line->getSize(); loop++)
  {
    double lowest = 999999;
    int loop2;
    
    for (loop2 = 0; loop2 < period; loop2++)
    {
      if ( line->getData(loop - loop2) < lowest)
	lowest = line->getData(loop - loop2);
    }
  
    lineLow->append(lowest);
  }
  return lineLow;
}

PlotLine *AdaptSTOCH::getStdDev( PlotLine *line,  int period )
{
  PlotLine *std = new PlotLine;
 
  int loop;
  
  for (loop = period -1; loop < (int) line->getSize(); loop++)
  {
    double mean = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
    mean += line->getData(loop - loop2);
		
    mean /= (double)period;
	
    double ds = 0;
    for (loop2 = 0; loop2 < period; loop2++)
    {
      double t = line->getData(loop - loop2) - mean;
      ds += (t * t);
    }
  
    ds = sqrt(ds / (double)period);
    std->append(ds);
  }
  return std;
}

int AdaptSTOCH::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString ll = QObject::tr("Label");
  QString perl = QObject::tr("Period");
  QString il = QObject::tr("Input");
  QString bzc = QObject::tr("Buy Zone Color");
  QString szc = QObject::tr("Sell Zone Color");
  QString bz = QObject::tr("Buy Zone");
  QString sz = QObject::tr("Sell Zone");
  
  QString perMin = QObject::tr("Min Lookback Period");
  QString perMax = QObject::tr("Max Lookback Period");
  
  QString stk = QObject::tr("%K Smoothing Type");
  QString std = QObject::tr("%D Smoothing Type");
  
  QString perk = QObject::tr("%K Period");
  QString perd = QObject::tr("%D Period");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("AdaptSTOCH Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  dialog->addIntItem(perMin, pl, minLookback, 0, 99999999);
  dialog->addIntItem(perMax, pl, maxLookback, 0, 99999999);
  
  pl = QObject::tr("%K Parms");
  dialog->createPage (pl);
  QString t = QObject::tr("%K Color");
  dialog->addColorItem(t, pl, kcolor);
  t = QObject::tr("%K Line Type");
  dialog->addComboItem(t, pl, lineTypes, klineType);
  t = QObject::tr("%K Label");
  dialog->addTextItem(t, pl, klabel);
  
  QStringList l2 = getMATypes();
  dialog->addComboItem(stk, pl, l2, kMaType);
  dialog->addIntItem(perk, pl, kperiod, 0, 99999999);
  
  pl = QObject::tr("%D Parms");
  dialog->createPage (pl);
  t = QObject::tr("%D Color");
  dialog->addColorItem(t, pl, dcolor);
  t = QObject::tr("%D Line Type");
  dialog->addComboItem(t, pl, lineTypes, dlineType);
  t = QObject::tr("%D Label");
  dialog->addTextItem(t, pl, dlabel);
  
  QStringList l3 = getMATypes();
  dialog->addComboItem(std, pl, l3, dMaType);

  dialog->addIntItem(perd, pl, dperiod, 0, 99999999);
  
  pl = QObject::tr("Zones");
  dialog->createPage (pl);
  dialog->addColorItem(bzc, pl, buyColor);
  dialog->addColorItem(szc, pl, sellColor);
  dialog->addIntItem(bz, pl, buyLine, 0, 100);
  dialog->addIntItem(sz, pl, sellLine, 0, 100);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    t = QObject::tr("%D Color");
    dcolor = dialog->getColor(t);
    t = QObject::tr("%D Line Type");
    dlineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("%D Period");
    dperiod = dialog->getInt(t);
    t = QObject::tr("%D Label");
    dlabel = dialog->getText(t);
    t = QObject::tr("%K Color");
    kcolor = dialog->getColor(t);
    t = QObject::tr("%K Line Type");
    klineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("%K Period");
    kperiod = dialog->getInt(t);
    t = QObject::tr("%K Label");
    klabel = dialog->getText(t);
    period = dialog->getInt(perl);	
    t = QObject::tr("Min Lookback Period");
    minLookback = dialog->getInt(t);
    t = QObject::tr("Max Lookback Period");
    maxLookback = dialog->getInt(t);
	
    kMaType = dialog->getComboIndex(stk);
    dMaType = dialog->getComboIndex(std);
	
    buyColor = dialog->getColor(bzc);
    sellColor = dialog->getColor(szc);
    buyLine = dialog->getInt(bz);
    sellLine = dialog->getInt(sz);
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void AdaptSTOCH::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("dcolor");
  if (s.length())
    dcolor.setNamedColor(s);
    
  s = dict.getData("kcolor");
  if (s.length())
    kcolor.setNamedColor(s);
  
  s = dict.getData("buyColor");
  if (s.length())
    buyColor.setNamedColor(s);
  
  s = dict.getData("sellColor");
  if (s.length())
    sellColor.setNamedColor(s);
  
  s = dict.getData("dlineType");
  if (s.length())
    dlineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("klineType");
  if (s.length())
    klineType = (PlotLine::LineType) s.toInt();
  
  s = dict.getData("period");
  if (s.length())
    period = s.toInt();

  s = dict.getData("dperiod");
  if (s.length())
    dperiod = s.toInt();
  
  s = dict.getData("kperiod");
  if (s.length())
    kperiod = s.toInt();
  
  s = dict.getData("dlabel");
  if (s.length())
    dlabel = s;
      
  s = dict.getData("klabel");
  if (s.length())
    klabel = s;

  s = dict.getData("buyLine");
  if (s.length())
    buyLine = s.toInt();

  s = dict.getData("sellLine");
  if (s.length())
    sellLine = s.toInt();

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;

  s = dict.getData("label");
  if (s.length())
    label = s;
	
  s = dict.getData("kMaType");
  if (s.length())
    kMaType = s.toInt();
	
  s = dict.getData("dMaType");
  if (s.length())
    dMaType = s.toInt();
	
  s = dict.getData("minLookback");
  if (s.length())
    minLookback = s.toInt();   
	
  s = dict.getData("maxLookback");
  if (s.length())
    maxLookback = s.toInt();   
}

void AdaptSTOCH::getIndicatorSettings (Setting &dict)
{
  dict.setData("dcolor", dcolor.name());
  dict.setData("buyColor", buyColor.name());
  dict.setData("sellColor", sellColor.name());
  dict.setData("dlineType", QString::number(dlineType));
  dict.setData("dperiod", QString::number(dperiod));
  dict.setData("dlabel", dlabel);
  dict.setData("kcolor", kcolor.name());
  dict.setData("klineType", QString::number(klineType));
  dict.setData("kperiod", QString::number(kperiod));
  dict.setData("klabel", klabel);
  dict.setData("period", QString::number(period));
  dict.setData("buyLine", QString::number(buyLine));
  dict.setData("sellLine", QString::number(sellLine));
  dict.setData("customInput", customInput);
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
  dict.setData("kMaType", QString::number(kMaType));
  dict.setData("dMaType", QString::number(dMaType));
  dict.setData("minLookback", QString::number(minLookback));
  dict.setData("maxLookback", QString::number(maxLookback));
}

PlotLine * AdaptSTOCH::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, K_MA_TYPE, D_MA_TYPE, PERIOD, K_PERIOD, D_PERIOD, MIN_LOOKBACK, MAX_LOOKBACK

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 8)
    ;
  else
  {
    qDebug("AdaptSTOCH::calculateCustom: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("AdaptSTOCH::calculateCustom: no input");
    return 0;
  }

  QStringList mal = getMATypes();
  if (mal.findIndex(l[1]) == -1)
  {
    qDebug("AdaptSTOCH::calculateCustom: invalid K_MA_TYPE parm");
    return 0;
  }
  else
    kMaType = mal.findIndex(l[1]);

  if (mal.findIndex(l[2]) == -1)
  {
    qDebug("AdaptSTOCH::calculateCustom: invalid D_MA_TYPE parm");
    return 0;
  }
  else
    dMaType = mal.findIndex(l[2]);

  bool ok;
  int t = l[3].toInt(&ok);
  if (ok)
    period = t;
  else
  {
    qDebug("AdaptSTOCH::calculateCustom: invalid PERIOD parm");
    return 0;
  }

  t = l[4].toInt(&ok);
  if (ok)
    kperiod = t;
  else
  {
    qDebug("AdaptSTOCH::calculateCustom: invalid K_PERIOD parm");
    return 0;
  }

  t = l[5].toInt(&ok);
  if (ok)
    dperiod = t;
  else
  {
    qDebug("AdaptSTOCH::calculateCustom: invalid D_PERIOD parm");
    return 0;
  }

  t = l[6].toInt(&ok);
  if (ok)
    minLookback = t;
  else
  {
    qDebug("AdaptSTOCH::calculateCustom: invalid MIN_LOOKBACK parm");
    return 0;
  }

  t = l[7].toInt(&ok);
  if (ok)
    maxLookback = t;
  else
  {
    qDebug("AdaptSTOCH::calculateCustom: invalid MAX_LOOKBACK parm");
    return 0;
  }

  clearOutput();
  calculate2(d.at(0));
  return output->getLine(0);
}

int AdaptSTOCH::getMinBars ()
{
  int t = minBars + period + dperiod + kperiod;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  AdaptSTOCH *o = new AdaptSTOCH;
  return ((IndicatorPlugin *) o);
}




