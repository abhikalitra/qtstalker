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

#include "ENV.h"
#include "PrefDialog.h"
#include <qdict.h>

ENV::ENV ()
{
  pluginName = "ENV";
  plotFlag = TRUE;
  
  bandList.append(tr("Upper"));
  bandList.append(tr("Lower"));
  
  setDefaults();
}

ENV::~ENV ()
{
}

void ENV::setDefaults ()
{
  upperColor.setNamedColor("red");
  lowerColor.setNamedColor("red");
  upperLineType = PlotLine::Line;
  lowerLineType = PlotLine::Line;
  upperLabel = tr("ENVU");
  lowerLabel = tr("ENVL");
  period = 10;
  upperPercent = 1.02;
  lowerPercent = 0.98;
  input = BarData::Close;
  maType = IndicatorPlugin::SMA;
  customBand = tr("Upper");
  customInput = "1";
}

void ENV::calculate ()
{
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("ENV::calculate: no input");
    return;
  }

  PlotLine *uma = getMA(in, maType, period);

  PlotLine *lma = getMA(in, maType, period);

  int maLoop = uma->getSize() - 1;

  while (maLoop > -1)
  {
    double t = uma->getData(maLoop);
    uma->setData(maLoop, t * upperPercent);

    t = lma->getData(maLoop);
    lma->setData(maLoop, t * lowerPercent);

    maLoop--;
  }

  if (! customFlag)
    delete in;

  uma->setColor(upperColor);
  uma->setType(upperLineType);
  uma->setLabel(upperLabel);
  output.append(uma);

  lma->setColor(lowerColor);
  lma->setType(lowerLineType);
  lma->setLabel(lowerLabel);
  output.append(lma);
}

int ENV::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("ENV Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  dialog->addComboItem(tr("MA Type"), tr("Parms"), maTypeList, maType);
  if (customFlag)
  {
    dialog->addComboItem(tr("Plot"), tr("Parms"), bandList, customBand);
    dialog->addFormulaInputItem(tr("Input"), tr("Parms"), FALSE, customInput);
  }
  else
    dialog->addComboItem(tr("Input"), tr("Parms"), inputTypeList, input);
  
  dialog->createPage (tr("Upper"));
  dialog->addColorItem(tr("Upper Color"), tr("Upper"), upperColor);
  dialog->addComboItem(tr("Upper Line Type"), tr("Upper"), lineTypes, upperLineType);
  dialog->addTextItem(tr("Upper Label"), tr("Upper"), upperLabel);
  dialog->addFloatItem(tr("Upper %"), tr("Upper"), upperPercent, 0, 99999999.0);
  
  dialog->createPage (tr("Lower"));
  dialog->addColorItem(tr("Lower Color"), tr("Lower"), lowerColor);
  dialog->addComboItem(tr("Lower Line Type"), tr("Lower"), lineTypes, lowerLineType);
  dialog->addTextItem(tr("Lower Label"), tr("Lower"), lowerLabel);
  dialog->addFloatItem(tr("Lower %"), tr("Lower"), lowerPercent, 0, 99999999.0);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    period = dialog->getInt(tr("Period"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("MA Type"));
    if (customFlag)
    {
      customBand = dialog->getCombo(tr("Plot"));
      customInput = dialog->getFormulaInput(tr("Input"));
    }
    else
      input = (BarData::InputType) dialog->getComboIndex(tr("Input"));
    
    upperColor = dialog->getColor(tr("Upper Color"));
    upperLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Upper Line Type"));
    upperLabel = dialog->getText(tr("Upper Label"));
    upperPercent = dialog->getFloat(tr("Upper %"));
    
    lowerColor = dialog->getColor(tr("Lower Color"));
    lowerLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Lower Line Type"));
    lowerLabel = dialog->getText(tr("Lower Label"));
    lowerPercent = dialog->getFloat(tr("Lower %"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void ENV::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["period"];
  if (s)
    period = s->left(s->length()).toInt();
  
  s = dict["maType"];
  if (s)
    maType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();

  s = dict["input"];
  if (s)
    input = (BarData::InputType) s->left(s->length()).toInt();
      
  s = dict["upperColor"];
  if (s)
    upperColor.setNamedColor(s->left(s->length()));
    
  s = dict["upperLineType"];
  if (s)
    upperLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["upperLabel"];
  if (s)
    upperLabel = s->left(s->length());
      
  s = dict["upperPercent"];
  if (s)
    upperPercent = s->left(s->length()).toFloat();
    
  s = dict["lowerColor"];
  if (s)
    lowerColor.setNamedColor(s->left(s->length()));
    
  s = dict["lowerLineType"];
  if (s)
    lowerLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["lowerLabel"];
  if (s)
    lowerLabel = s->left(s->length());
      
  s = dict["lowerPercent"];
  if (s)
    lowerPercent = s->left(s->length()).toFloat();
}

void ENV::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("period", new QString(QString::number(period)));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("input", new QString(QString::number(input)));
  
  dict.replace("upperColor", new QString(upperColor.name()));
  dict.replace("upperLineType", new QString(QString::number(upperLineType)));
  dict.replace("upperLabel", new QString(upperLabel));
  dict.replace("upperPercent", new QString(QString::number(upperPercent)));
  
  dict.replace("lowerColor", new QString(lowerColor.name()));
  dict.replace("lowerLineType", new QString(QString::number(lowerLineType)));
  dict.replace("lowerLabel", new QString(lowerLabel));
  dict.replace("lowerPercent", new QString(QString::number(lowerPercent)));

  dict.replace("plugin", new QString(pluginName));
  
  saveFile(file, dict);
}

PlotLine * ENV::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  if (! customBand.compare(tr("Upper")))
    return output.at(0);
  else
    return output.at(1);
}

QString ENV::getCustomSettings ()
{
  QString s("ENV");
  s.append("," + maType);
  s.append("," + QString::number(period));
  s.append("," + QString::number(input));
  s.append("," + QString::number(bandFlag));
  
  s.append("," + upperColor.name());
  s.append("," + QString::number(upperLineType));
  s.append("," + upperLabel);
  s.append("," + QString::number(upperPercent));
  
  s.append("," + lowerColor.name());
  s.append("," + QString::number(lowerLineType));
  s.append("," + lowerLabel);
  s.append("," + QString::number(lowerPercent));

  s.append("," + customBand);
  s.append("," + customInput);
    
  return s;
}

void ENV::setCustomSettings (QString d)
{
  customFlag = TRUE;

  QStringList l = QStringList::split(",", d, FALSE);
  maType = (IndicatorPlugin::MAType) l[1].toInt();
  period = l[2].toInt();
  input = (BarData::InputType) l[3].toInt();
  bandFlag = l[4].toInt();
  
  upperColor.setNamedColor(l[5]);
  upperLineType = (PlotLine::LineType) l[6].toInt();
  upperLabel = l[7];
  upperPercent = l[8].toDouble();

  lowerColor.setNamedColor(l[9]);
  lowerLineType = (PlotLine::LineType) l[10].toInt();
  lowerLabel = l[11];
  lowerPercent = l[12].toDouble();
  
  customBand = l[13];
  customInput = l[14];
  
}

Plugin * create ()
{
  ENV *o = new ENV;
  return ((Plugin *) o);
}


