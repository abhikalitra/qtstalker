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

#include "PER.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

PER::PER ()
{
  pluginName = "PER";
  helpFile = "per.html";
  
  setDefaults();
}

PER::~PER ()
{
}

void PER::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Histogram;
  label = pluginName;
  input = BarData::Close;
}

void PER::calculate ()
{
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("PC::calculate: no input");
    return;
  }

  PlotLine *per = new PlotLine();
  per->setColor(color);
  per->setType(lineType);
  per->setLabel(label);

  double base = in->getData(0);
  int loop;
  for (loop = 1; loop < (int) in->getSize(); loop++)
    per->append(((in->getData(loop) - base) / base) * 100);

  output->addLine(per);
  
  if (! customFlag)
    delete in;
}

int PER::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("PER Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  if (customFlag)
    dialog->addFormulaInputItem(QObject::tr("Input"), QObject::tr("Parms"), FALSE, customInput);
  else
    dialog->addComboItem(QObject::tr("Input"), QObject::tr("Parms"), inputTypeList, input);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    label = dialog->getText(QObject::tr("Label"));
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

void PER::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
}

void PER::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("label", label);
  dict.setData("input", QString::number(input));
  dict.setData("plugin", pluginName);
  dict.setData("customInput", customInput);
}

PlotLine * PER::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(0);
}

int PER::getMinBars ()
{
  int t = minBars + 1;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  PER *o = new PER;
  return ((IndicatorPlugin *) o);
}






