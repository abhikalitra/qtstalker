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

#include "MMA.h"

MMA::MMA ()
{
  pluginName = "MMA";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Fast Color"), "red", Setting::Color);
  set(tr("Fast Line Type"), tr("Line"), Setting::LineType);
  set(tr("Fast Label 1"), tr("MMAF1"), Setting::Text);
  set(tr("Fast Label 2"), tr("MMAF2"), Setting::Text);
  set(tr("Fast Label 3"), tr("MMAF3"), Setting::Text);
  set(tr("Fast Label 4"), tr("MMAF4"), Setting::Text);
  set(tr("Fast Label 5"), tr("MMAF5"), Setting::Text);
  set(tr("Fast Label 6"), tr("MMAF6"), Setting::Text);
  set(tr("Fast Period 1"), "3", Setting::Integer);
  set(tr("Fast Period 2"), "6", Setting::Integer);
  set(tr("Fast Period 3"), "8", Setting::Integer);
  set(tr("Fast Period 4"), "10", Setting::Integer);
  set(tr("Fast Period 5"), "12", Setting::Integer);
  set(tr("Fast Period 6"), "15", Setting::Integer);
  set(tr("Fast Displace 1"), "0", Setting::Integer);
  set(tr("Fast Displace 2"), "0", Setting::Integer);
  set(tr("Fast Displace 3"), "0", Setting::Integer);
  set(tr("Fast Displace 4"), "0", Setting::Integer);
  set(tr("Fast Displace 5"), "0", Setting::Integer);
  set(tr("Fast Displace 6"), "0", Setting::Integer);
  set(tr("Fast Type"), "EMA", Setting::MAType);
  set(tr("Fast Input"), tr("Close"), Setting::InputField);
  set(tr("Slow Color"), "yellow", Setting::Color);
  set(tr("Slow Line Type"), tr("Line"), Setting::LineType);
  set(tr("Slow Label 1"), tr("MMAS1"), Setting::Text);
  set(tr("Slow Label 2"), tr("MMAS2"), Setting::Text);
  set(tr("Slow Label 3"), tr("MMAS3"), Setting::Text);
  set(tr("Slow Label 4"), tr("MMAS4"), Setting::Text);
  set(tr("Slow Label 5"), tr("MMAS5"), Setting::Text);
  set(tr("Slow Label 6"), tr("MMAS6"), Setting::Text);
  set(tr("Slow Period 1"), "30", Setting::Integer);
  set(tr("Slow Period 2"), "35", Setting::Integer);
  set(tr("Slow Period 3"), "40", Setting::Integer);
  set(tr("Slow Period 4"), "45", Setting::Integer);
  set(tr("Slow Period 5"), "50", Setting::Integer);
  set(tr("Slow Period 6"), "60", Setting::Integer);
  set(tr("Slow Displace 1"), "0", Setting::Integer);
  set(tr("Slow Displace 2"), "0", Setting::Integer);
  set(tr("Slow Displace 3"), "0", Setting::Integer);
  set(tr("Slow Displace 4"), "0", Setting::Integer);
  set(tr("Slow Displace 5"), "0", Setting::Integer);
  set(tr("Slow Displace 6"), "0", Setting::Integer);
  set(tr("Slow Type"), "EMA", Setting::MAType);
  set(tr("Slow Input"), tr("Close"), Setting::InputField);
  set(tr("Long-term Color"), "blue", Setting::Color);
  set(tr("Long-term Line Type"), tr("Line"), Setting::LineType);
  set(tr("Long-term Label"), tr("MMAL"), Setting::Text);
  set(tr("Long-term Period"), "150", Setting::Integer);
  set(tr("Long-term Displace"), "0", Setting::Integer);
  set(tr("Long-term Type"), "SMA", Setting::MAType);
  set(tr("Long-term Input"), tr("Close"), Setting::InputField);
  set(tr("Plot"), tr("False"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Multiple Moving Averages\n";
}

MMA::~MMA ()
{
}

void MMA::calculate ()
{
  PlotLine *fin = getInput(getData(tr("Fast Input")));
  PlotLine *sin = getInput(getData(tr("Slow Input")));
  PlotLine *lin = getInput(getData(tr("Long-term Input")));

  PlotLine *fma1 = getMA(fin, getData(tr("Fast Type")), getInt(tr("Fast Period 1")), getInt(tr("Fast Displace 1")));
  fma1->setColor(getData(tr("Fast Color")));
  fma1->setType(getData(tr("Fast Line Type")));
  fma1->setLabel(getData(tr("Fast Label 1")));
  PlotLine *fma2 = getMA(fin, getData(tr("Fast Type")), getInt(tr("Fast Period 2")), getInt(tr("Fast Displace 2")));
  fma2->setColor(getData(tr("Fast Color")));
  fma2->setType(getData(tr("Fast Line Type")));
  fma2->setLabel(getData(tr("Fast Label 2")));
  PlotLine *fma3 = getMA(fin, getData(tr("Fast Type")), getInt(tr("Fast Period 3")), getInt(tr("Fast Displace 3")));
  fma3->setColor(getData(tr("Fast Color")));
  fma3->setType(getData(tr("Fast Line Type")));
  fma3->setLabel(getData(tr("Fast Label 3")));
  PlotLine *fma4 = getMA(fin, getData(tr("Fast Type")), getInt(tr("Fast Period 4")), getInt(tr("Fast Displace 4")));
  fma4->setColor(getData(tr("Fast Color")));
  fma4->setType(getData(tr("Fast Line Type")));
  fma4->setLabel(getData(tr("Fast Label 4")));
  PlotLine *fma5 = getMA(fin, getData(tr("Fast Type")), getInt(tr("Fast Period 5")), getInt(tr("Fast Displace 5")));
  fma5->setColor(getData(tr("Fast Color")));
  fma5->setType(getData(tr("Fast Line Type")));
  fma5->setLabel(getData(tr("Fast Label 5")));
  PlotLine *fma6 = getMA(fin, getData(tr("Fast Type")), getInt(tr("Fast Period 6")), getInt(tr("Fast Displace 6")));
  fma6->setColor(getData(tr("Fast Color")));
  fma6->setType(getData(tr("Fast Line Type")));
  fma6->setLabel(getData(tr("Fast Label 6")));

  PlotLine *sma1 = getMA(sin, getData(tr("Slow Type")), getInt(tr("Slow Period 1")), getInt(tr("Slow Displace 1")));
  sma1->setColor(getData(tr("Slow Color")));
  sma1->setType(getData(tr("Slow Line Type")));
  sma1->setLabel(getData(tr("Slow Label 1")));
  PlotLine *sma2 = getMA(sin, getData(tr("Slow Type")), getInt(tr("Slow Period 2")), getInt(tr("Slow Displace 2")));
  sma2->setColor(getData(tr("Slow Color")));
  sma2->setType(getData(tr("Slow Line Type")));
  sma2->setLabel(getData(tr("Slow Label 2")));
  PlotLine *sma3 = getMA(sin, getData(tr("Slow Type")), getInt(tr("Slow Period 3")), getInt(tr("Slow Displace 3")));
  sma3->setColor(getData(tr("Slow Color")));
  sma3->setType(getData(tr("Slow Line Type")));
  sma3->setLabel(getData(tr("Slow Label 3")));
  PlotLine *sma4 = getMA(sin, getData(tr("Slow Type")), getInt(tr("Slow Period 4")), getInt(tr("Slow Displace 4")));
  sma4->setColor(getData(tr("Slow Color")));
  sma4->setType(getData(tr("Slow Line Type")));
  sma4->setLabel(getData(tr("Slow Label 4")));
  PlotLine *sma5 = getMA(sin, getData(tr("Slow Type")), getInt(tr("Slow Period 5")), getInt(tr("Slow Displace 5")));
  sma5->setColor(getData(tr("Slow Color")));
  sma5->setType(getData(tr("Slow Line Type")));
  sma5->setLabel(getData(tr("Slow Label 5")));
  PlotLine *sma6 = getMA(sin, getData(tr("Slow Type")), getInt(tr("Slow Period 6")), getInt(tr("Slow Displace 6")));
  sma6->setColor(getData(tr("Slow Color")));
  sma6->setType(getData(tr("Slow Line Type")));
  sma6->setLabel(getData(tr("Slow Label 6")));

  PlotLine *lma = getMA(lin, getData(tr("Long-term Type")), getInt(tr("Long-term Period")), getInt(tr("Long-term Displace")));
  lma->setColor(getData(tr("Long-term Color")));
  lma->setType(getData(tr("Long-term Line Type")));
  lma->setLabel(getData(tr("Long-term Label")));

  delete fin;
  delete sin;
  delete lin;

  if (fma1->getSize())
    output.append(fma1);
  else
    delete fma1;

  if (fma2->getSize())
    output.append(fma2);
  else
    delete fma2;

  if (fma3->getSize())
    output.append(fma3);
  else
    delete fma3;

  if (fma4->getSize())
    output.append(fma4);
  else
    delete fma4;

  if (fma5->getSize())
    output.append(fma5);
  else
    delete fma5;

  if (fma6->getSize())
    output.append(fma6);
  else
    delete fma6;

  if (sma1->getSize())
    output.append(sma1);
  else
    delete sma1;

  if (sma2->getSize())
    output.append(sma2);
  else
    delete sma2;

  if (sma3->getSize())
    output.append(sma3);
  else
    delete sma3;

  if (sma4->getSize())
    output.append(sma4);
  else
    delete sma4;

  if (sma5->getSize())
    output.append(sma5);
  else
    delete sma5;

  if (sma6->getSize())
    output.append(sma6);
  else
    delete sma6;

  if (lma->getSize())
    output.append(lma);
  else
    delete lma;
}

Plugin * create ()
{
  MMA *o = new MMA;
  return ((Plugin *) o);
}


