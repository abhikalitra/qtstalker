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

#include "VOLR.h"

VOLR::VOLR ()
{
  pluginName = "VOLR";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Period"), "14", Setting::Integer);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("True"), Setting::None);

  about = "Volatility Ratio\n";
}

VOLR::~VOLR ()
{
}

void VOLR::calculate ()
{
  int period = getInt(tr("Period"));

  PlotLine *trg = getTR();
  int trgLoop = trg->getSize() - 1;

  PlotLine *ma = getEMA(trg, period);
  int maLoop = ma->getSize() - 1;

  PlotLine *volr = new PlotLine;

  while (maLoop > -1 && trgLoop > -1)
  {
    volr->prepend(trg->getData(trgLoop) / ma->getData(maLoop));
    maLoop--;
    trgLoop--;
  }

  volr->setColor(getData(tr("Color")));
  volr->setType(getData(tr("Line Type")));
  volr->setLabel(getData(tr("Label")));
  output.append(volr);

  delete trg;
  delete ma;
}

QMemArray<int> VOLR::getAlerts ()
{
  alerts.fill(0, data->count());

  if (! output.count())
    return alerts;

  PlotLine *line = output.at(0);

  int lineLoop;
  int listLoop = data->count() - line->getSize();
  for (lineLoop = 0; lineLoop < line->getSize(); lineLoop++, listLoop++)
  {
    if (line->getData(lineLoop) > 2)
      alerts[listLoop] = 1;
  }

  return alerts;
}

Plugin * create ()
{
  VOLR *o = new VOLR;
  return ((Plugin *) o);
}

