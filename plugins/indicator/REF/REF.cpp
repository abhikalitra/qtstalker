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

#include "REF.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

REF::REF ()
{
  pluginName = "REF";
  customFlag = TRUE;
  helpFile = "ref.html";
  
  setDefaults();
}

REF::~REF ()
{
}

void REF::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  input = BarData::Close;
  period = 0;
}

void REF::calculate ()
{
  PlotLine *line = new PlotLine;
  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  
  PlotLine *in = data->getInput(input);
  
  int loop = 0;
  for (loop = 0; loop < in->getSize(); loop++)
  {
    if (loop - period < 0)
      continue;
      
    line->append(in->getData(loop - period));
  }
  
  output->addLine(line);
  
  delete in;
}

int REF::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("REF Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addIntItem(QObject::tr("Period"), QObject::tr("Parms"), period, 0, 999999);
  dialog->addComboItem(QObject::tr("Input"), QObject::tr("Parms"), inputTypeList, input);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    label = dialog->getText(QObject::tr("Label"));
    period = dialog->getInt(QObject::tr("Period"));
    input = (BarData::InputType) dialog->getComboIndex(QObject::tr("Input"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void REF::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void REF::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void REF::setIndicatorSettings (Setting dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("label");
  if (s.length())
    label = s;
        
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();
    
  s = dict.getData("period");
  if (s.length())
    period = s.toInt();
    
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();
}

Setting REF::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("label", label);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("plugin", pluginName);
  dict.setData("period", QString::number(period));
  dict.setData("input", QString::number(input));
  return dict;
}

PlotLine * REF::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output->getLine(0);
}

IndicatorPlugin * createIndicatorPlugin ()
{
  REF *o = new REF;
  return ((IndicatorPlugin *) o);
}



