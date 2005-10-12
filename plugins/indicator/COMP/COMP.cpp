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
  helpFile = "comp.html";
}

COMP::~COMP ()
{
}

PlotLine * COMP::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, DOUBLE, METHOD
  // format2: ARRAY_INPUT, ARRAY_INPUT2, METHOD

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 3)
    ;
  else
  {
    qDebug("COMP::calculateCustom: invalid parm count");
    return 0;
  }

  PlotLine *input = 0;
  PlotLine *input2 = 0;
  if (! d.count())
  {
    qDebug("COMP::calculateCustom: invalid ARRAY_INPUT parm");
    return 0;
  }
  else
    input = d.at(0);

  double inputNum = 0;
  if (d.count() < 2)
  {
    bool ok;
    double t = l[1].toDouble(&ok);
    if (ok)
      inputNum = t;
    else
    {
      qDebug("COMP::calculateCustom: invalid DOUBLE parm");
      return 0;
    }
  }
  else
    input2 = d.at(1);

  if (opList.findIndex(l[2]) == -1)
  {
    qDebug("COMP::calculateCustom: invalid METHOD parm");
    return 0;
  }

  clearOutput();

  int loop = input->getSize() - 1;
  int loop2 = 0;
  if (input2)
    loop2 = input2->getSize() - 1;

  PlotLine *line = new PlotLine;
  
  Operator op = getOperator(l[2]);
  
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

//***************************************************************
//***************************************************************
//***************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  COMP *o = new COMP;
  return ((IndicatorPlugin *) o);
}



