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

#include "STOCH.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <math.h>

STOCH::STOCH ()
{
  pluginName = "STOCH";
  helpFile = "stoch.html";

  methodList.append("Standard");
  methodList.append("Singular");
  methodList.append("Adaptive");
  
  setDefaults();
}

STOCH::~STOCH ()
{
}

void STOCH::setDefaults ()
{
  dcolor.setNamedColor("yellow");
  kcolor.setNamedColor("red");
  buyColor.setNamedColor("gray");
  sellColor.setNamedColor("gray");
  dlineType = PlotLine::Dash;
  dlabel = "%D";
  dperiod = 3;
  klineType = PlotLine::Line;
  klabel = "%K";
  kperiod = 3;
  period = 14;
  buyLine = 20;
  sellLine = 80;
  label = pluginName;
  input = BarData::Close;
  minLookback = 5;
  maxLookback = 20;
  kMaType = 0;
  dMaType = 0;
  method = "Standard";
}

void STOCH::calculate ()
{
  if (! method.compare("Standard"))
    calculateStandard();
  else
  {
    PlotLine *in = data->getInput(input);
    if (! in)
    {
      qDebug("STOCH::calculate: no input");
      return;
    }

    if (! method.compare("Singular"))
      calculateSingular(in);
    else
      calculateAdaptive(in);

    delete in;
  }
}

void STOCH::calculateStandard ()
{
  PlotLine *k = new PlotLine();
  int loop;
  
  for (loop = period; loop < (int) data->count(); loop++)
  {
    int loop2;
    double l = 0;
    double h = 0;
    for (loop2 = 0, l = 9999999, h = 0; loop2 < period; loop2++)
    {
      double t = data->getHigh(loop - loop2);
      if (t > h)
        h = t;

      t = data->getLow(loop - loop2);
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
    PlotLine *k2 = getMA(k, kMaType, kperiod);
    delete k;
    k = k2;
  }
  
  k->setColor(kcolor);
  k->setType(klineType);
  k->setLabel(klabel);
  output->addLine(k);

  if (dperiod > 1)
  {
    PlotLine *d = getMA(k, dMaType, dperiod);
    d->setColor(dcolor);
    d->setType(dlineType);
    d->setLabel(dlabel);
    output->addLine(d);
  }
  
  createZones();
}

void STOCH::calculateSingular (PlotLine *in)
{
  PlotLine *k = new PlotLine();
  int loop;
  
  for (loop = period; loop < (int) in->getSize(); loop++)
  {
    int loop2;
    double l;
    double h;
    for (loop2 = 0, l = 9999999, h = 0; loop2 < period; loop2++)
    {
      double t = in->getData(loop - loop2);
      if (t > h)
        h = t;
      if (t < l)
        l = t;
    }

    double close = in->getData(loop);
    double t = ((close - l) / (h - l)) * 100;
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    k->append(t);
  }

  if (kperiod > 1)
  {
    PlotLine *k2 = getMA(k, kMaType, kperiod);
    delete k;
    k = k2;
  }
  
  k->setColor(kcolor);
  k->setType(klineType);
  k->setLabel(klabel);
  output->addLine(k);

  if (dperiod > 1)
  {
    PlotLine *d = getMA(k, dMaType, dperiod);
    d->setColor(dcolor);
    d->setType(dlineType);
    d->setLabel(dlabel);
    output->addLine(d);
  }
  
  createZones();
}

void STOCH::calculateAdaptive (PlotLine *in)
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
    PlotLine *aStoch2 = getMA(aStoch, kMaType, kperiod);
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
    PlotLine *d = getMA(aStoch, dMaType, dperiod);
    d->setColor(dcolor);
    d->setType(dlineType);
    d->setLabel(dlabel);
    output->addLine(d);
  }
  
  createZones();
}

PlotLine * STOCH::getHighest( PlotLine *line,  int period)
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

PlotLine * STOCH::getLowest( PlotLine *line,  int period)
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

PlotLine * STOCH::getStdDev( PlotLine *line,  int period )
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

