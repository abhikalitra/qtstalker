/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
 *  Adaptation to AROON algorithm humbly copyright (C) 2005 Jean "Flynn" Flinois
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

#include "AROON.h"
#include <math.h>
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

AROON::AROON ()
{
  pluginName = "AROON";
  
  lineList.append("Up");
  lineList.append("Down");
  lineList.append("Osc");
  
  helpFile = "aroon.html";
  
  setDefaults();
}

AROON::~AROON ()
{
}

void AROON::setDefaults ()
{
  downColor.setNamedColor("red");
  upColor.setNamedColor("green");
  oscColor.setNamedColor("yellow");
  buyColor.setNamedColor("gray");
  sellColor.setNamedColor("gray");
  downLineType = PlotLine::Line;
  upLineType = PlotLine::Line;
  oscLineType = PlotLine::Line;
  downLabel = "DOWN";
  upLabel = "UP";
  oscLabel = "OSC";
  period = 14;
  buyLine = 25;
  sellLine = 75;
  label = pluginName;
}

void AROON::calculate ()
{
  getAROON(period);
  getOSC();
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

int AROON::indicatorPrefDialog (QWidget *w)
{
  QString perl = QObject::tr("Period");
  QString bzc = QObject::tr("Buy Zone Color");
  QString szc = QObject::tr("Sell Zone Color");
  QString bz = QObject::tr("Buy Zone");
  QString sz = QObject::tr("Sell Zone");
  
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("AROON Indicator"));
  dialog->setHelpFile(helpFile);
  
  QString pl = "AROON";
  dialog->createPage (pl);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  QStringList l = getMATypes();
  if (customFlag)
  {
    QString t = QObject::tr("Label");
    dialog->addTextItem(t, pl, label);
    t = QObject::tr("Plot");
    dialog->addComboItem(t, pl, lineList, lineRequest);
  }
  
  pl = "DOWN";
  dialog->createPage (pl);
  QString t = QObject::tr("DOWN Color");
  dialog->addColorItem(t, pl, downColor);
  t =  QObject::tr("DOWN Label");
  dialog->addTextItem(t, pl, downLabel);
  t = QObject::tr("DOWN Line Type");
  dialog->addComboItem(t, pl, lineTypes, downLineType);
  
  pl = "UP";
  dialog->createPage (pl);
  t = QObject::tr("UP Color");
  dialog->addColorItem(t, pl, upColor);
  t = QObject::tr("UP Label");
  dialog->addTextItem(t, pl, upLabel);
  t = QObject::tr("UP Line Type");
  dialog->addComboItem(t, pl, lineTypes, upLineType);
  
  pl = "OSC";
  dialog->createPage (pl);
  t = QObject::tr("OSC Color");
  dialog->addColorItem(t, pl, oscColor);
  t = QObject::tr("OSC Label");
  dialog->addTextItem(t, pl, oscLabel);
  t = QObject::tr("OSC Line Type");
  dialog->addComboItem(t, pl, lineTypes, oscLineType);
  
  pl = QObject::tr("Zones");
  dialog->createPage (pl);
  dialog->addColorItem(bzc, pl, buyColor);
  dialog->addColorItem(szc, pl, sellColor);
  dialog->addIntItem(bz, pl, buyLine, 0, 100);
  dialog->addIntItem(sz, pl, sellLine, 0, 100);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    period = dialog->getInt(perl);
    if (customFlag)
    {
      t = QObject::tr("Label");
      label = dialog->getText(t);
      t = QObject::tr("Plot");
      lineRequest = dialog->getCombo(t);    
    }
    
    buyColor = dialog->getColor(bzc);
    sellColor = dialog->getColor(szc);
    buyLine = dialog->getInt(bz);
    sellLine = dialog->getInt(sz);

    t = QObject::tr("DOWN Color");
    downColor = dialog->getColor(t);
    t = QObject::tr("DOWN Line Type");
    downLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("DOWN Label");
    downLabel = dialog->getText(t);
    
    t = QObject::tr("UP Color");
    upColor = dialog->getColor(t);
    t = QObject::tr("UP Line Type");
    upLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("UP Label");
    upLabel = dialog->getText(t);
    
    t = QObject::tr("OSC Color");
    oscColor = dialog->getColor(t);
    t = QObject::tr("OSC Line Type");
    oscLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("OSC Label");
    oscLabel = dialog->getText(t);
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void AROON::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("downColor");
  if (s.length())
    downColor.setNamedColor(s);
    
  s = dict.getData("upColor");
  if (s.length())
    upColor.setNamedColor(s);
  
  s = dict.getData("oscColor");
  if (s.length())
    oscColor.setNamedColor(s);
    
  s = dict.getData("buyColor");
  if (s.length())
    buyColor.setNamedColor(s);
  
  s = dict.getData("sellColor");
  if (s.length())
    sellColor.setNamedColor(s);
  
  s = dict.getData("period");
  if (s.length())
    period = (PlotLine::LineType) s.toInt();
    
  s = dict.getData("downLabel");
  if (s.length())
    downLabel = s;
    
  s = dict.getData("upLabel");
  if (s.length())
    upLabel = s;
    
  s = dict.getData("oscLabel");
  if (s.length())
    oscLabel = s;
        
  s = dict.getData("downLineType");
  if (s.length())
    downLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("upLineType");
  if (s.length())
    upLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("oscLineType");
  if (s.length())
    oscLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("buyLine");
  if (s.length())
    buyLine = s.toInt();

  s = dict.getData("sellLine");
  if (s.length())
    sellLine = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;

  s = dict.getData("lineRequest");
  if (s.length())
    lineRequest = s;
}

