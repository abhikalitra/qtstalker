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

#include "MAOSC.h"
#include "PrefDialog.h"
#include <qdict.h>

MAOSC::MAOSC ()
{
  pluginName = "MAOSC";
  plotFlag = FALSE;
  setDefaults();
}

MAOSC::~MAOSC ()
{
}

void MAOSC::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Histogram;
  label = pluginName;
  fastPeriod = 9;
  slowPeriod = 18;
  fastMaType = IndicatorPlugin::SMA;  
  slowMaType = IndicatorPlugin::SMA;  
  input = BarData::Close;
}

void MAOSC::calculate ()
{
  PlotLine *in = data->getInput(input);
  
  PlotLine *fma = getMA(in, fastMaType, fastPeriod);
  int fmaLoop = fma->getSize() - 1;

  PlotLine *sma = getMA(in, slowMaType, slowPeriod);
  int smaLoop = sma->getSize() - 1;

  PlotLine *osc = new PlotLine();
  osc->setColor(color);
  osc->setType(lineType);
  osc->setLabel(label);
  
  while (fmaLoop > -1 && smaLoop > -1)
  {
    osc->prepend(fma->getData(fmaLoop) - sma->getData(smaLoop));
    fmaLoop--;
    smaLoop--;
  }

  output.append(osc);
  
  delete fma;
  delete sma;
  delete in;
}

int MAOSC::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("MAOSC Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addIntItem(tr("Fast Period"), tr("Parms"), fastPeriod, 1, 99999999);
  dialog->addIntItem(tr("Slow Period"), tr("Parms"), slowPeriod, 1, 99999999);
  dialog->addComboItem(tr("Fast MA Type"), tr("Parms"), maTypeList, fastMaType);
  dialog->addComboItem(tr("Slow MA Type"), tr("Parms"), maTypeList, slowMaType);
  dialog->addComboItem(tr("Input"), tr("Parms"), inputTypeList, input);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    fastPeriod = dialog->getInt(tr("Fast Period"));
    slowPeriod = dialog->getInt(tr("Slow Period"));
    label = dialog->getText(tr("Label"));
    fastMaType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("Fast MA Type"));
    slowMaType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("Slow MA Type"));
    input = (BarData::InputType) dialog->getComboIndex(tr("Input"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MAOSC::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["color"];
  if (s)
    color.setNamedColor(s->left(s->length()));
    
  s = dict["lineType"];
  if (s)
    lineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["fastPeriod"];
  if (s)
    fastPeriod = s->left(s->length()).toInt();

  s = dict["slowPeriod"];
  if (s)
    slowPeriod = s->left(s->length()).toInt();
  
  s = dict["label"];
  if (s)
    label = s->left(s->length());
      
  s = dict["fastMaType"];
  if (s)
    fastMaType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
    
  s = dict["slowMaType"];
  if (s)
    slowMaType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
  
  s = dict["input"];
  if (s)
    input = (BarData::InputType) s->left(s->length()).toInt();
}

void MAOSC::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("fastPeriod", new QString(QString::number(fastPeriod)));
  dict.replace("slowPeriod", new QString(QString::number(slowPeriod)));
  dict.replace("label", new QString(label));
  dict.replace("fastMaType", new QString(QString::number(fastMaType)));
  dict.replace("slowMaType", new QString(QString::number(slowMaType)));
  dict.replace("input", new QString(QString::number(input)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  MAOSC *o = new MAOSC;
  return ((Plugin *) o);
}