void STOCH::createZones ()
{
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

int STOCH::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString ll = QObject::tr("Label");
  QString perl = QObject::tr("Period");
  QString stl = QObject::tr("Smoothing Type");
  QString il = QObject::tr("Input");
  QString bzc = QObject::tr("Buy Zone Color");
  QString szc = QObject::tr("Sell Zone Color");
  QString bz = QObject::tr("Buy Zone");
  QString sz = QObject::tr("Sell Zone");
  QString ml = QObject::tr("Method");
  QString perMin = QObject::tr("Min Lookback Period");
  QString perMax = QObject::tr("Max Lookback Period");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("STOCH Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addComboItem(ml, pl, methodList, method);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  dialog->addComboItem(il, pl, inputTypeList, input);
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
  t = QObject::tr("%K Smoothing");
  dialog->addIntItem(t, pl, kperiod, 0, 99999999);
  QStringList l = getMATypes();
  t = QObject::tr("%K MA Type");
  dialog->addComboItem(t, pl, l, kMaType);
  
  pl = QObject::tr("%D Parms");
  dialog->createPage (pl);
  t = QObject::tr("%D Color");
  dialog->addColorItem(t, pl, dcolor);
  t = QObject::tr("%D Line Type");
  dialog->addComboItem(t, pl, lineTypes, dlineType);
  t = QObject::tr("%D Label");
  dialog->addTextItem(t, pl, dlabel);
  t = QObject::tr("%D Smoothing");
  dialog->addIntItem(t, pl, dperiod, 0, 99999999);
  t = QObject::tr("%D MA Type");
  dialog->addComboItem(t, pl, l, dMaType);
  
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
    t = QObject::tr("%D Smoothing");
    dperiod = dialog->getInt(t);
    t = QObject::tr("%D Label");
    dlabel = dialog->getText(t);
    t = QObject::tr("%K Color");
    kcolor = dialog->getColor(t);
    t = QObject::tr("%K Line Type");
    klineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("%K Smoothing");
    kperiod = dialog->getInt(t);
    t = QObject::tr("%K Label");
    klabel = dialog->getText(t);
    period = dialog->getInt(perl);
    input = (BarData::InputType) dialog->getComboIndex(il);
    method = dialog->getCombo(ml);

    t = QObject::tr("Min Lookback Period");
    minLookback = dialog->getInt(t);
    t = QObject::tr("Max Lookback Period");
    maxLookback = dialog->getInt(t);

    t = QObject::tr("%K MA Type");
    kMaType = dialog->getComboIndex(t);
    t = QObject::tr("%D MA Type");
    dMaType = dialog->getComboIndex(t);
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

void STOCH::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("label");
  if (s.length())
    label = s;

  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();

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

  s = dict.getData("method");
  if (s.length())
    method = s;
}

void STOCH::getIndicatorSettings (Setting &dict)
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
  dict.setData("label", label);
  dict.setData("input", QString::number(input));
  dict.setData("plugin", pluginName);
  dict.setData("kMaType", QString::number(kMaType));
  dict.setData("dMaType", QString::number(dMaType));
  dict.setData("minLookback", QString::number(minLookback));
  dict.setData("maxLookback", QString::number(maxLookback));
  dict.setData("method", method);
}

