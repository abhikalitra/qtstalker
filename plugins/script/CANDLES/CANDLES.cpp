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

#include "CANDLES.h"
#include "Curve.h"
#include "Globals.h"

#include <QtDebug>

CANDLES::CANDLES ()
{
  _plugin = "CANDLES";
}

int CANDLES::command (Command *command)
{
  // PARMS:
  // INPUT_OPEN
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // NAME
  // COLOR_UP
  // COLOR_DOWN
  // COLOR_NEUTRAL

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *iopen = i->line(command->parm("INPUT_OPEN"));
  if (! iopen)
  {
    qDebug() << _plugin << "::command: invalid INPUT_OPEN" << command->parm("INPUT_OPEN");
    return 1;
  }

  Curve *ihigh = i->line(command->parm("INPUT_HIGH"));
  if (! ihigh)
  {
    qDebug() << _plugin << "::command: invalid INPUT_HIGH" << command->parm("INPUT_HIGH");
    return 1;
  }

  Curve *ilow = i->line(command->parm("INPUT_LOW"));
  if (! ilow)
  {
    qDebug() << _plugin << "::command: invalid INPUT_LOW" << command->parm("INPUT_LOW");
    return 1;
  }

  Curve *iclose = i->line(command->parm("INPUT_CLOSE"));
  if (! iclose)
  {
    qDebug() << _plugin << "::command: invalid INPUT_CLOSE" << command->parm("INPUT_CLOSE");
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  QColor upColor(Qt::green);
  QString s = command->parm("COLOR_UP");
  if (! s.isEmpty())
  {
    upColor.setNamedColor(s);
    if (! upColor.isValid())
    {
      qDebug() << _plugin << "::command: invalid COLOR_UP" << command->parm("COLOR_UP");
      return 1;
    }
  }

  QColor downColor(Qt::red);
  s = command->parm("COLOR_DOWN");
  if (! s.isEmpty())
  {
    downColor.setNamedColor(s);
    if (! downColor.isValid())
    {
      qDebug() << _plugin << "::command: invalid COLOR_DOWN" << command->parm("COLOR_DOWN");
      return 1;
    }
  }

  QColor neutralColor(Qt::blue);
  s = command->parm("COLOR_NEUTRAL");
  if (! s.isEmpty())
  {
    neutralColor.setNamedColor(s);
    if (! neutralColor.isValid())
    {
      qDebug() << _plugin << "::command: invalid COLOR_NEUTRAL" << command->parm("COLOR_NEUTRAL");
      return 1;
    }
  }

  line = new Curve(Curve::Candle);

  int ipos = 0;
  int end = 0;
  iclose->keyRange(ipos, end);
  for (; ipos <= end; ipos++)
  {
    CurveBar *obar = iopen->bar(ipos);
    if (! obar)
      continue;

    CurveBar *hbar = ihigh->bar(ipos);
    if (! hbar)
      continue;

    CurveBar *lbar = ilow->bar(ipos);
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(ipos);
    if (! cbar)
      continue;

    CurveBar *bar = new CurveBar;
    bar->setData(0, obar->data());
    bar->setData(1, hbar->data());
    bar->setData(2, lbar->data());
    bar->setData(3, cbar->data());
    bar->setColor(neutralColor);

    CurveBar *ycbar = iclose->bar(ipos - 1);
    if (ycbar)
    {
      if (cbar->data() > ycbar->data())
        bar->setColor(upColor);
      else
      {
        if (cbar->data() < ycbar->data())
          bar->setColor(downColor);
      }
    }

    line->setBar(ipos, bar);
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  CANDLES *o = new CANDLES;
  return ((Plugin *) o);
}
