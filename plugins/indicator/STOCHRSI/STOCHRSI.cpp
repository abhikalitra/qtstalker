/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "STOCHRSI.h"
#include <math.h>

STOCHRSI::STOCHRSI ()
{
  pluginName = "STOCHRSI";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "14", Setting::Integer);
  set(tr("Input"), tr("Close"), Setting::InputField);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Stochastic Relative Strength Index\n";
}

STOCHRSI::~STOCHRSI ()
{
}

void STOCHRSI::calculate ()
{
  int period = getInt(tr("Period"));

  PlotLine *in = getInput(getData(tr("Input")));

  PlotLine *rsi = getRSI(in, period);

  PlotLine *data = new PlotLine();

  int loop;
  for (loop = period - 1; loop < (int) rsi->getSize(); loop++)
  {
    int loop2;
    double l;
    double h;
    for (loop2 = 0, l = 9999999, h = 0; loop2 < period; loop2++)
    {
      double t = rsi->getData(loop - loop2);

      if (t > h)
        h = t;

      if (t < l)
        l = t;
    }

    double t = ((rsi->getData(loop) - l) / (h - l));

    if (t > 1)
      t = 1;
    if (t < 0)
      t = 0;

    data->append(t);
  }

  data->setColor(getData(tr("Color")));
  data->setType(getData(tr("Line Type")));
  data->setLabel(getData(tr("Label")));
  output.append(data);

  delete in;
  delete rsi;
}

PlotLine * STOCHRSI::getRSI (PlotLine *in, int period)
{
  PlotLine *rsi = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
  {
    double loss = 0;
    double gain = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
    {
      double t = in->getData(loop - loop2) - in->getData(loop - loop2 - 1);
      if (t > 0)
        gain = gain + t;
      if (t < 0)
        loss = loss + fabs(t);
    }

    double again = gain / period;
    double aloss = loss / period;
    double rs = again / aloss;
    double t = 100 - (100 / (1 + rs));
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    rsi->append(t);
  }

  return rsi;
}

Plugin * create ()
{
  STOCHRSI *o = new STOCHRSI;
  return ((Plugin *) o);
}





