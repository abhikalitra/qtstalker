/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "WILLR.h"

WILLR::WILLR ()
{
  pluginName = "WILLR";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "10", Setting::Integer);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Williams Percent R\n";
}

WILLR::~WILLR ()
{
}

void WILLR::calculate ()
{
  int period = getInt(tr("Period"));

  PlotLine *willr = new PlotLine();

  int loop;
  for (loop = period; loop < (int) data.count(); loop++)
  {
    int loop2;
    double lw;
    double hg;
    for (loop2 = 0, lw = 9999999, hg = 0; loop2 < period; loop2++)
    {
      Setting *set = data.at(loop - loop2);
      double high = set->getFloat("High");
      double low = set->getFloat("Low");

      if (high > hg)
        hg = high;

      if (low < lw)
        lw = low;
    }

    Setting *set = data.at(loop);
    double t = ((hg - set->getFloat("Close")) / (hg - lw)) * 100;
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    willr->append(t);
  }

  willr->setColor(getData(tr("Color")));
  willr->setType(getData(tr("Line Type")));
  willr->setLabel(getData(tr("Label")));
  output.append(willr);
}

Plugin * create ()
{
  WILLR *o = new WILLR;
  return ((Plugin *) o);
}




