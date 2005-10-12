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

#include "MATH.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

MATH::MATH ()
{
  pluginName = "MATH";
  
  methodList.append("ADD");
  methodList.append("DIV");
  methodList.append("MUL");
  methodList.append("SUB");

  helpFile = "math.html";
}

MATH::~MATH ()
{
}

PlotLine * MATH::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, DOUBLE, METHOD
  // format2: ARRAY_INPUT, ARRAY_INPUT2, METHOD

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 3)
    ;
  else
  {
    qDebug("MATH::calculateCustom: invalid parm count");
    return 0;
  }

  PlotLine *input = 0;
  PlotLine *input2 = 0;
  if (! d.count())
  {
    qDebug("MATH::calculateCustom: invalid ARRAY_INPUT parm");
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
    double t = l[1].toDouble(&ok);
    if (ok)
      inputNum = t;
    else
    {
      qDebug("MATH::calculateCustom: invalid DOUBLE parm");
      return 0;
    }
  }

  QString method;
  if (methodList.findIndex(l[2]) == -1)
  {
    qDebug("MATH::calculateCustom: invalid METHOD parm");
    return 0;
  }
  else
    method = l[2];

  clearOutput();

  int loop = input->getSize() - 1;
  int loop2 = 0;
  if (input2)
    loop2 = input2->getSize() - 1;
    
  PlotLine *line = new PlotLine;
  
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
	  v = v + inputNum;
	  break;
        case 1:
	  if (inputNum)
	    v = v / inputNum;
	  break;
        case 2:
	  if (inputNum)
	    v = v * inputNum;
	  break;
        case 3:
	  v = v - inputNum;
	  break;
        default:
          break;
      }
    }
    
    line->prepend(v);
    loop--;
  }
  
  output->addLine(line);

  return output->getLine(0);
}

//******************************************************************
//******************************************************************
//******************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MATH *o = new MATH;
  return ((IndicatorPlugin *) o);
}



