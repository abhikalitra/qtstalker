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
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("VT Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  dialog->addComboItem(QObject::tr("Method"), QObject::tr("Parms"), methodList, method);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    label = dialog->getText(QObject::tr("Label"));
    method = dialog->getCombo(QObject::tr("Method"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void VT::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void VT::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void VT::setIndicatorSettings (Setting dict)
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

Setting VT::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("label", label);
  dict.setData("method", method);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * VT::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output->getLine(0);
}

IndicatorPlugin * createIndicatorPlugin ()
{
  VT *o = new VT;
  return ((IndicatorPlugin *) o);
}


