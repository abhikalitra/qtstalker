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
  label = volLabel;
  maLabel = "MAVol";
  period = 0;
  maType = 1;
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
  dialog->setCaption(QObject::tr("VOL Indicator"));
  dialog->setHelpFile(helpFile);
  
  QString pl = QObject::tr("VOL");
  dialog->createPage (pl);
  QString t = QObject::tr("Up Color");
  dialog->addColorItem(t, pl, upColor);
  t = QObject::tr("Down Color");
  dialog->addColorItem(t, pl, downColor);
  t = QObject::tr("VOL Label");
  dialog->addTextItem(t, pl, volLabel);
  t = QObject::tr("VOL Line Type");
  dialog->addComboItem(t, pl, lineTypes, volLineType);
  
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
    t = QObject::tr("Up Color");
    upColor = dialog->getColor(t);
    t = QObject::tr("Down Color");
    downColor = dialog->getColor(t);
    t = QObject::tr("VOL Label");
    volLabel = dialog->getText(t);
    label = volLabel;
    t = QObject::tr("VOL Line Type");
    volLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    
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
    maType = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;
}

void VOL::getIndicatorSettings (Setting &dict)
{
  dict.setData("upColor", upColor.name());
  dict.setData("downColor", downColor.name());
  dict.setData("volLabel", volLabel);
  dict.setData("volLineType", QString::number(volLineType));
  dict.setData("maColor", maColor.name());
  dict.setData("maPeriod", QString::number(period));
  dict.setData("maLabel", maLabel);
  dict.setData("maLineType", QString::number(maLineType));
  dict.setData("maType", QString::number(maType));
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
}

PlotLine * VOL::calculateCustom (QDict<PlotLine> *)
{
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

