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

#include "WAD.h"

WAD::WAD ()
{
  pluginName = "WAD";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Williams Accumulation Distribution\n";
}

WAD::~WAD ()
{
}

void WAD::calculate ()
{
  PlotLine *wad = new PlotLine();

  int loop;
  double accum = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    Setting *set = data->at(loop);
    double high = set->getFloat("High");
    double low = set->getFloat("Low");
    double close = set->getFloat("Close");
    set = data->at(loop - 1);
    double yclose = set->getFloat("Close");

    double h = high;
    if (yclose > h)
      h = yclose;

    double l = low;
    if (yclose < l)
      l = yclose;

    if (close > yclose)
      accum = accum + (close - l);
    else
    {
      if (yclose == close)
        ;
      else
        accum = accum - (h - close);
    }

    wad->append(accum);
  }

  wad->setColor(getData(tr("Color")));
  wad->setType(getData(tr("Line Type")));
  wad->setLabel(getData(tr("Label")));
  output.append(wad);
}

Plugin * create ()
{
  WAD *o = new WAD;
  return ((Plugin *) o);
}



