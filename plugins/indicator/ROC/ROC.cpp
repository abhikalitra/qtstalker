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

#include "ROC.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

ROC::ROC ()
{
  pluginName = "ROC";
  setDefaults();
}

ROC::~ROC ()
{
}

void ROC::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Histogram;
  label = pluginName;
  period = 14;
  smoothing = 10;
  maType = IndicatorPlugin::SMA;  
  input = BarData::Close;
}

void ROC::calculate ()
{
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("ROC::calculate: no input");
    return;
  }

  PlotLine *roc = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
    roc->append(((in->getData(loop) - in->getData(loop - period)) / in->getData(loop - period)) * 100);

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(roc, maType, smoothing);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output->addLine(ma);
    delete roc;
  }
  else
  {
    roc->setColor(color);
    roc->setType(lineType);
    roc->setLabel(label);
    output->addLine(roc);
  }

  if (! customFlag)
    delete in;
}

int ROC::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("ROC Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  dialog->addIntItem(QObject::tr("Period"), QObject::tr("Parms"), period, 1, 99999999);
  dialog->addComboItem(QObject::tr("Smoothing Type"), QObject::tr("Parms"), maTypeList, maType);
  if (customFlag)
    dialog->addFormulaInputItem(QObject::tr("Input"), QObject::tr("Parms"), FALSE, customInput);
  else
    dialog->addComboItem(QObject::tr("Input"), QObject::tr("Parms"), inputTypeList, input);
  dialog->addIntItem(QObject::tr("Smoothing"), QObject::tr("Parms"), smoothing, 0, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    period = dialog->getInt(QObject::tr("Period"));
    label = dialog->getText(QObject::tr("Label"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(QObject::tr("Smoothing Type"));
    smoothing = dialog->getInt(QObject::tr("Smoothing"));
    if (customFlag)
      customInput = dialog->getFormulaInput(QObject::tr("Input"));
    else
      input = (BarData::InputType) dialog->getComboIndex(QObject::tr("Input"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void ROC::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void ROC::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void ROC::setIndicatorSettings (Setting dict)
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
    
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();
    
  s = dict.getData("smoothing");
  if (s.length())
    smoothing = s.toInt();

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
}

Setting ROC::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("maType", QString::number(maType));
  dict.setData("input", QString::number(input));
  dict.setData("smoothing", QString::number(smoothing));
  dict.setData("customInput", customInput);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * ROC::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(0);
}

IndicatorPlugin * createIndicatorPlugin ()
{
  ROC *o = new ROC;
  return ((IndicatorPlugin *) o);
}




