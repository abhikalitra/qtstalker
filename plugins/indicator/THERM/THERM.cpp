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

/* The "Market Thermometer" indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

#include "THERM.h"
#include <math.h>

THERM::THERM ()
{
  pluginName = "THERM";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color Below MA"), "green", Setting::Color);
  set(tr("Color Above MA"), "magenta", Setting::Color);
  set(tr("Color Threshold"), "red", Setting::Color);
  set(tr("Threshold"), "3", Setting::Float);
  set(tr("MA Color"), "yellow", Setting::Color);
  set(tr("Line Type"), tr("Histogram Bar"), Setting::LineType);
  set(tr("MA Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("MA Label"), tr("THERM MA"), Setting::Text);
  set(tr("Smoothing"), "2", Setting::Integer);
  set(tr("Smoothing Type"), tr("EMA"), Setting::MAType);
  set(tr("MA Period"), "22", Setting::Integer);
  set(tr("MA Type"), tr("EMA"), Setting::MAType);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Market Thermometer\n";
}

THERM::~THERM ()
{
}

void THERM::calculate ()
{
  PlotLine *therm = new PlotLine();

  int loop;
  double thermometer = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    Setting *r = data->at(loop);
    Setting *pr = data->at(loop - 1);
    double high = fabs(r->getFloat("High") - pr->getFloat("High"));
    double lo = fabs(pr->getFloat("Low") - r->getFloat("Low"));
    
    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    therm->append(thermometer);
  }

  if (getInt(tr("Smoothing")) > 1)
  {
    PlotLine *ma = getMA(therm, getData(tr("Smoothing Type")), getInt(tr("Smoothing")));
    output.append(ma);
    delete therm;
    therm = ma;
  }
  else
    output.append(therm);

  PlotLine *therm_ma = getMA(therm, getData(tr("MA Type")), getInt(tr("MA Period")));
  therm_ma->setColor(getData(tr("MA Color")));
  therm_ma->setType(getData(tr("MA Line Type")));
  therm_ma->setLabel(getData(tr("MA Label")));
  output.append(therm_ma);

  // assign the therm colors

  therm->setColorFlag(TRUE);
  therm->setType(getData(tr("Line Type")));
  therm->setLabel(getData(tr("Label")));

  int thermLoop = therm->getSize() - 1;
  int maLoop = therm_ma->getSize() - 1;
  double threshold = getFloat(tr("Threshold"));
  while (thermLoop > -1)
  {
    if (maLoop > -1)
    {
      double thrm = therm->getData(thermLoop);
      double thrmma = therm_ma->getData(maLoop);

      if (thrm > (thrmma * threshold))
        therm->prependColorBar(getData(tr("Color Threshold")));
      else
      {
        if (thrm > thrmma)
          therm->prependColorBar(getData(tr("Color Above MA")));
        else
          therm->prependColorBar(getData(tr("Color Below MA")));
      }
    }
    else
      therm->prependColorBar(getData(tr("Color Below MA")));

    thermLoop--;
    maLoop--;
  }
}

Plugin * create ()
{
  THERM *o = new THERM;
  return ((Plugin *) o);
}

/* Alert Notes

1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days

*/


