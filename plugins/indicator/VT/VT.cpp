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

#include "VT.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

VT::VT ()
{
  pluginName = "VT";
  
  methodList.append("NVI");
  methodList.append("OBV");
  methodList.append("PVI");
  methodList.append("PVT");

  helpFile = "vt.html";
    
  setDefaults();
}

VT::~VT ()
{
}

void VT::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  method = "OBV";
}

void VT::calculate ()
{
  if (! method.compare("OBV"))
    calculateOBV();
  else
  {
    if (! method.compare("NVI"))
      calculateNVI();
    else
    {
      if (! method.compare("PVI"))
        calculatePVI();
      else
        calculatePVT();
    }
  }
}

void VT::calculateOBV ()
{
  PlotLine *obv = new PlotLine();
  obv->setColor(color);
  obv->setType(lineType);
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
}

void VT::calculateNVI ()
{
  PlotLine *nvi = new PlotLine();
  nvi->setColor(color);
  nvi->setType(lineType);
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
}

void VT::calculatePVI ()
{
  PlotLine *pvi = new PlotLine();
  pvi->setColor(color);
  pvi->setType(lineType);
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
}

void VT::calculatePVT ()
{
  PlotLine *pvt = new PlotLine();
  pvt->setColor(color);
  pvt->setType(lineType);
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
}

int VT::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString ml = QObject::tr("Method");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("VT Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ml, pl, methodList, method);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    method = dialog->getCombo(ml);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void VT::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("method");
  if (s.length())
    method = s;
}

void VT::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("label", label);
  dict.setData("method", method);
  dict.setData("plugin", pluginName);
}

PlotLine * VT::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output->getLine(0);
}

int VT::getMinBars ()
{
  int t = minBars + 2;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  VT *o = new VT;
  return ((IndicatorPlugin *) o);
}


