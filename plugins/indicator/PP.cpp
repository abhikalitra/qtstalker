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

#include "PP.h"

PP::PP ()
{
  pluginName = "PP";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color Resistance"), "red", Setting::Color);
  set(tr("Color Support"), "red", Setting::Color);
  set(tr("Line Type Resistance"), tr("Horizontal"), Setting::None);
  set(tr("Line Type Support"), tr("Horizontal"), Setting::None);
  set(tr("Label First Support"), tr("PP FS"), Setting::Text);
  set(tr("Label Second Support"), tr("PP SS"), Setting::Text);
  set(tr("Label Third Support"), tr("PP TS"), Setting::Text);
  set(tr("Label First Resistance"), tr("PP FR"), Setting::Text);
  set(tr("Label Second Resistance"), tr("PP SR"), Setting::Text);
  set(tr("Label Third Resistance"), tr("PP TR"), Setting::Text);
  set(tr("Plot"), tr("True"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Pivot Point\n";
}

PP::~PP ()
{
}

void PP::calculate ()
{
  Setting *set = data->at(data->count() - 1);
  double high = set->getFloat("High");
  double low = set->getFloat("Low");
  double close = set->getFloat("Close");
  double pp = (high + low + close) / 3;

  PlotLine *fr = new PlotLine();
  fr->setColor(getData(tr("Color Resistance")));
  fr->setType(getData(tr("Line Type Resistance")));
  fr->setLabel(getData(tr("Label First Resistance")));
  fr->append((2 * pp) - low);

  PlotLine *sr = new PlotLine();
  sr->setColor(getData(tr("Color Resistance")));
  sr->setType(getData(tr("Line Type Resistance")));
  sr->setLabel(getData(tr("Label Second Resistance")));
  sr->append(pp + (high - low));

  PlotLine *thr = new PlotLine();
  thr->setColor(getData(tr("Color Resistance")));
  thr->setType(getData(tr("Line Type Resistance")));
  thr->setLabel(getData(tr("Label Third Resistance")));
  thr->append((2 * pp) + (high - (2 * low)));

  PlotLine *fs = new PlotLine();
  fs->setColor(getData(tr("Color Support")));
  fs->setType(getData(tr("Line Type Support")));
  fs->setLabel(getData(tr("Label First Support")));
  fs->append((2 * pp) - high);

  PlotLine *ss = new PlotLine();
  ss->setColor(getData(tr("Color Support")));
  ss->setType(getData(tr("Line Type Support")));
  ss->setLabel(getData(tr("Label Second Support")));
  ss->append(pp - (high - low));

  PlotLine *ts = new PlotLine();
  ts->setColor(getData(tr("Color Support")));
  ts->setType(getData(tr("Line Type Support")));
  ts->setLabel(getData(tr("Label Second Support")));
  ts->append((2 * pp) - ((2 * high) - low));

  output.append(ts);
  output.append(ss);
  output.append(fs);
  output.append(fr);
  output.append(sr);
  output.append(thr);
}

Plugin * create ()
{
  PP *o = new PP;
  return ((Plugin *) o);
}


