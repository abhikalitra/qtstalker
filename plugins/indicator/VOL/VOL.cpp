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

#include "VOL.h"
#include "PrefDialog.h"
#include <qdict.h>

VOL::VOL ()
{
  pluginName = "VOL";
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
  volLineType = PlotLine::HistogramBar;
  maLineType = PlotLine::Line;
  volLabel = "VOL";
  maLabel = "MAVol";
  period = 0;
  maType = IndicatorPlugin::SMA;
}

void VOL::calculate ()
{
  PlotLine *pl = data->getInput(BarData::Volume);
  pl->setType(volLineType);
  pl->setLabel(volLabel);
  pl->setColorFlag(TRUE);

  // set the first bar color
  pl->appendColorBar(upColor);

  int loop;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    if (data->getClose(loop) > data->getClose(loop - 1))
      pl->appendColorBar(upColor);
    else
      pl->appendColorBar(downColor);
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

int VOL::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("VOL Indicator"));
  
  dialog->createPage (tr("VOL"));
  dialog->addColorItem(tr("Up Color"), tr("VOL"), upColor);
  dialog->addColorItem(tr("Down Color"), tr("VOL"), downColor);
  dialog->addTextItem(tr("VOL Label"), tr("VOL"), volLabel);
  dialog->addComboItem(tr("VOL Line Type"), tr("VOL"), lineTypes, volLineType);
  
  dialog->createPage (tr("MA"));
  dialog->addColorItem(tr("MA Color"), tr("MA"), maColor);
  dialog->addIntItem(tr("MA Period"), tr("MA"), period, 0, 99999999);
  dialog->addTextItem(tr("MA Label"), tr("MA"), maLabel);
  dialog->addComboItem(tr("MA Line Type"), tr("MA"), lineTypes, maLineType);
  dialog->addComboItem(tr("MA Type"), tr("MA"), maTypeList, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    upColor = dialog->getColor(tr("Up Color"));
    downColor = dialog->getColor(tr("Down Color"));
    volLabel = dialog->getText(tr("VOL Label"));
    volLineType = (PlotLine::LineType) dialog->getComboIndex(tr("VOL Line Type"));
    
    maColor = dialog->getColor(tr("MA Color"));
    period = dialog->getInt(tr("MA Period"));
    maLabel = dialog->getText(tr("MA Label"));
    maLineType = (PlotLine::LineType) dialog->getComboIndex(tr("MA Line Type"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("MA Type"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void VOL::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void VOL::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void VOL::setIndicatorSettings (Setting dict)
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
    
  s = dict.getData("volLabel");
  if (s.length())
    volLabel = s;
        
  s = dict.getData("volLineType");
  if (s.length())
    volLineType = (PlotLine::LineType) s.toInt();
        
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
    maType = (IndicatorPlugin::MAType) s.toInt();
}

Setting VOL::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("upColor", upColor.name());
  dict.setData("downColor", downColor.name());
  dict.setData("volLabel", volLabel);
  dict.setData("volLineType", QString::number(volLineType));
  dict.setData("maColor", maColor.name());
  dict.setData("maPeriod", QString::number(period));
  dict.setData("maLabel", maLabel);
  dict.setData("maLineType", QString::number(maLineType));
  dict.setData("maType", QString::number(maType));
  dict.setData("plugin", pluginName);
  return dict;
}

Plugin * create ()
{
  VOL *o = new VOL;
  return ((Plugin *) o);
}

