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
  alertFlag = TRUE;
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
  input = IndicatorPlugin::Close;
  maType = IndicatorPlugin::SMA;
}

void ENV::calculate ()
{
  PlotLine *in = getInput(input);

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

QMemArray<int> ENV::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 2)
    return alerts;

  PlotLine *uma = output.at(0);
  PlotLine *lma = output.at(1);

  int listLoop = data->count() - uma->getSize();
  int maLoop;
  int status = 0;
  for (maLoop = 0; maLoop < (int) uma->getSize(); maLoop++, listLoop++)
  {
    double close = data->getClose(listLoop);

    double t = (close - lma->getData(maLoop)) / (uma->getData(maLoop) - lma->getData(maLoop));

    switch (status)
    {
      case -1:
        if ((t < .2) || (close < lma->getData(maLoop)))
          status = 1;
        break;
      case 1:
        if ((t > .8) || (close > uma->getData(maLoop)))
	  status = -1;
	break;
      default:
        if ((t < .2) || (close < lma->getData(maLoop)))
	  status = 1;
	else
	{
          if ((t > .8) || (close > uma->getData(maLoop)))
	    status = -1;
	}
	break;
    }
    
    alerts[listLoop] = status;
  }

  return alerts;
}

int ENV::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("ENV Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addIntItem(tr("Period"), 1, period, 1, 99999999);
  dialog->addComboItem(tr("MA Type"), 1, getMATypes(), maType);
  dialog->addComboItem(tr("Input"), 1, getInputFields(), input);
  
  dialog->createPage (tr("Upper"));
  dialog->addColorItem(tr("Upper Color"), 2, upperColor);
  dialog->addComboItem(tr("Upper Line Type"), 2, lineTypes, upperLineType);
  dialog->addTextItem(tr("Upper Label"), 2, upperLabel);
  dialog->addFloatItem(tr("Upper %"), 2, upperPercent, 0, 99999999.0);
  
  dialog->createPage (tr("Lower"));
  dialog->addColorItem(tr("Lower Color"), 3, lowerColor);
  dialog->addComboItem(tr("Lower Line Type"), 3, lineTypes, lowerLineType);
  dialog->addTextItem(tr("Lower Label"), 3, lowerLabel);
  dialog->addFloatItem(tr("Lower %"), 3, lowerPercent, 0, 99999999.0);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    period = dialog->getInt(tr("Period"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("MA Type"));
    input = (IndicatorPlugin::InputType) dialog->getComboIndex(tr("Input"));
    
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
    input = (IndicatorPlugin::InputType) s->left(s->length()).toInt();
      
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

  saveFile(file, dict);
}

Plugin * create ()
{
  ENV *o = new ENV;
  return ((Plugin *) o);
}


