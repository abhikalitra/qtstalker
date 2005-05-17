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

#include "COMP.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

COMP::COMP ()
{
  pluginName = "COMP";
  customFlag = TRUE;
  helpFile = "comp.html";
  
  setDefaults();
}

COMP::~COMP ()
{
}

void COMP::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  method = "EQ";
  method2 = "EQ";
  data1 = "1";
  data2 = "2";
  data3 = "1";
  data4 = "#0";
  data5 = "#0";
  data6 = "#0";
  delay1 = "#0";
  delay2 = "#0";
  delay3 = "#0";
  delay4 = "#0";
  andor = "NOP";
}

void COMP::calculate ()
{
  calculateIf();
  if (! output->getLines())
    return;
    
  calculateAndOr();
  
  calculateThen();
}

void COMP::calculateIf ()
{
  PlotLine *input = customLines->find(data1);
  if (! input)
  {
    qDebug("COMP::calculate: no data1 input %s", data1.latin1());
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
      qDebug("COMP::calculate: no data2 input");
      return;
    }
    
    loop2 = input2->getSize() - 1;
  }

  int timeLoop1 = 0;
  int displace1 = 0;
  PlotLine *timeInput1 = 0;
  if (delay1.contains("#"))
  {
    QString s = delay1;
    s.remove(0, 1);
    displace1 = s.toInt();
  }
  else
  {
    timeInput1 = customLines->find(delay1);
    if (! timeInput1)
    {
      qDebug("COMP::calculate: no delay1 input");
      return;
    }
    timeLoop1 = timeInput1->getSize() - 1;
    displace1 = (int) timeInput1->getData(timeLoop1);
  }
  
  int timeLoop2 = 0;
  int displace2 = 0;
  PlotLine *timeInput2 = 0;
  if (delay2.contains("#"))
  {
    QString s = delay2;
    s.remove(0, 1);
    displace2 = s.toInt();
  }
  else
  {
    timeInput2 = customLines->find(delay2);
    if (! timeInput2)
    {
      qDebug("COMP::calculate: no delay2 input");
      return;
    }
    timeLoop2 = timeInput2->getSize() - 1;
    displace2 = (int) timeInput2->getData(timeLoop2);
  }
    
  PlotLine *line = new PlotLine;
  
  Operator op = getOperator(method);
  
  while (loop - displace1 > -1 && loop - displace1 < input->getSize())
  {
    if (input2)
    {
      if (loop2 - displace2 < 0 || loop2 - displace2 >= input2->getSize())
        break;
    }
    
    double t = 0;
    
    if (! input2)
      t = inputNum;
    else
      t = input2->getData(loop2 - displace2);
      
    switch (op)
    {
      case Equal:
        if (input->getData(loop - displace1) == t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case LessThan:
        if (input->getData(loop - displace1) < t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case LessThanEqual:
        if (input->getData(loop - displace1) <= t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case GreaterThan:
        if (input->getData(loop - displace1) > t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case GreaterThanEqual:
        if (input->getData(loop - displace1) >= t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case And:
        if (input->getData(loop - displace1) && t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case Or:
        if (input->getData(loop - displace1) || t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      default:
        break;
    }
      
    loop--;
    if(timeInput1 && timeLoop1 > 0)
    {
      timeLoop1--;
      displace1 = (int) timeInput1->getData(timeLoop1);
    }
    
    if (input2)
    {
      loop2--;
      if(timeInput2 && timeLoop2 > 0)
      {
        timeLoop2--;
        displace2 = (int) timeInput2->getData(timeLoop2);
      }
    }
  }
  
  output->addLine(line);
}

void COMP::calculateAndOr ()
{
  if (! andor.compare("NOP"))
    return;
    
  PlotLine *input = customLines->find(data3);
  if (! input)
  {
    qDebug("COMP::calculateAndOr: no data3 input %s", data3.latin1());
    return;
  }
  int loop = input->getSize() - 1;
    
  int loop2 = 0;
  double inputNum = 0;
  PlotLine *input2 = 0;
  if (data4.contains("#"))
  {
    QString s = data4;
    s.remove(0, 1);
    inputNum = s.toDouble();
  }
  else
  {
    input2 = customLines->find(data4);
    if (! input2)
    {
      qDebug("COMP::calculate: no data4 input");
      return;
    }
    
    loop2 = input2->getSize() - 1;
  }

  int timeLoop1 = 0;
  int displace1 = 0;
  PlotLine *timeInput1 = 0;
  if (delay3.contains("#"))
  {
    QString s = delay3;
    s.remove(0, 1);
    displace1 = s.toInt();
  }
  else
  {
    timeInput1 = customLines->find(delay3);
    if (! timeInput1)
    {
      qDebug("COMP::calculate: no delay3 input");
      return;
    }
    timeLoop1 = timeInput1->getSize() - 1;
    displace1 = (int) timeInput1->getData(timeLoop1);
  }
  
  int timeLoop2 = 0;
  int displace2 = 0;
  PlotLine *timeInput2 = 0;
  if (delay4.contains("#"))
  {
    QString s = delay4;
    s.remove(0, 1);
    displace2 = s.toInt();
  }
  else
  {
    timeInput2 = customLines->find(delay4);
    if (! timeInput2)
    {
      qDebug("COMP::calculate: no delay4 input");
      return;
    }
    timeLoop2 = timeInput2->getSize() - 1;
    displace2 = (int) timeInput2->getData(timeLoop2);
  }
    
  PlotLine *line = new PlotLine;
  
  Operator op = getOperator(method2);
  
  while (loop - displace1 > -1 && loop - displace1 < input->getSize())
  {
    if (input2)
    {
      if (loop2 - displace2 < 0 || loop2 - displace2 >= input2->getSize())
        break;
    }
    
    double t = 0;
    
    if (! input2)
      t = inputNum;
    else
      t = input2->getData(loop2 - displace2);
      
    switch (op)
    {
      case Equal:
        if (input->getData(loop - displace1) == t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case LessThan:
        if (input->getData(loop - displace1) < t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case LessThanEqual:
        if (input->getData(loop - displace1) <= t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case GreaterThan:
        if (input->getData(loop - displace1) > t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case GreaterThanEqual:
        if (input->getData(loop - displace1) >= t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case And:
        if (input->getData(loop - displace1) && t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case Or:
        if (input->getData(loop - displace1) || t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      default:
        break;
    }
      
    loop--;
    if(timeInput1 && timeLoop1 > 0)
    {
      timeLoop1--;
      displace1 = (int) timeInput1->getData(timeLoop1);
    }
    
    if (input2)
    {
      loop2--;
      if(timeInput2 && timeLoop2 > 0)
      {
        timeLoop2--;
        displace2 = (int) timeInput2->getData(timeLoop2);
      }
    }
  }
  
  output->addLine(line);
}

void COMP::calculateThen ()
{
  PlotLine *input = output->getLine(0);
  int inloop = input->getSize() - 1;
  
  int inloop2 = -1;
  PlotLine *input2 = output->getLine(1);
  if (input2)
    inloop2 = input2->getSize() - 1;

  int inloop3 = -1;
  double inputNum = 0;
  PlotLine *input3 = 0;
  if (data5.contains("#"))
  {
    QString s = data5;
    s.remove(0, 1);
    inputNum = s.toDouble();
  }
  else
  {
    input3 = customLines->find(data5);
    if (! input3)
    {
      qDebug("COMP::calculateThen: no data5 input");
      return;
    }
    inloop3 = input3->getSize() - 1;
  }
  
  int inloop4 = 0;
  double inputNum2 = 0;
  PlotLine *input4 = 0;
  if (data6.contains("#"))
  {
    QString s = data6;
    s.remove(0, 1);
    inputNum2 = s.toDouble();
  }
  else
  {
    input4 = customLines->find(data6);
    if (! input4)
    {
      qDebug("COMP::calculateThen: no data6 input");
      return;
    }
    inloop4 = input4->getSize() - 1;
  }
  
  PlotLine *line = new PlotLine;
  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  
  while (inloop > -1)
  {
    if (input2)
    {
      if (inloop2 < 0)
        break;
    }
    
    if (input3)
    {
      if (inloop3 < 0)
        break;
      inputNum = input3->getData(inloop3);
    }
    
    if (input4)
    {
      if (inloop4 < 0)
        break;
      inputNum2 = input4->getData(inloop4);
    }
    
    if (! input2)
    {
      if (input->getData(inloop))
        line->prepend(inputNum);
      else
        line->prepend(inputNum2);
    }
    else
    {
      if (! andor.compare("AND"))
      {
        if (input->getData(inloop) && input2->getData(inloop2))
          line->prepend(inputNum);
	else
          line->prepend(inputNum2);
      }
      else
      {
        if (input->getData(inloop) || input2->getData(inloop2))
          line->prepend(inputNum);
	else
          line->prepend(inputNum2);
      }
    }
    
    inloop--;
    inloop2--;
    inloop3--;
    inloop4--;
  }
  
  output->clearLines();
  output->addLine(line);
}

int COMP::indicatorPrefDialog (QWidget *w)
{
  QStringList l;
  l.append("NOP");
  l.append("AND");
  l.append("OR");
  
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString d1l = QObject::tr("Data1");
  QString d2l = QObject::tr("Data2");
  QString d3l = QObject::tr("Data3");
  QString d4l = QObject::tr("Data4");
  QString d5l = QObject::tr("Data5");
  QString d6l = QObject::tr("Data6");
  QString y1l = QObject::tr("Delay1");
  QString y2l = QObject::tr("Delay2");
  QString y3l = QObject::tr("Delay3");
  QString y4l = QObject::tr("Delay4");
  QString ml = QObject::tr("Method");
  QString m2l = QObject::tr("Method 2");
  QString al = QObject::tr("AND/OR");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("COMP Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  
  pl = QObject::tr("If");
  dialog->createPage (pl);
  dialog->addFormulaInputItem(d1l, pl, FALSE, data1);
  dialog->addFormulaInputItem(y1l, pl, TRUE, delay1);
  dialog->addComboItem(ml, pl, opList, method);
  dialog->addFormulaInputItem(d2l, pl, TRUE, data2);
  dialog->addFormulaInputItem(y2l, pl, TRUE, delay2);
  
  pl = QObject::tr("And/Or");
  dialog->createPage (pl);
  dialog->addComboItem(al, pl, l, andor);
  dialog->addFormulaInputItem(d3l, pl, FALSE, data3);
  dialog->addFormulaInputItem(y3l, pl, TRUE, delay3);
  dialog->addComboItem(m2l, pl, opList, method2);
  dialog->addFormulaInputItem(d4l, pl, TRUE, data4);
  dialog->addFormulaInputItem(y4l, pl, TRUE, delay4);
  
  pl = QObject::tr("Then");
  dialog->createPage (pl);
  dialog->addFormulaInputItem(d5l, pl, TRUE, data5);
  
  pl = QObject::tr("Else");
  dialog->createPage (pl);
  dialog->addFormulaInputItem(d6l, pl, TRUE, data6);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    method = dialog->getCombo(ml);
    method2 = dialog->getCombo(m2l);
    data1 = dialog->getFormulaInput(d1l);
    delay1 = dialog->getFormulaInput(y1l);
    data2 = dialog->getFormulaInput(d2l);
    delay2 = dialog->getFormulaInput(y2l);
    data3 = dialog->getFormulaInput(d3l);
    delay3 = dialog->getFormulaInput(y3l);
    data4 = dialog->getFormulaInput(d4l);
    delay4 = dialog->getFormulaInput(y4l);
    data5 = dialog->getFormulaInput(d5l);
    data6 = dialog->getFormulaInput(d6l);
    andor = dialog->getCombo(al);
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void COMP::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("method2");
  if (s.length())
    method2 = s;
  
  s = dict.getData("data1");
  if (s.length())
    data1 = s;

  s = dict.getData("delay1");
  if (s.length())
    delay1 = s;

  s = dict.getData("data2");
  if (s.length())
    data2 = s;

  s = dict.getData("delay2");
  if (s.length())
    delay2 = s;

  s = dict.getData("data3");
  if (s.length())
    data3 = s;

  s = dict.getData("delay3");
  if (s.length())
    delay3 = s;

  s = dict.getData("data4");
  if (s.length())
    data4 = s;

  s = dict.getData("delay4");
  if (s.length())
    delay4 = s;

  s = dict.getData("data5");
  if (s.length())
    data5 = s;

  s = dict.getData("data6");
  if (s.length())
    data6 = s;

  s = dict.getData("andor");
  if (s.length())
    andor = s;
}

void COMP::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("label", label);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("plugin", pluginName);
  dict.setData("method", method);
  dict.setData("method2", method2);
  dict.setData("data1", data1);
  dict.setData("delay1", delay1);
  dict.setData("data2", data2);
  dict.setData("delay2", delay2);
  dict.setData("data3", data3);
  dict.setData("delay3", delay3);
  dict.setData("data4", data4);
  dict.setData("delay4", delay4);
  dict.setData("data5", data5);
  dict.setData("data6", data6);
  dict.setData("andor", andor);
}

PlotLine * COMP::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(0);
}

int COMP::getMinBars ()
{
  int t = minBars;
  if (delay1.toInt() > delay2.toInt())
    t = t + delay1.toInt();
  else
  {
    if (delay2.toInt() > delay1.toInt())
      t = t + delay2.toInt();
    else
      t = t + delay1.toInt();
  }
  
  return t;
}

//***************************************************************
//***************************************************************
//***************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  COMP *o = new COMP;
  return ((IndicatorPlugin *) o);
}