PlotLine * STOCH::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format standard: METHOD, K_MA_TYPE, D_MA_TYPE, PERIOD, K_PERIOD, D_PERIOD
  // format singular: METHOD, ARRAY_INPUT, K_MA_TYPE, D_MA_TYPE, PERIOD, K_PERIOD, D_PERIOD
  // format2 adaptive: METHOD, ARRAY_INPUT, K_MA_TYPE, D_MA_TYPE, PERIOD, K_PERIOD, D_PERIOD, MIN_LOOKBACK, MAX_LOOKBACK

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 6 || l.count() == 7 || l.count() == 9)
    ;
  else
  {
    qDebug("STOCH::calculateCustom: invalid parm count");
    return 0;
  }

  if (methodList.findIndex(l[0]) == -1)
  {
    qDebug("STOCH::calculateCustom: invalid METHOD parm");
    return 0;
  }
  else
    method = methodList.findIndex(l[0]);

  QStringList mal = getMATypes();

  if (! method.compare("Standard"))
  {
    if (mal.findIndex(l[1]) == -1)
    {
      qDebug("STOCH::calculateCustom: invalid K_MA_TYPE parm");
      return 0;
    }
    else
      kMaType = mal.findIndex(l[1]);

    if (mal.findIndex(l[2]) == -1)
    {
      qDebug("STOCH::calculateCustom: invalid D_MA_TYPE parm");
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
      qDebug("STOCH::calculateCustom: invalid PERIOD parm");
      return 0;
    }

    t = l[4].toInt(&ok);
    if (ok)
      kperiod = t;
    else
    {
      qDebug("STOCH::calculateCustom: invalid K_PERIOD parm");
      return 0;
    }

    t = l[5].toInt(&ok);
    if (ok)
      dperiod = t;
    else
    {
      qDebug("STOCH::calculateCustom: invalid D_PERIOD parm");
      return 0;
    }
  }

  if (! method.compare("Singular"))
  {
    if (! d.count())
    {
      qDebug("STOCH::calculateCustom: no input");
      return 0;
    }

    if (mal.findIndex(l[2]) == -1)
    {
      qDebug("STOCH::calculateCustom: invalid K_MA_TYPE parm");
      return 0;
    }
    else
      kMaType = mal.findIndex(l[2]);

    if (mal.findIndex(l[3]) == -1)
    {
      qDebug("STOCH::calculateCustom: invalid D_MA_TYPE parm");
      return 0;
    }
    else
      dMaType = mal.findIndex(l[3]);

    bool ok;
    int t = l[4].toInt(&ok);
    if (ok)
      period = t;
    else
    {
      qDebug("STOCH::calculateCustom: invalid PERIOD parm");
      return 0;
    }

    t = l[5].toInt(&ok);
    if (ok)
      kperiod = t;
    else
    {
      qDebug("STOCH::calculateCustom: invalid K_PERIOD parm");
      return 0;
    }

    t = l[6].toInt(&ok);
    if (ok)
      dperiod = t;
    else
    {
      qDebug("STOCH::calculateCustom: invalid D_PERIOD parm");
      return 0;
    }
  }

  if (! method.compare("Adaptive"))
  {
    if (! d.count())
    {
      qDebug("STOCH::calculateCustom: no input");
      return 0;
    }

    if (mal.findIndex(l[2]) == -1)
    {
      qDebug("STOCH::calculateCustom: invalid K_MA_TYPE parm");
      return 0;
    }
    else
      kMaType = mal.findIndex(l[2]);

    if (mal.findIndex(l[3]) == -1)
    {
      qDebug("STOCH::calculateCustom: invalid D_MA_TYPE parm");
      return 0;
    }
    else
      dMaType = mal.findIndex(l[3]);

    bool ok;
    int t = l[4].toInt(&ok);
    if (ok)
      period = t;
    else
    {
      qDebug("STOCH::calculateCustom: invalid PERIOD parm");
      return 0;
    }

    t = l[5].toInt(&ok);
    if (ok)
      kperiod = t;
    else
    {
      qDebug("STOCH::calculateCustom: invalid K_PERIOD parm");
      return 0;
    }

    t = l[6].toInt(&ok);
    if (ok)
      dperiod = t;
    else
    {
      qDebug("STOCH::calculateCustom: invalid D_PERIOD parm");
      return 0;
    }

    t = l[7].toInt(&ok);
    if (ok)
      minLookback = t;
    else
    {
      qDebug("STOCH::calculateCustom: invalid MIN_LOOKBACK parm");
      return 0;
    }

    t = l[8].toInt(&ok);
    if (ok)
      maxLookback = t;
    else
    {
      qDebug("STOCH::calculateCustom: invalid MAX_LOOKBACK parm");
      return 0;
    }
  }

  clearOutput();

  if (! method.compare("Standard"))
    calculateStandard();
  else
  {
    if (! method.compare("Singular"))
      calculateSingular(d.at(0));
    else
      calculateAdaptive(d.at(0));
  }

  return output->getLine(0);
}

int STOCH::getMinBars ()
{
  int t = minBars + period + dperiod + kperiod;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  STOCH *o = new STOCH;
  return ((IndicatorPlugin *) o);
}




