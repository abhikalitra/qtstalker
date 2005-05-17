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

#include "COUNTER.h"
#include "PrefDialog.h"
#include <qdict.h>

COUNTER::COUNTER ()
{
  pluginName = "COUNTER";
  helpFile = "counter.html";
  
  setDefaults();
}

COUNTER::~COUNTER ()
{
}

void COUNTER::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::HistogramBar;
  label = pluginName;
  customInput = "1";
  customInput2 = "2";
}

void COUNTER::calculate ()
{
  PlotLine *counter = new PlotLine;
  PlotLine *in = 0;
  PlotLine *in2 = 0;

  int inLoop = 0;
  int in2Loop = 0;
  in = getInputLine(customInput);
  if (! in )
  {
    qDebug("COUNTER::calculate: missing input");
    return;
  }
  
  if(customInput2.toInt() != customInput.toInt())
  {
    in2 = getInputLine(customInput2);
    if (! in2 )
    {
      qDebug("COUNTER::calculate: missing reset");
      return;
    }
    
    in2Loop=in2->getSize()-in->getSize();
    if(in2Loop<0)
    {
      inLoop=-in2Loop;
      in2Loop=0;
    }
  }

  int t=0;

  while (inLoop < in->getSize())
  {
    if(in2)
    {
      if(in2->getData(in2Loop)) t=0;
      if(in->getData(inLoop)) t++;
      in2Loop++;
    }
    else
    {
      if(in->getData(inLoop))
        t=1;
      else
        t++;
    }
    
    counter->append(t);
    inLoop++;
  }

  counter->setColor(color);
  counter->setType(lineType);
  counter->setLabel(label);
  output->addLine(counter);
}

int COUNTER::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString il = QObject::tr("Input");
  QString rl = QObject::tr("Reset");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("COUNTER Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addFormulaInputItem(il, pl, FALSE, customInput);
  dialog->addFormulaInputItem(rl, pl, FALSE, customInput2);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    customInput = dialog->getFormulaInput(il);
    customInput2 = dialog->getFormulaInput(rl);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void COUNTER::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;

  s = dict.getData("customInput2");
  if (s.length())
    customInput2 = s;
}

void COUNTER::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
  dict.setData("customInput", customInput);
  dict.setData("customInput2", customInput2);
}

PlotLine * COUNTER::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(0);
}

IndicatorPlugin * createIndicatorPlugin ()
{
  COUNTER *o = new COUNTER;
  return ((IndicatorPlugin *) o);
}





