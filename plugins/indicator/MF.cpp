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

#include "MF.h"

MF::MF ()
{
  pluginName = "MF";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "10", Setting::Integer);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Money Flow\n";
}

MF::~MF ()
{
}

void MF::calculate ()
{
  SettingItem *set = getItem(tr("Period"));
  int period = set->data.toInt();

  Output *d = new Output();

  int loop;
  for (loop = period; loop < (int) data.count(); loop++)
  {
    double pmf = 0;
    double nmf = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
    {
      Setting *set = data.at(loop - loop2);
      double high = set->getFloat("High");
      double low = set->getFloat("Low");
      double close = set->getFloat("Close");
      double volume = set->getFloat("Volume");

      double ttp = (high + low + close) / 3;
      double mf = ttp * volume;

      set = data.at(loop - loop2 - 1);
      high = set->getFloat("High");
      low = set->getFloat("Low");
      close = set->getFloat("Close");

      double ytp = (high + low + close) / 3;

      if (ttp > ytp)
        pmf = pmf + mf;

      if (ytp > ttp)
        nmf = nmf + mf;
    }

    double mfi = 100 - (100 / (1 + (pmf / nmf)));

    if (mfi > 100)
      mfi = 100;
    if (mfi < 0)
      mfi = 0;

    d->append(mfi);
  }

  output.append(d);
}

Plugin * create ()
{
  MF *o = new MF;
  return ((Plugin *) o);
}






