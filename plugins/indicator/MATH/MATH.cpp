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

#include "MATH.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qinputdialog.h>

MATH::MATH ()
{
  pluginName = "MATH";
  customFlag = TRUE;
  
  methodList.append(tr("ADD"));
  methodList.append(tr("DIV"));
  methodList.append(tr("MUL"));
  methodList.append(tr("SUB"));
  methodList.append(tr("MIN"));
  methodList.append(tr("MAX"));
  
  setDefaults();
}

MATH::~MATH ()
{
}

void MATH::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  data1 = "1";
  data2 = "2";
  period = 10;
}

void MATH::calculate ()
{
  if (! method.compare("MIN") || ! method.compare("MAX"))
    calculateMinMax();
  else
    calculateOper();
}

void MATH::calculateMinMax ()
{
  PlotLine *input = customLines->find(data1);
  if (! input)
  {
    qDebug("MATH::calculateMinMax: no data1 input %s", data1.latin1());
    return;
  }
    
  PlotLine *line = new PlotLine;
  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  
  int type = methodList.findIndex(method);
  
  int loop;
  for (loop = period; loop < (int) input->getSize(); loop++)
  {
    int loop2;
    double h = -99999999;
    double l = 99999999;
    for (loop2 = 1; loop2 <= period; loop2++)
    {
      double t = input->getData(loop - loop2);
      if (t > h)
        h = t;
      if (t < l)
        l = t;
    }

    if (type == 4)
      line->append(l);
    else
      line->append(h);
  }
  
  output->addLine(line);
}

void MATH::calculateOper ()
{
  PlotLine *input = customLines->find(data1);
  if (! input)
  {
    qDebug("MATH::calculateOper: no data1 input %s", data1.latin1());
    return;
  }
  int loop = input->getSize() - 1;
    
  int loop2 = 0;
  double inputNum = 0;
  PlotLine *input2 = 0;
  if (data2.contains("#"))
  {
    QString s = data2;
    s.remove(0, 1);
    inputNum = s.toDouble();
  }
  else
  {
    input2 = customLines->find(data2);
    if (! input2)
    {
      qDebug("MATH::calculate: no data2 input");
      return;
    }
    
    loop2 = input2->getSize() - 1;
  }
    
  PlotLine *line = new PlotLine;
  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  
  int type = methodList.findIndex(method);
  
  while (loop > -1)
  {
    if (input2)
    {
      if (loop2 < 0)
        break;
    }
    
    double t = 0;
    
    if (! input2)
      t = inputNum;
    else
      t = input2->getData(loop2);
    
    switch (type)
    {
      case 0:  
        line->prepend(input->getData(loop) + t);
	break;
      case 1:
        line->prepend(input->getData(loop) / t);
	break;
      case 2:
        line->prepend(input->getData(loop) * t);
	break;
      case 3:
        line->prepend(input->getData(loop) - t);
	break;
      default:
        break;
    }
    
    loop--;
    if (input2)
      loop2--;
  }
  
  output->addLine(line);
}

int MATH::indicatorPrefDialog (QWidget *w)
{
  if (! method.length())
  {
    bool ok = FALSE;
    method = QInputDialog::getItem(tr("Select MATH Function"),
                                   tr("Select MATH function"),
				   methodList,
				   0,
				   FALSE,
				   &ok,
				   w);
    if (! ok)
      return FALSE;
  }

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("MATH Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addFormulaInputItem(tr("Data1"), tr("Parms"), FALSE, data1);
  
  QStringList l;
  l.append(method);
  dialog->addComboItem(tr("Method"), tr("Parms"), l, method);
  
  if (! method.compare("MIN") || ! method.compare("MAX"))
    dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  else
    dialog->addFormulaInputItem(tr("Data2"), tr("Parms"), TRUE, data2);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    label = dialog->getText(tr("Label"));
    data1 = dialog->getFormulaInput(tr("Data1"));
    
    if (! method.compare("MIN") || ! method.compare("MAX"))
      period = dialog->getInt(tr("Period"));
    else
      data2 = dialog->getFormulaInput(tr("Data2"));
      
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MATH::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void MATH::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void MATH::setIndicatorSettings (Setting dict)
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

  s = dict.getData("data2");
  if (s.length())
    data2 = s;

  s = dict.getData("period");
  if (s.length())
    period = s.toInt();
}

Setting MATH::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("label", label);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("plugin", pluginName);
  dict.setData("method", method);
  dict.setData("data1", data1);
  dict.setData("data2", data2);
  dict.setData("period", QString::number(period));
  return dict;
}

PlotLine * MATH::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(0);
}

//******************************************************************
//******************************************************************
//******************************************************************

Plugin * create ()
{
  MATH *o = new MATH;
  return ((Plugin *) o);
}



