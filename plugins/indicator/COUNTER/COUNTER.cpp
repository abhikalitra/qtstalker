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
}

COUNTER::~COUNTER ()
{
}

PlotLine * COUNTER::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT
  // format2: ARRAY_INPUT, ARRAY_INPUT2

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 1 || l.count() == 2)
    ;
  else
  {
    qDebug("COUNTER::calculateCustom: invalid parm count");
    return 0;
  }

  PlotLine *in = 0;
  PlotLine *in2 = 0;
  if (! d.count())
  {
    qDebug("COUNTER::calculateCustom: invalid ARRAY_INPUT parm");
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

//***************************************************************
//***************************************************************
//***************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  COUNTER *o = new COUNTER;
  return ((IndicatorPlugin *) o);
}





