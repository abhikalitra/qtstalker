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

#include "VOL.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

VOL::VOL ()
{
  pluginName = "VOL";
  helpFile = "vol.html";

  methodList.append("VOL");
  methodList.append("NVI");
  methodList.append("OBV");
  methodList.append("PVI");
  methodList.append("PVT");
  methodList.sort();
  
  setDefaults();
}

VOL::~VOL ()
{
}

void VOL::setDefaults ()
{
  upColor.setNamedColor("green");
  downColor.setNamedColor("red");
  maColor.setNamedColor("yellow");
  vtColor.setNamedColor("red");
  volLineType = PlotLine::HistogramBar;
  maLineType = PlotLine::Line;
  vtLineType = PlotLine::Line;
  volLabel = "VOL";
  label = volLabel;
  maLabel = "MAVol";
  period = 0;
  maType = 1;
  method = "VOL";
}

void VOL::calculate ()
{
  while (1)
  {
    if (! method.compare("VOL"))
    {
      calculateVOL();
      break;
    }

    if (! method.compare("OBV"))
    {
      calculateOBV();
      break;
    }

    if (! method.compare("NVI"))
    {
      calculateNVI();
      break;
    }

    if (! method.compare("PVI"))
    {
      calculatePVI();
      break;
    }

    if (! method.compare("PVI"))
    {
      calculatePVT();
      break;
    }

    break;
  }
}

void VOL::calculateVOL ()
{
  PlotLine *pl = data->getInput(BarData::Volume);
  pl->setType(volLineType);
  pl->setLabel(volLabel);
  pl->setColorFlag(TRUE);

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    if (loop > 0)
    {
      if (data->getClose(loop) > data->getClose(loop - 1))
        pl->setColorBar(loop, upColor);
      else
        pl->setColorBar(loop, downColor);
    }
    else
      pl->setColorBar(loop, upColor);
  }

  output->addLine(pl);

  if (period < 1)
    return;

  PlotLine *ma = getMA(pl, maType, period);
  ma->setColor(maColor);
  ma->setType(maLineType);
  ma->setLabel(maLabel);
  output->addLine(ma);
}

void VOL::calculateOBV ()
{
  PlotLine *obv = new PlotLine();
  obv->setColor(vtColor);
  obv->setType(vtLineType);
  obv->setLabel(label);

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

  output->addLine(obv);

  if (period < 1)
    return;

  PlotLine *ma = getMA(obv, maType, period);
  ma->setColor(maColor);
  ma->setType(maLineType);
  ma->setLabel(maLabel);
  output->addLine(ma);
}

void VOL::calculateNVI ()
{
  PlotLine *nvi = new PlotLine();
  nvi->setColor(vtColor);
  nvi->setType(vtLineType);
  nvi->setLabel(label);

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
  
  output->addLine(nvi);

  if (period < 1)
    return;

  PlotLine *ma = getMA(nvi, maType, period);
  ma->setColor(maColor);
  ma->setType(maLineType);
  ma->setLabel(maLabel);
  output->addLine(ma);
}

void VOL::calculatePVI ()
{
  PlotLine *pvi = new PlotLine();
  pvi->setColor(vtColor);
  pvi->setType(vtLineType);
  pvi->setLabel(label);

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

  output->addLine(pvi);

  if (period < 1)
    return;

  PlotLine *ma = getMA(pvi, maType, period);
  ma->setColor(maColor);
  ma->setType(maLineType);
  ma->setLabel(maLabel);
  output->addLine(ma);
}

void VOL::calculatePVT ()
{
  PlotLine *pvt = new PlotLine();
  pvt->setColor(vtColor);
  pvt->setType(vtLineType);
  pvt->setLabel(label);

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

  output->addLine(pvt);

  if (period < 1)
    return;

  PlotLine *ma = getMA(pvt, maType, period);
  ma->setColor(maColor);
  ma->setType(maLineType);
  ma->setLabel(maLabel);
  output->addLine(ma);
}

