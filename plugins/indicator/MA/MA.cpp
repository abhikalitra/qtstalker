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

#include "MA.h"
#include "PrefDialog.h"
#include <qdict.h>

MA::MA ()
{
  pluginName = "MA";
  plotFlag = TRUE;
  alertFlag = TRUE;
  setDefaults();
}

MA::~MA ()
{
}

void MA::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 10;
  displace = 0;  
  maType = IndicatorPlugin::SMA;  
  input = IndicatorPlugin::Close;
}

void MA::calculate ()
{
  PlotLine *in = getInput(input);

  PlotLine *data = getMA(in, maType, period, displace);

  delete in;

  data->setColor(color);
  data->setType(lineType);
  data->setLabel(label);
  output.append(data);
}

QMemArray<int> MA::getAlerts ()
{
  alerts.fill(0, data->count());

  if (! output.count())
    return alerts;

  PlotLine *line = output.at(0);

  PlotLine *in = getInput(input);

  int listLoop = data->count() - line->getSize() + 1;
  int lineLoop;
  int status = 0;
  for (lineLoop = 1; listLoop < (int) data->count(); lineLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if ((in->getData(listLoop) > line->getData(lineLoop)) && (in->getData(listLoop - 1) >= line->getData(lineLoop - 1)))
	  status = 1;
	break;
      case 1:
        if ((in->getData(listLoop) < line->getData(lineLoop)) && (in->getData(listLoop - 1) <= line->getData(lineLoop - 1)))
	  status = -1;
	break;
      default:
        if ((in->getData(listLoop) > line->getData(lineLoop)) && (in->getData(listLoop - 1) >= line->getData(lineLoop - 1)))
	  status = 1;
	else
	{
          if ((in->getData(listLoop) < line->getData(lineLoop)) && (in->getData(listLoop - 1) <= line->getData(lineLoop - 1)))
	    status = -1;
	}
	break;
    }
    
    alerts[listLoop] = status;
  }

  delete in;

  return alerts;
}

int MA::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("MA Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), 1, color);
  dialog->addComboItem(tr("Line Type"), 1, lineTypes, lineType);
  dialog->addTextItem(tr("Label"), 1, label);
  dialog->addIntItem(tr("Period"), 1, period, 1, 99999999);
  dialog->addComboItem(tr("MA Type"), 1, getMATypes(), maType);
  dialog->addComboItem(tr("Input"), 1, getInputFields(), input);
  dialog->addIntItem(tr("Displace"), 1, displace, 0, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    period = dialog->getInt(tr("Period"));
    label = dialog->getText(tr("Label"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("MA Type"));
    displace = dialog->getInt(tr("Displace"));
    input = (IndicatorPlugin::InputType) dialog->getComboIndex(tr("Input"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MA::loadIndicatorSettings (QString file)
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
      
  s = dict["maType"];
  if (s)
    maType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
    
  s = dict["input"];
  if (s)
    input = (IndicatorPlugin::InputType) s->left(s->length()).toInt();
    
  s = dict["displace"];
  if (s)
    displace = s->left(s->length()).toInt();
}

void MA::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("period", new QString(QString::number(period)));
  dict.replace("label", new QString(label));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("input", new QString(QString::number(input)));
  dict.replace("displace", new QString(QString::number(displace)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  MA *o = new MA;
  return ((Plugin *) o);
}


