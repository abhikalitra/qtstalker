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

#include "DPO.h"
#include "PrefDialog.h"
#include <qdict.h>

DPO::DPO ()
{
  pluginName = "DPO";
  plotFlag = FALSE;
  setDefaults();
}

DPO::~DPO ()
{
}

void DPO::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 21;
  maType = IndicatorPlugin::SMA;
}

void DPO::calculate ()
{
  PlotLine *c = data->getInput(BarData::Close);

  PlotLine *ma = getMA(c, maType, period);

  PlotLine *dpo = new PlotLine();
  dpo->setColor(color);
  dpo->setType(lineType);
  dpo->setLabel(label);
  output.append(dpo);

  int maLoop = ma->getSize() - 1;
  int closeLoop = c->getSize() - 1;
  int t = (int) ((period / 2) + 1);

  while (maLoop >= t)
  {
    dpo->prepend(c->getData(closeLoop) - ma->getData(maLoop - t));
    closeLoop--;
    maLoop--;
  }

  delete c;
  delete ma;
}

int DPO::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("DPO Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  dialog->addComboItem(tr("MA Type"), tr("Parms"), maTypeList, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    period = dialog->getInt(tr("Period"));
    label = dialog->getText(tr("Label"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("MA Type"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void DPO::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void DPO::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void DPO::setIndicatorSettings (Setting dict)
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

  s = dict.getData("period");
  if (s.length())
    period = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("maType");
  if (s.length())
    maType = (IndicatorPlugin::MAType) s.toInt();
}

Setting DPO::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("maType", QString::number(maType));
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * DPO::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output.at(0);
}

Plugin * create ()
{
  DPO *o = new DPO;
  return ((Plugin *) o);
}

