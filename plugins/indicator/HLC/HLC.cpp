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

#include "HLC.h"
#include "PrefDialog.h"
#include <qdict.h>

HLC::HLC ()
{
  pluginName = "HLC";
  plotFlag = TRUE;
  
  bandList.append(tr("Upper"));
  bandList.append(tr("Lower"));
  
  setDefaults();
}

HLC::~HLC ()
{
}

void HLC::setDefaults ()
{
  upperColor.setNamedColor("red");
  lowerColor.setNamedColor("red");
  upperLineType = PlotLine::Line;
  lowerLineType = PlotLine::Line;
  upperLabel = tr("HLCU");
  lowerLabel = tr("HLCL");
  period = 20;
  customBand = tr("Upper");
}

void HLC::calculate ()
{
  PlotLine *ub = new PlotLine();
  PlotLine *lb = new PlotLine();

  PlotLine *in = 0;
  if (customFlag)
  {
    in = getInputLine(customInput);
    if (! in)
    {
      qDebug("HLC::calculate: no input");
      return;
    }
    
    int loop;
    for (loop = period; loop < (int) in->getSize(); loop++)
    {
      int loop2;
      double h = -99999999;
      double l = 99999999;
      for (loop2 = 1; loop2 <= period; loop2++)
      {
        double t = in->getData(loop - loop2);

        if (t > h)
          h = t;

        if (t < l)
          l = t;
      }

      ub->append(h);
      lb->append(l);
    }
  }
  else
  {    
    int loop;
    for (loop = period; loop < (int) data->count(); loop++)
    {
      int loop2;
      double h = -99999999;
      double l = 99999999;
      for (loop2 = 1; loop2 <= period; loop2++)
      {
        double high = data->getHigh(loop - loop2);
        double low = data->getLow(loop - loop2);

        if (high > h)
          h = high;

        if (low < l)
          l = low;
      }

      ub->append(h);
      lb->append(l);
    }
  }

  ub->setColor(upperColor);
  ub->setType(upperLineType);
  ub->setLabel(upperLabel);
  output.append(ub);

  lb->setColor(lowerColor);
  lb->setType(lowerLineType);
  lb->setLabel(lowerLabel);
  output.append(lb);
}

int HLC::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("HLC Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  if (customFlag)
  {
    dialog->addComboItem(tr("Plot"), tr("Parms"), bandList, customBand);
    dialog->addFormulaInputItem(tr("Input"), tr("Parms"), FALSE, customInput);
  }
  
  dialog->createPage (tr("Upper"));
  dialog->addColorItem(tr("Upper Color"), tr("Upper"), upperColor);
  dialog->addComboItem(tr("Upper Line Type"), tr("Upper"), lineTypes, upperLineType);
  dialog->addTextItem(tr("Upper Label"), tr("Upper"), upperLabel);
  
  dialog->createPage (tr("Lower"));
  dialog->addColorItem(tr("Lower Color"), tr("Lower"), lowerColor);
  dialog->addComboItem(tr("Lower Line Type"), tr("Lower"), lineTypes, lowerLineType);
  dialog->addTextItem(tr("Lower Label"), tr("Lower"), lowerLabel);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    period = dialog->getInt(tr("Period"));
    upperColor = dialog->getColor(tr("Upper Color"));
    upperLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Upper Line Type"));
    upperLabel = dialog->getText(tr("Upper Label"));
    lowerColor = dialog->getColor(tr("Lower Color"));
    lowerLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Lower Line Type"));
    lowerLabel = dialog->getText(tr("Lower Label"));
    if (customFlag)
    {
      customBand = dialog->getCombo(tr("Plot"));
      customInput = dialog->getFormulaInput(tr("Input"));
    }
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void HLC::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void HLC::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void HLC::setIndicatorSettings (Setting dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("period");
  if (s.length())
    period = s.toInt();
  
  s = dict.getData("upperColor");
  if (s.length())
    upperColor.setNamedColor(s);
    
  s = dict.getData("upperLineType");
  if (s.length())
    upperLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("upperLabel");
  if (s.length())
    upperLabel = s;
      
  s = dict.getData("lowerColor");
  if (s.length())
    lowerColor.setNamedColor(s);
    
  s = dict.getData("lowerLineType");
  if (s.length())
    lowerLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("lowerLabel");
  if (s.length())
    lowerLabel = s;

  s = dict.getData("customBand");
  if (s.length())
    customBand = s;

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
}

Setting HLC::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("period", QString::number(period));
  dict.setData("upperColor", upperColor.name());
  dict.setData("upperLineType", QString::number(upperLineType));
  dict.setData("upperLabel", upperLabel);
  dict.setData("lowerColor", lowerColor.name());
  dict.setData("lowerLineType", QString::number(lowerLineType));
  dict.setData("lowerLabel", lowerLabel);
  dict.setData("customBand", customBand);
  dict.setData("customInput", customInput);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * HLC::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  if (! customBand.compare(tr("Upper")))
    return output.at(0);
  else
    return output.at(1);
}

Plugin * create ()
{
  HLC *o = new HLC;
  return ((Plugin *) o);
}


