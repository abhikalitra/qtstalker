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

#include "REF.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

REF::REF ()
{
  pluginName = "REF";
  helpFile = "ref.html";
}

REF::~REF ()
{
}

PlotLine * REF::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, PERIOD

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 2)
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
  int t = l[1].toInt(&ok);
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

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  REF *o = new REF;
  return ((IndicatorPlugin *) o);
}



