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

#include "TRIX.h"
#include "PrefDialog.h"
#include <qdict.h>

TRIX::TRIX ()
{
  pluginName = "TRIX";
  plotFlag = FALSE;
  setDefaults();
}

TRIX::~TRIX ()
{
}

void TRIX::setDefaults ()
{
  color.setNamedColor("red");
  trigColor.setNamedColor("yellow");
  lineType = PlotLine::Line;
  trigLineType = PlotLine::Dash;
  label = pluginName;
  trigLabel = tr("TRIX Trig");
  period = 12;
  tperiod = 9;
  input = BarData::Close;
  maType = IndicatorPlugin::SMA;  
}

void TRIX::calculate ()
{
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("MOM::calculate: no input");
    return;
  }

  PlotLine *ema = getEMA(in, period);
  PlotLine *ema2 = getEMA(ema, period);
  PlotLine *ema3 = getEMA(ema2, period);
  int emaLoop = ema3->getSize() - 1;

  PlotLine *trix = new PlotLine();

  while (emaLoop > 0)
  {
    trix->prepend(((ema3->getData(emaLoop) - ema3->getData(emaLoop - 1)) / ema3->getData(emaLoop - 1)) * 100);
    emaLoop--;
  }

  PlotLine *trigger = getMA(trix, maType, tperiod);
  trigger->setColor(trigColor);
  trigger->setType(trigLineType);
  trigger->setLabel(trigLabel);

  trix->setColor(color);
  trix->setType(lineType);
  trix->setLabel(label);
  output.append(trix);

  output.append(trigger);

  if (! customFlag)
    delete in;
  delete ema;
  delete ema2;
  delete ema3;
}

int TRIX::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("TRIX Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  if (customFlag)
    dialog->addFormulaInputItem(tr("Input"), tr("Parms"), FALSE, customInput);
  else
    dialog->addComboItem(tr("Input"), tr("Parms"), inputTypeList, input);
  
  dialog->createPage (tr("Trigger Parms"));
  dialog->addColorItem(tr("Trigger Color"), tr("Trigger Parms"), trigColor);
  dialog->addComboItem(tr("Trigger Line Type"), tr("Trigger Parms"), lineTypes, trigLineType);
  dialog->addTextItem(tr("Trigger Label"), tr("Trigger Parms"), trigLabel);
  dialog->addIntItem(tr("Trigger Period"), tr("Trigger Parms"), tperiod, 1, 99999999);
  dialog->addComboItem(tr("Trigger Type"), tr("Trigger Parms"), maTypeList, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    period = dialog->getInt(tr("Period"));
    label = dialog->getText(tr("Label"));
    if (customFlag)
      customInput = dialog->getFormulaInput(tr("Input"));
    else
      input = (BarData::InputType) dialog->getComboIndex(tr("Input"));
    
    trigColor = dialog->getColor(tr("Trigger Color"));
    trigLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Trigger Line Type"));
    tperiod = dialog->getInt(tr("Trigger Period"));
    trigLabel = dialog->getText(tr("Trigger Label"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("Trigger Type"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void TRIX::loadIndicatorSettings (QString file)
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

  s = dict["period"];
  if (s)
    period = s->left(s->length()).toInt();

  s = dict["label"];
  if (s)
    label = s->left(s->length());
      
  s = dict["input"];
  if (s)
    input = (BarData::InputType) s->left(s->length()).toInt();
  
  s = dict["trigColor"];
  if (s)
    trigColor.setNamedColor(s->left(s->length()));
    
  s = dict["trigLineType"];
  if (s)
    trigLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["tperiod"];
  if (s)
    tperiod = s->left(s->length()).toInt();

  s = dict["trigLabel"];
  if (s)
    trigLabel = s->left(s->length());
  
  s = dict["maType"];
  if (s)
    maType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
}

void TRIX::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("period", new QString(QString::number(period)));
  dict.replace("label", new QString(label));
  dict.replace("input", new QString(QString::number(input)));

  dict.replace("trigColor", new QString(trigColor.name()));
  dict.replace("trigLineType", new QString(QString::number(trigLineType)));
  dict.replace("tperiod", new QString(QString::number(tperiod)));
  dict.replace("trigLabel", new QString(trigLabel));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

PlotLine * TRIX::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output.at(0);
}

QString TRIX::getCustomSettings ()
{
  QString s("TRIX");
  s.append("," + customInput);
  s.append("," + QString::number(period));
  s.append("," + color.name());
  s.append("," + QString::number(lineType));
  s.append("," + label);
  return s;
}

void TRIX::setCustomSettings (QString d)
{
  customFlag = TRUE;
  QStringList l = QStringList::split(",", d, FALSE);
  customInput = l[1];
  period = l[2].toInt();
  color.setNamedColor(l[3]);
  lineType = (PlotLine::LineType) l[4].toInt();
  label = l[5];
}

Plugin * create ()
{
  TRIX *o = new TRIX;
  return ((Plugin *) o);
}


