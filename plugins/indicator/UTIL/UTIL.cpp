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

#include "UTIL.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qinputdialog.h>
#include <qobject.h>
#include <math.h>

UTIL::UTIL ()
{
  pluginName = "UTIL";
  customFlag = TRUE;
  
  methodList.append("ACCUM");
  methodList.append("TypPrice");
  methodList.append("Normal");

  helpFile = "math.html";
    
  setDefaults();
}

UTIL::~UTIL ()
{
}

void UTIL::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  data1 = "1";
  method = "ACCUM";
}

void UTIL::calculate ()
{
  while (1)
  {
    if (! method.compare("ACCUM"))
    {
      calculateAccum();
      break;
    }

    if (! method.compare("TypPrice"))
    {
      calculateTypPrice();
      break;
    }

    if (! method.compare("Normal"))
    {
      calculateNormal();
      break;
    }

    break;
  }
}

void UTIL::calculateAccum ()
{
  PlotLine *input = customLines->find(data1);
  if (! input)
  {
    qDebug("UTIL::calculateAccum: no data1 input %s", data1.latin1());
    return;
  }
    
  PlotLine *line = new PlotLine;
  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  
  int loop;
  double accum = 0;
  for (loop = 0; loop < (int) input->getSize(); loop++)
  {
    accum = accum + input->getData(loop);
    line->append(accum);
  }
  
  output->addLine(line);
}

void UTIL::calculateTypPrice()
{
  // n.b. no ratl input here....
  PlotLine *input = customLines->find(data1);
  if (! input)
  {
    qDebug("UTIL::calculateTypPrice: no data1 input %s", data1.latin1());
    return;
  }

  PlotLine *typPrice = new PlotLine;
  typPrice->setColor(color);
  typPrice->setType(lineType);
  typPrice->setLabel(label);

  int loop;

  for (loop = 0; loop < (int) data->count(); loop++)
    typPrice->append((data->getHigh(loop) + data->getLow(loop) + data->getClose(loop)) / 3);
	
  output->addLine(typPrice);
}

void UTIL::calculateNormal()
{
  PlotLine *input = customLines->find(data1);
  if (! input)
  {
    qDebug("UTIL::calculateNormal: no data1 input %s", data1.latin1());
    return;
  }

  PlotLine *normal = new PlotLine;
  normal->setColor(color);
  normal->setType(lineType);
  normal->setLabel(label);

  int loop = 0;
  double range = 0;
  double max = -99999999.0;
  double min = 99999999.0;
  double norm = 0;
  for (loop = 0; loop < input->getSize(); loop++)
  {
    if (input->getData(loop) > max)
      max = input->getData(loop);

    if (input->getData(loop) < min)
      min = input->getData(loop);
  }
	
  range = fabs(max) + fabs(min);
	
  for (loop = 0; loop < input->getSize(); loop++)
  {	
    norm = ((input->getData(loop) - min) / range) * 100;
    input->append(norm);
  }
	
  output->addLine(normal);
}

int UTIL::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString d1l = QObject::tr("Data1");
  QString ml = QObject::tr("Method");
  
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("UTIL Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addComboItem(ml, pl, methodList, method);
  dialog->addFormulaInputItem(d1l, pl, FALSE, data1);

  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    data1 = dialog->getFormulaInput(d1l);    
    method = dialog->getCombo(ml);
      
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void UTIL::setIndicatorSettings (Setting &dict)
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
    
  s = dict.getData("method");
  if (s.length())
    method = s;

  s = dict.getData("data1");
  if (s.length())
    data1 = s;

  s = dict.getData("method");
  if (s.length())
    method = s;
}

void UTIL::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("label", label);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("plugin", pluginName);
  dict.setData("method", method);
  dict.setData("data1", data1);
}

PlotLine * UTIL::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(0);
}

int UTIL::getMinBars ()
{
  int t = minBars;
  return t;
}

//******************************************************************
//******************************************************************
//******************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  UTIL *o = new UTIL;
  return ((IndicatorPlugin *) o);
}



