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
#include <qobject.h>

MATH::MATH ()
{
  pluginName = "MATH";
  customFlag = TRUE;
  
  methodList.append("ADD");
  methodList.append("DIV");
  methodList.append("MUL");
  methodList.append("SUB");
  methodList.append("MIN");
  methodList.append("MAX");

  helpFile = "math.html";
    
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
  data2 = "1";
  data3 = "#0";
  data4 = "#0";
  data5 = "#0";
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
    for (loop2 = 0; loop2 <= period; loop2++)
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
  double inputNum2 = 0;
  PlotLine *input2 = 0;
  if (data2.contains("#"))
  {
    QString s = data2;
    s.remove(0, 1);
    inputNum2 = s.toDouble();
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
    
  int loop3 = 0;
  double inputNum3 = 0;
  PlotLine *input3 = 0;
  if (data3.contains("#"))
  {
    QString s = data3;
    s.remove(0, 1);
    inputNum3 = s.toDouble();
  }
  else
  {
    input3 = customLines->find(data3);
    if (input3)
      loop3 = input3->getSize() - 1;
  }
  
  int loop4 = 0;
  double inputNum4 = 0;
  PlotLine *input4 = 0;
  if (data4.contains("#"))
  {
    QString s = data4;
    s.remove(0, 1);
    inputNum4 = s.toDouble();
  }
  else
  {
    input4 = customLines->find(data4);
    if (input4)
      loop4 = input4->getSize() - 1;
  }
  
  int loop5 = 0;
  double inputNum5 = 0;
  PlotLine *input5 = 0;
  if (data5.contains("#"))
  {
    QString s = data5;
    s.remove(0, 1);
    inputNum5 = s.toDouble();
  }
  else
  {
    input5 = customLines->find(data5);
    if (input5)
      loop5 = input5->getSize() - 1;
  }
  
  PlotLine *line = new PlotLine;
  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  
  int type = methodList.findIndex(method);
  
  while (loop > -1)
  {
    double v = input->getData(loop);
    
    if (input2)
    {
      if (loop2 < 0)
        break;
    
      switch (type)
      {
        case 0:
	  v = v + input2->getData(loop2);
	  break;
        case 1:
	  v = v / input2->getData(loop2);
	  break;
        case 2:
	  v = v * input2->getData(loop2);
	  break;
        case 3:
	  v = v - input2->getData(loop2);
	  break;
        default:
          break;
      }
      
      loop2--;
    }
    else
    {
      switch (type)
      {
        case 0:
	  v = v + inputNum2;
	  break;
        case 1:
	  if (inputNum2)
	    v = v / inputNum2;
	  break;
        case 2:
	  if (inputNum2)
	    v = v * inputNum2;
	  break;
        case 3:
	  v = v - inputNum2;
	  break;
        default:
          break;
      }
    }
    
    if (input3)
    {
      if (loop3 < 0)
        break;
    
      switch (type)
      {
        case 0:
	  v = v + input3->getData(loop3);
	  break;
        case 1:
	  v = v / input3->getData(loop3);
	  break;
        case 2:
	  v = v * input3->getData(loop3);
	  break;
        case 3:
	  v = v - input3->getData(loop3);
	  break;
        default:
          break;
      }
      
      loop3--;
    }
    else
    {
      switch (type)
      {
        case 0:
	  v = v + inputNum3;
	  break;
        case 1:
	  if (inputNum3)
	    v = v / inputNum3;
	  break;
        case 2:
	  if (inputNum3)
	    v = v * inputNum3;
	  break;
        case 3:
	  v = v - inputNum3;
	  break;
        default:
          break;
      }
    }
    
    if (input4)
    {
      if (loop4 < 0)
        break;
    
      switch (type)
      {
        case 0:
	  v = v + input4->getData(loop4);
	  break;
        case 1:
	  v = v / input4->getData(loop4);
	  break;
        case 2:
	  v = v * input4->getData(loop4);
	  break;
        case 3:
	  v = v - input4->getData(loop4);
	  break;
        default:
          break;
      }
      
      loop4--;
    }
    else
    {
      switch (type)
      {
        case 0:
	  v = v + inputNum4;
	  break;
        case 1:
	  if (inputNum4)
	    v = v / inputNum4;
	  break;
        case 2:
	  if (inputNum4)
	    v = v * inputNum4;
	  break;
        case 3:
	  v = v - inputNum4;
	  break;
        default:
          break;
      }
    }
    
    if (input5)
    {
      if (loop5 < 0)
        break;
    
      switch (type)
      {
        case 0:
	  v = v + input5->getData(loop5);
	  break;
        case 1:
	  v = v / input5->getData(loop5);
	  break;
        case 2:
	  v = v * input5->getData(loop5);
	  break;
        case 3:
	  v = v - input5->getData(loop5);
	  break;
        default:
          break;
      }
      
      loop5--;
    }
    else
    {
      switch (type)
      {
        case 0:
	  v = v + inputNum5;
	  break;
        case 1:
	  if (inputNum5)
	    v = v / inputNum5;
	  break;
        case 2:
	  if (inputNum5)
	    v = v * inputNum5;
	  break;
        case 3:
	  v = v - inputNum5;
	  break;
        default:
          break;
      }
    }
    
    line->prepend(v);
    loop--;
  }
  
  output->addLine(line);
}

int MATH::indicatorPrefDialog (QWidget *w)
{
  if (! method.length())
  {
    bool ok = FALSE;
    method = QInputDialog::getItem(QObject::tr("Select MATH Function"),
                                   QObject::tr("Select MATH function"),
				   methodList,
				   0,
				   FALSE,
				   &ok,
				   w);
    if (! ok)
      return FALSE;
  }

  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString d1l = QObject::tr("Data1");
  QString d2l = QObject::tr("Data2");
  QString d3l = QObject::tr("Data3");
  QString d4l = QObject::tr("Data4");
  QString d5l = QObject::tr("Data5");
  QString perl = QObject::tr("Period");
  QString ml = QObject::tr("Method");
  
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("MATH Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  
  QStringList l;
  l.append(method);
  dialog->addComboItem(ml, pl, l, method);
  
  dialog->addFormulaInputItem(d1l, pl, FALSE, data1);
  
  if (! method.compare("MIN") || ! method.compare("MAX"))
    dialog->addIntItem(perl, pl, period, 1, 99999999);
  else
  {
    dialog->addFormulaInputItem(d2l, pl, TRUE, data2);
    dialog->addFormulaInputItem(d3l, pl, TRUE, data3);
    dialog->addFormulaInputItem(d4l, pl, TRUE, data4);
    dialog->addFormulaInputItem(d5l, pl, TRUE, data5);
  }
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    data1 = dialog->getFormulaInput(d1l);    
    
    if (! method.compare("MIN") || ! method.compare("MAX"))
      period = dialog->getInt(perl);
    else
    {
      data2 = dialog->getFormulaInput(d2l);
      data3 = dialog->getFormulaInput(d3l);
      data4 = dialog->getFormulaInput(d4l);
      data5 = dialog->getFormulaInput(d5l);
    }
      
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MATH::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("data3");
  if (s.length())
    data3 = s;
  
  s = dict.getData("data4");
  if (s.length())
    data4 = s;
  
  s = dict.getData("data5");
  if (s.length())
    data5 = s;
  
  s = dict.getData("period");
  if (s.length())
    period = s.toInt();
}

void MATH::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("label", label);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("plugin", pluginName);
  dict.setData("method", method);
  dict.setData("data1", data1);
  dict.setData("data2", data2);
  dict.setData("data3", data3);
  dict.setData("data4", data4);
  dict.setData("data5", data5);
  dict.setData("period", QString::number(period));
}

PlotLine * MATH::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(0);
}

int MATH::getMinBars ()
{
  int t = minBars + period;
  return t;
}

//******************************************************************
//******************************************************************
//******************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MATH *o = new MATH;
  return ((IndicatorPlugin *) o);
}



