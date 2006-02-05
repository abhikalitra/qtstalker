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
  
  methodList.append("ACCUM");
  methodList.append("Normal");
  methodList.append("ADD");
  methodList.append("DIV");
  methodList.append("MUL");
  methodList.append("SUB");
  methodList.append("COMP");
  methodList.append("COUNTER");
  methodList.append("REF");
  methodList.append("PER");
  methodList.append("COLOR");
  methodList.sort();

  helpFile = "math.html";
}

UTIL::~UTIL ()
{
}

PlotLine * UTIL::calculateAccum (QString &p, QPtrList<PlotLine> &d)
{
  // format: METHOD, INPUT_ARRAY

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 2)
    ;
  else
  {
    qDebug("UTIL::calculateAccum: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("UTIL::calculateAccum: invalid ARRAY_INPUT parm");
    return 0;
  }

  clearOutput();

  PlotLine *line = new PlotLine;
  PlotLine *input = d.at(0);
  
  int loop;
  double accum = 0;
  for (loop = 0; loop < (int) input->getSize(); loop++)
  {
    accum = accum + input->getData(loop);
    line->append(accum);
  }
  
  output->addLine(line);
  return output->getLine(0);
}

PlotLine * UTIL::calculateNormal(QString &p, QPtrList<PlotLine> &d)
{
  // format: METHOD, INPUT_ARRAY

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 2)
    ;
  else
  {
    qDebug("UTIL::calculateNormal: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("UTIL::calculateNormal: invalid ARRAY_INPUT parm");
    return 0;
  }

  clearOutput();

  PlotLine *input = d.at(0);

  PlotLine *normal = new PlotLine;
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
  return output->getLine(0);
}

PlotLine * UTIL::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() > 0)
    ;
  else
  {
    qDebug("UTIL::calculateCustom: invalid parm count");
    return 0;
  }

  if (methodList.findIndex(l[0]) == -1)
  {
    qDebug("UTIL::calculateCustom: invalid METHOD parm");
    return 0;
  }

  PlotLine *out = 0;

  while (1)
  {
    if (! l[0].compare("ACCUM"))
    {
      out = calculateAccum(p, d);
      break;
    }

    if (! l[0].compare("Normal"))
    {
      out = calculateNormal(p, d);
      break;
    }

    if (! l[0].compare("ADD"))
    {
      out = calculateADD(p, d);
      break;
    }

    if (! l[0].compare("DIV"))
    {
      out = calculateDIV(p, d);
      break;
    }

    if (! l[0].compare("MUL"))
    {
      out = calculateMUL(p, d);
      break;
    }

    if (! l[0].compare("SUB"))
    {
      out = calculateSUB(p, d);
      break;
    }

    if (! l[0].compare("COMP"))
    {
      out = calculateCOMP(p, d);
      break;
    }

    if (! l[0].compare("COUNTER"))
    {
      out = calculateCOUNTER(p, d);
      break;
    }

    if (! l[0].compare("REF"))
    {
      out = calculateREF(p, d);
      break;
    }

    if (! l[0].compare("PER"))
    {
      out = calculatePER(p, d);
      break;
    }

    if (! l[0].compare("COLOR"))
    {
      out = calculateCOLOR(p, d);
      break;
    }

    break;
  }

  return out;
}