int VOL::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("VOL Indicator"));
  dialog->setHelpFile(helpFile);
  QString pl = QObject::tr("VOL");
  dialog->createPage (pl);

  QString t = QObject::tr("Method");
  dialog->addComboItem(t, pl, methodList, method);
  t = QObject::tr("Up Color");
  dialog->addColorItem(t, pl, upColor);
  t = QObject::tr("Down Color");
  dialog->addColorItem(t, pl, downColor);
  t = QObject::tr("Color");
  dialog->addColorItem(t, pl, vtColor);
  t = QObject::tr("VOL Label");
  dialog->addTextItem(t, pl, volLabel);
  t = QObject::tr("VOL Line Type");
  dialog->addComboItem(t, pl, lineTypes, volLineType);
  t = QObject::tr("Line Type");
  dialog->addComboItem(t, pl, lineTypes, vtLineType);
  
  pl = QObject::tr("MA");
  dialog->createPage (pl);
  t = QObject::tr("MA Color");
  dialog->addColorItem(t, pl, maColor);
  t = QObject::tr("MA Period");
  dialog->addIntItem(t, pl, period, 0, 99999999);
  t = QObject::tr("MA Label");
  dialog->addTextItem(t, pl, maLabel);
  t = QObject::tr("MA Line Type");
  dialog->addComboItem(t, pl, lineTypes, maLineType);
  QStringList l = getMATypes();
  t = QObject::tr("MA Type");
  dialog->addComboItem(t, pl, l, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    t = QObject::tr("Method");
    method = dialog->getCombo(t);
    t = QObject::tr("Up Color");
    upColor = dialog->getColor(t);
    t = QObject::tr("Down Color");
    downColor = dialog->getColor(t);
    t = QObject::tr("Color");
    vtColor = dialog->getColor(t);
    t = QObject::tr("VOL Label");
    volLabel = dialog->getText(t);
    label = volLabel;
    t = QObject::tr("VOL Line Type");
    volLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("Line Type");
    vtLineType = (PlotLine::LineType) dialog->getComboIndex(t);

    t = QObject::tr("MA Color");
    maColor = dialog->getColor(t);
    t = QObject::tr("MA Period");
    period = dialog->getInt(t);
    t = QObject::tr("MA Label");
    maLabel = dialog->getText(t);
    t = QObject::tr("MA Line Type");
    maLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("MA Type");
    maType = dialog->getComboIndex(t);
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void VOL::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("upColor");
  if (s.length())
    upColor.setNamedColor(s);
    
  s = dict.getData("downColor");
  if (s.length())
    downColor.setNamedColor(s);
    
  s = dict.getData("vtColor");
  if (s.length())
    vtColor.setNamedColor(s);

  s = dict.getData("volLabel");
  if (s.length())
    volLabel = s;
        
  s = dict.getData("volLineType");
  if (s.length())
    volLineType = (PlotLine::LineType) s.toInt();
        
  s = dict.getData("vtLineType");
  if (s.length())
    vtLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("maColor");
  if (s.length())
    maColor.setNamedColor(s);
        
  s = dict.getData("maPeriod");
  if (s.length())
    period = s.toInt();
	
  s = dict.getData("maLabel");
  if (s.length())
    maLabel = s;
        
  s = dict.getData("maLineType");
  if (s.length())
    maLineType = (PlotLine::LineType) s.toInt();
        
  s = dict.getData("maType");
  if (s.length())
    maType = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;

  s = dict.getData("method");
  if (s.length())
    method = s;
}

void VOL::getIndicatorSettings (Setting &dict)
{
  dict.setData("upColor", upColor.name());
  dict.setData("downColor", downColor.name());
  dict.setData("vtColor", vtColor.name());
  dict.setData("volLabel", volLabel);
  dict.setData("volLineType", QString::number(volLineType));
  dict.setData("vtLineType", QString::number(vtLineType));
  dict.setData("maColor", maColor.name());
  dict.setData("maPeriod", QString::number(period));
  dict.setData("maLabel", maLabel);
  dict.setData("maLineType", QString::number(maLineType));
  dict.setData("maType", QString::number(maType));
  dict.setData("label", label);
  dict.setData("method", method);
  dict.setData("plugin", pluginName);
}

PlotLine * VOL::calculateCustom (QString &p, QPtrList<PlotLine> &)
{
  // format1: METHOD

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 1)
    ;
  else
  {
    qDebug("VOL::calculateCustom: invalid parm count");
    return 0;
  }

  if (methodList.findIndex(l[0]) == -1)
  {
    qDebug("VOL::calculateCustom: invalid METHOD parm");
    return 0;
  }
  else
    method = methodList.findIndex(l[0]);

  clearOutput();
  calculate();
  return output->getLine(0);
}

int VOL::getMinBars ()
{
  int t = minBars + period;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  VOL *o = new VOL;
  return ((IndicatorPlugin *) o);
}

