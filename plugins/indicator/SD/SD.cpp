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

#include "SD.h"
#include <math.h>
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

SD::SD ()
{
  pluginName = "SD";
  helpFile = "sd.html";
  
  setDefaults();
}

SD::~SD ()
{
}

void SD::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 21;
  input = BarData::Close;
}

void SD::calculate ()
{
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("SD::calculate: no input");
    return;
  }

  PlotLine *sd = new PlotLine();
  sd->setColor(color);
  sd->setType(lineType);
  sd->setLabel(label);

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
  {
    double mean = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
      mean = mean + in->getData(loop - loop2);
    mean = mean / period;

    double ds = 0;
    for (loop2 = 0; loop2 < period; loop2++)
    {
      double t = in->getData(loop - loop2) - mean;
      ds = ds + (t * t);
    }
    ds = sqrt(ds / period);

    sd->append(ds);
  }

  output->addLine(sd);
  
  if (! customFlag)
    delete in;
}

int SD::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString il = QObject::tr("Input");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("SD Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  if (customFlag)
    dialog->addFormulaInputItem(il, pl, FALSE, customInput);
  else
    dialog->addComboItem(il, pl, inputTypeList, input);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    period = dialog->getInt(perl);
    label = dialog->getText(ll);
    if (customFlag)
      customInput = dialog->getFormulaInput(il);
    else
      input = (BarData::InputType) dialog->getComboIndex(il);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void SD::setIndicatorSettings (Setting &dict)
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
      
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
}

void SD::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("input", QString::number(input));
  dict.setData("customInput", customInput);
  dict.setData("plugin", pluginName);
}

PlotLine * SD::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(0);
}

int SD::getMinBars ()
{
  int t = minBars + period;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  SD *o = new SD;
  return ((IndicatorPlugin *) o);
}