PlotLine * UTIL::calculateCOUNTER (QString &p, QPtrList<PlotLine> &d)
{
  // format1: METHOD, ARRAY_INPUT
  // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 2 || l.count() == 3)
    ;
  else
  {
    qDebug("UTIL::COUNTER: invalid parm count");
    return 0;
  }

  PlotLine *in = 0;
  PlotLine *in2 = 0;
  if (! d.count())
  {
    qDebug("UTIL::COUNTER: invalid ARRAY_INPUT parm");
    return 0;
  }
  else
    in = d.at(0);

  if (d.count() == 2)
    in2 = d.at(1);

  clearOutput();

  int inLoop = 0;
  int in2Loop = 0;
  
  if (in2)
  {
    in2Loop = in2->getSize() - in->getSize();
    if (in2Loop < 0)
    {
      inLoop=-in2Loop;
      in2Loop = 0;
    }
  }

  int t = 0;
  PlotLine *counter = new PlotLine;

  while (inLoop < in->getSize())
  {
    if (in2)
    {
      if (in2->getData(in2Loop))
        t = 0;
      if (in->getData(inLoop))
        t++;
      in2Loop++;
    }
    else
    {
      if (in->getData(inLoop))
        t = 1;
      else
        t++;
    }
    
    counter->append(t);
    inLoop++;
  }

  output->addLine(counter);

  return output->getLine(0);
}