void AROON::getIndicatorSettings (Setting &dict)
{
  dict.setData("period", QString::number(period));
  dict.setData("downColor", downColor.name());
  dict.setData("upColor", upColor.name());
  dict.setData("oscColor", oscColor.name());
  dict.setData("buyColor", buyColor.name());
  dict.setData("sellColor", sellColor.name());
  dict.setData("downLineType", QString::number(downLineType));
  dict.setData("upLineType", QString::number(upLineType));
  dict.setData("oscLineType", QString::number(oscLineType));
  dict.setData("downLabel", downLabel);
  dict.setData("upLabel", upLabel);
  dict.setData("oscLabel", oscLabel);
  dict.setData("buyLine", QString::number(buyLine));
  dict.setData("sellLine", QString::number(sellLine));
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
  dict.setData("lineRequest", lineRequest);
}

PlotLine * AROON::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  if (! lineRequest.compare("DOWN"))
    return output->getLine(0);
  else
  {
    if (! lineRequest.compare("UP"))
      return output->getLine(1);
    else
      return output->getLine(2);
  }
}

void AROON::getAROON (int period)
{
  PlotLine *down = new PlotLine();
  PlotLine *up = new PlotLine();
  
  int loop;
  int last = (int)data->count() ; 
  int maxNdx = -1 ;
  int minNdx = -1 ;
  for (loop = period; loop < last; loop++)
  {
    int back = loop-period ;
    if ((maxNdx<=back)||(minNdx<=back))
    {
      double tmpMin=99999999 ;
      double tmpMax=0 ;
      for(int i=back+1; i<=loop; ++i)
      {
	if (data->getHigh(i)>tmpMax) { maxNdx=i; tmpMax=data->getHigh(i) ; }
	if (data->getLow(i)<tmpMin) { minNdx=i; tmpMin=data->getLow(i) ; }
      }
    } else {
      if ((data->getHigh(loop)>=data->getHigh(maxNdx))) maxNdx = loop ;
      if ((data->getLow(loop)<=data->getLow(minNdx))) minNdx = loop ;
    }
    double m = 100 * (period - (loop - minNdx)) / period ;
    double p = 100 * (period - (loop - maxNdx)) / period ;
   
    down->append(m);
    up->append(p);
  }


  down->setColor(downColor);
  down->setType(downLineType);
  down->setLabel(downLabel);
  output->addLine(down);
  
  up->setColor(upColor);
  up->setType(upLineType);
  up->setLabel(upLabel);
  output->addLine(up);

}

void AROON::getOSC ()
{
  PlotLine *down = output->getLine(0);
  if (! down)
    return;
    
  PlotLine *up = output->getLine(1);
  if (! up)
    return;
  
  int downLoop = down->getSize() - 1;
  int upLoop = up->getSize() - 1;

  PlotLine *osc = new PlotLine;

  while (downLoop > -1 && upLoop > -1)
  {
    osc->prepend(up->getData(upLoop) - down->getData(downLoop));
    downLoop--;
    upLoop--;
  }

  osc->setColor(oscColor);
  osc->setType(oscLineType);
  osc->setLabel(oscLabel);
  output->addLine(osc);
  
}

int AROON::getMinBars ()
{
  int t = minBars + 1 + period ;
  return t;
}

//*********************************************************
//*********************************************************
//*********************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  AROON *o = new AROON;
  return ((IndicatorPlugin *) o);
}

