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

#include "UO.h"

UO::UO ()
{
  pluginName = "UO";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "red", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Short Period"), "7", Setting::Integer);
  set(tr("Medium Period"), "14", Setting::Integer);
  set(tr("Long Period"), "28", Setting::Integer);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Ultimate Oscillator\n";
}

UO::~UO ()
{
}

void UO::calculate ()
{
  int shortPeriod = getInt(tr("Short Period"));

  int medPeriod = getInt(tr("Medium Period"));

  int longPeriod = getInt(tr("Long Period"));

  PlotLine *trg = getTR();

  PlotLine *atr = getSMA(trg, shortPeriod);
  int atrLoop = atr->getSize() - 1;

  PlotLine *atr2 = getSMA(trg, medPeriod);
  int atr2Loop = atr2->getSize() - 1;

  PlotLine *atr3 = getSMA(trg, longPeriod);
  int atr3Loop = atr3->getSize() - 1;

  PlotLine *f = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data.count(); loop++)
  {
    Setting *set = data.at(loop);
    f->append(set->getFloat("Close") - set->getFloat("Low"));
  }

  PlotLine *sma = getSMA(f, shortPeriod);
  int smaLoop = sma->getSize() - 1;

  PlotLine *sma2 = getSMA(f, medPeriod);
  int sma2Loop = sma2->getSize() - 1;

  PlotLine *sma3 = getSMA(f, longPeriod);
  int sma3Loop = sma3->getSize() - 1;

  PlotLine *uo = new PlotLine();

  while (smaLoop > -1 && sma2Loop > -1 && sma3Loop > -1 && atrLoop > -1 && atr2Loop > -1 && atr3Loop > -1)
  {
    double t = (sma->getData(smaLoop) / atr->getData(atrLoop)) * 4;
    t = t + ((sma2->getData(sma2Loop) / atr2->getData(atr2Loop)) * 2);
    t = t + (sma3->getData(sma3Loop) / atr3->getData(atr3Loop));
    t = (t / 7) * 100;

    uo->prepend(t);

    smaLoop--;
    sma2Loop--;
    sma3Loop--;
    atrLoop--;
    atr2Loop--;
    atr3Loop--;
  }

  uo->setColor(getData(tr("Color")));
  uo->setType(getData(tr("Line Type")));
  uo->setLabel(getData(tr("Label")));
  output.append(uo);

  delete trg;
  delete atr;
  delete atr2;
  delete atr3;
  delete sma;
  delete sma2;
  delete sma3;
  delete f;
}

Plugin * create ()
{
  UO *o = new UO;
  return ((Plugin *) o);
}