PlotLine * UTIL::calculateCOMP (QString &p, QPtrList<PlotLine> &d)
{
  // format1: METHOD, ARRAY_INPUT, DOUBLE, OPERATOR
  // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2, OPERATOR

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 4)
    ;
  else
  {
    qDebug("UTIL::COMP: invalid parm count");
    return 0;
  }

  PlotLine *input = 0;
  PlotLine *input2 = 0;
  if (! d.count())
  {
    qDebug("UTIL::COMP: invalid ARRAY_INPUT parm");
    return 0;
  }
  else
    input = d.at(0);

  double inputNum = 0;
  if (d.count() == 1)
  {
    bool ok;
    double t = l[2].toDouble(&ok);
    if (ok)
      inputNum = t;
    else
    {
      qDebug("UTIL::COMP: invalid DOUBLE parm");
      return 0;
    }
  }
  else
    input2 = d.at(1);

  if (opList.findIndex(l[3]) == -1)
  {
    qDebug("UTIL::COMP: invalid METHOD parm:%s", l[3].latin1());
    return 0;
  }

  clearOutput();

  int loop = input->getSize() - 1;
  int loop2 = 0;
  if (input2)
    loop2 = input2->getSize() - 1;

  PlotLine *line = new PlotLine;
  
  Operator op = getOperator(l[3]);
  
  while (loop > -1 && loop < input->getSize())
  {
    if (input2)
    {
      if (loop2 < 0 || loop2 >= input2->getSize())
        break;
    }
    
    double t = 0;
    
    if (! input2)
      t = inputNum;
    else
      t = input2->getData(loop2);
      
    switch (op)
    {
      case Equal:
        if (input->getData(loop) == t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case LessThan:
        if (input->getData(loop) < t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case LessThanEqual:
        if (input->getData(loop) <= t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case GreaterThan:
        if (input->getData(loop) > t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case GreaterThanEqual:
        if (input->getData(loop) >= t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case And:
        if (input->getData(loop) && t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case Or:
        if (input->getData(loop) || t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      default:
        break;
    }
      
    loop--;
    
    if (input2)
      loop2--;
  }
  
  output->addLine(line);

  return output->getLine(0);
}

PlotLine * UTIL::calculateADD (QString &p, QPtrList<PlotLine> &d)
{
  // format1: METHOD, ARRAY_INPUT, DOUBLE
  // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 3)
    ;
  else
  {
    qDebug("UTIL::calculateADD::calculateCustom: invalid parm count");
    return 0;
  }

  PlotLine *input = 0;
  PlotLine *input2 = 0;
  if (! d.count())
  {
    qDebug("UTIL::calculateADD: invalid ARRAY_INPUT parm");
    return 0;
  }
  else
    input = d.at(0);

  if (d.count() == 2)
    input2 = d.at(1);

  double inputNum = 0;
  if (! input2)
  {
    bool ok;
    double t = l[2].toDouble(&ok);
    if (ok)
      inputNum = t;
    else
    {
      qDebug("UTIL::calculateADD: invalid DOUBLE parm");
      return 0;
    }
  }

  clearOutput();

  int loop = input->getSize() - 1;
  int loop2 = 0;
  if (input2)
    loop2 = input2->getSize() - 1;
    
  PlotLine *line = new PlotLine;
  
  while (loop > -1)
  {
    double v = input->getData(loop);
    
    if (input2)
    {
      if (loop2 < 0)
        break;
      v = v + input2->getData(loop2);
      loop2--;
    }
    else
      v = v + inputNum;
    
    line->prepend(v);
    loop--;
  }
  
  output->addLine(line);
  return output->getLine(0);
}

PlotLine * UTIL::calculateDIV (QString &p, QPtrList<PlotLine> &d)
{
  // format1: METHOD, ARRAY_INPUT, DOUBLE
  // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 3)
    ;
  else
  {
    qDebug("UTIL::calculateDIV::calculateCustom: invalid parm count");
    return 0;
  }

  PlotLine *input = 0;
  PlotLine *input2 = 0;
  if (! d.count())
  {
    qDebug("UTIL::calculateDIV: invalid ARRAY_INPUT parm");
    return 0;
  }
  else
    input = d.at(0);

  if (d.count() == 2)
    input2 = d.at(1);

  double inputNum = 0;
  if (! input2)
  {
    bool ok;
    double t = l[2].toDouble(&ok);
    if (ok)
      inputNum = t;
    else
    {
      qDebug("UTIL::calculateDIV: invalid DOUBLE parm");
      return 0;
    }
  }

  clearOutput();

  int loop = input->getSize() - 1;
  int loop2 = 0;
  if (input2)
    loop2 = input2->getSize() - 1;
    
  PlotLine *line = new PlotLine;
  
  while (loop > -1)
  {
    double v = input->getData(loop);
    
    if (input2)
    {
      if (loop2 < 0)
        break;

      v = v / input2->getData(loop2);
      loop2--;
    }
    else
      v = v / inputNum;
    
    line->prepend(v);
    loop--;
  }
  
  output->addLine(line);
  return output->getLine(0);
}

PlotLine * UTIL::calculateMUL (QString &p, QPtrList<PlotLine> &d)
{
  // format1: METHOD, ARRAY_INPUT, DOUBLE
  // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 3)
    ;
  else
  {
    qDebug("UTIL::calculateMUL::calculateCustom: invalid parm count");
    return 0;
  }

  PlotLine *input = 0;
  PlotLine *input2 = 0;
  if (! d.count())
  {
    qDebug("UTIL::calculateMUL: invalid ARRAY_INPUT parm");
    return 0;
  }
  else
    input = d.at(0);

  if (d.count() == 2)
    input2 = d.at(1);

  double inputNum = 0;
  if (! input2)
  {
    bool ok;
    double t = l[2].toDouble(&ok);
    if (ok)
      inputNum = t;
    else
    {
      qDebug("UTIL::calculateMUL: invalid DOUBLE parm");
      return 0;
    }
  }

  clearOutput();

  int loop = input->getSize() - 1;
  int loop2 = 0;
  if (input2)
    loop2 = input2->getSize() - 1;
    
  PlotLine *line = new PlotLine;
  
  while (loop > -1)
  {
    double v = input->getData(loop);
    
    if (input2)
    {
      if (loop2 < 0)
        break;

      v = v * input2->getData(loop2);
      loop2--;
    }
    else
      v = v * inputNum;
    
    line->prepend(v);
    loop--;
  }
  
  output->addLine(line);
  return output->getLine(0);
}

PlotLine * UTIL::calculateSUB (QString &p, QPtrList<PlotLine> &d)
{
  // format1: METHOD, ARRAY_INPUT, DOUBLE
  // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 3)
    ;
  else
  {
    qDebug("UTIL::calculateSUB::calculateCustom: invalid parm count");
    return 0;
  }

  PlotLine *input = 0;
  PlotLine *input2 = 0;
  if (! d.count())
  {
    qDebug("UTIL::calculateSUB: invalid ARRAY_INPUT parm");
    return 0;
  }
  else
    input = d.at(0);

  if (d.count() == 2)
    input2 = d.at(1);

  double inputNum = 0;
  if (! input2)
  {
    bool ok;
    double t = l[2].toDouble(&ok);
    if (ok)
      inputNum = t;
    else
    {
      qDebug("UTIL::calculateSUB: invalid DOUBLE parm");
      return 0;
    }
  }

  clearOutput();

  int loop = input->getSize() - 1;
  int loop2 = 0;
  if (input2)
    loop2 = input2->getSize() - 1;
    
  PlotLine *line = new PlotLine;
  
  while (loop > -1)
  {
    double v = input->getData(loop);
    
    if (input2)
    {
      if (loop2 < 0)
        break;

      v = v - input2->getData(loop2);
      loop2--;
    }
    else
      v = v - inputNum;
    
    line->prepend(v);
    loop--;
  }
  
  output->addLine(line);
  return output->getLine(0);
}

PlotLine * UTIL::calculateREF (QString &p, QPtrList<PlotLine> &d)
{
  // format1: METHOD, ARRAY_INPUT, PERIOD

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 3)
    ;
  else
  {
    qDebug("REF::calculateCustom: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("REF::calculateCustom: no input");
    return 0;
  }

  bool ok;
  int period;
  int t = l[2].toInt(&ok);
  if (ok)
    period = t;
  else
  {
    qDebug("REF::calculateCustom: invalid PERIOD parm");
    return 0;
  }

  clearOutput();

  PlotLine *line = new PlotLine;
  PlotLine *in = d.at(0);
  
  int loop = 0;
  for (loop = 0; loop < in->getSize(); loop++)
  {
    if (loop - period < 0)
      continue;
      
    line->append(in->getData(loop - period));
  }
  
  output->addLine(line);
  
  return output->getLine(0);
}

PlotLine * UTIL::calculatePER (QString &p, QPtrList<PlotLine> &d)
{
  // format: METHOD, INPUT_ARRAY

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 2)
    ;
  else
  {
    qDebug("UTIL::calculatePER: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("UTIL::calculatePER: invalid ARRAY_INPUT parm");
    return 0;
  }

  clearOutput();

  PlotLine *line = new PlotLine();
  PlotLine *input = d.at(0);
  
  double base = input->getData(0);
  int loop;
  for (loop = 1; loop < (int) input->getSize(); loop++)
    line->append(((input->getData(loop) - base) / base) * 100);

  output->addLine(line);
  return output->getLine(0);
}

PlotLine * UTIL::calculateCOLOR (QString &p, QPtrList<PlotLine> &d)
{
  // format1: METHOD, INPUT_ARRAY, COLOR_ARRAY, VALUE, COLOR

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 5)
    ;
  else
  {
    qDebug("UTIL::calculateCOLOR: invalid parm count");
    return 0;
  }

  if (d.count() != 2)
  {
    qDebug("UTIL::calculateCOLOR: 2 input arrays needed");
    return 0;
  }

  bool ok;
  int value;
  int t = l[3].toInt(&ok);
  if (ok)
    value = t;
  else
  {
    qDebug("UTIL::calculateCustom: invalid VALUE parm");
    return 0;
  }

  QColor c(l[4]);
  if (! c.isValid())
  {
    qDebug("UTIL::calculateCustom: invalid COLOR parm");
    return 0;
  }
  
  clearOutput();

  PlotLine *inbool = d.at(0);
  int inboolLoop = inbool->getSize() - 1;
  PlotLine *incol = d.at(1);
  int incolLoop = incol->getSize() - 1;
  while (inboolLoop > -1 && incolLoop > -1)
  {
    if (inbool->getData(inboolLoop) == value)
      incol->setColorBar(incolLoop, c);

    inboolLoop--;
    incolLoop--;
  }

  PlotLine *line = new PlotLine;
  output->addLine(line);
  return output->getLine(0);
}

//******************************************************************
//******************************************************************
//******************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  UTIL *o = new UTIL;
  return ((IndicatorPlugin *) o);
}



