/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "VOL.h"
#include "Globals.h"
#include "VOLDialog.h"

#include <QtDebug>

VOL::VOL ()
{
  _type = _INDICATOR;
  _plugin = "VOL";
}

int VOL::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  QColor upColor(settings->data("COLOR_UP"));
  QColor downColor(settings->data("COLOR_DOWN"));
  QColor neutralColor(settings->data("COLOR_NEUTRAL"));

  Curve *line = new Curve(Curve::HistogramBar);
  line->setZ(settings->getInt("Z"));

  int loop = 0;
  for (; loop < bd->count(); loop++)
  {
    Bar *bar = bd->bar(loop);
    if (! bar)
      continue;

    CurveBar *b = new CurveBar;
    b->setData(bar->volume());

    Bar *ybar = bd->bar(loop - 1);
    if (ybar)
    {
      if (bar->close() > ybar->close())
        b->setColor(upColor);
      else
      {
        if (bar->close() < ybar->close())
          b->setColor(downColor);
      }
    }

    line->setBar(loop, b);
  }

  line->setLabel("VOL");
  i->setLine(settings->data("VOL"), line);
  
  return 0;
}

QWidget * VOL::dialog (QWidget *p, Setting *set)
{
  return new VOLDialog(p, set);
}

void VOL::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR_UP", QString("green"));
  set->setData("COLOR_DOWN", QString("red"));
  set->setData("COLOR_NEUTRAL", QString("dimgray"));
  set->setData("Z", 0);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  VOL *o = new VOL;
  return ((Plugin *) o);
}
