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

#include "COMP.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

COMP::COMP ()
{
  pluginName = "COMP";
  customFlag = TRUE;
  
  opList.append("EQ");
  opList.append("LT");
  opList.append("LTEQ");
  opList.append("GT");
  opList.append("GTEQ");
  opList.append("AND");
  opList.append("OR");
  
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
  data1 = "1";
  data2 = "2";
  delay1 = "#0";
  delay2 = "#0";
}

void COMP::calculate ()
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
  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  
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

int COMP::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString d1l = QObject::tr("Data1");
  QString d2l = QObject::tr("Data2");
  QString y1l = QObject::tr("Delay1");
  QString y2l = QObject::tr("Delay2");
  QString ml = QObject::tr("Method");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("COMP Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addFormulaInputItem(d1l, pl, FALSE, data1);
  dialog->addFormulaInputItem(y1l, pl, TRUE, delay1);
  dialog->addComboItem(ml, pl, opList, method);
  dialog->addFormulaInputItem(d2l, pl, TRUE, data2);
  dialog->addFormulaInputItem(y2l, pl, TRUE, delay2);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    method = dialog->getCombo(ml);
    data1 = dialog->getFormulaInput(d1l);
    delay1 = dialog->getFormulaInput(y1l);
    data2 = dialog->getFormulaInput(d2l);
    delay2 = dialog->getFormulaInput(y2l);
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
}

void COMP::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("label", label);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("plugin", pluginName);
  dict.setData("method", method);
  dict.setData("data1", data1);
  dict.setData("delay1", delay1);
  dict.setData("data2", data2);
  dict.setData("delay2", delay2);
}

PlotLine * COMP::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(0);
}

COMP::Operator COMP::getOperator (QString d)
{
  Operator op = NoOp;
  
  while (1)
  {
    if (! d.compare("EQ"))
    {
      op = Equal;
      break;
    }
    
    if (! d.compare("LT"))
    {
      op = LessThan;
      break;
    }
  
    if (! d.compare("LTEQ"))
    {
      op = LessThanEqual;
      break;
    }
      
    if (! d.compare("GT"))
    {
      op = GreaterThan;
      break;
    }
    
    if (! d.compare("AND"))
    {
      op = And;
      break;
    }
    
    if (! d.compare("OR"))
    {
      op = Or;
      break;
    }
    
    if (! d.compare("GTEQ"))
      op = GreaterThanEqual;
    
    break;
  }
  
  return op;
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



