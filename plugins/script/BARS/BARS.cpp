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

#include "BARS.h"
#include "Curve.h"
#include "Globals.h"
#include "ScriptPluginFactory.h"

#include <QtDebug>

BARS::BARS ()
{
  _plugin = "BARS";
  _method << "BARS" << "CANDLES" << "Open" << "High" << "Low" << "Close" << "Volume" << "OI";
}

int BARS::command (Command *command)
{
  // PARMS:
  // <METHOD>

  switch ((Method) _method.indexOf(command->parm("METHOD")))
  {
    case _BARS:
      return getBars(command);
      break;
    case _CANDLES:
      return getCandles(command);
      break;
    case _OPEN:
    case _HIGH:
    case _LOW:
    case _CLOSE:
    case _VOLUME:
    case _OI:
      return getBarField(command);
      break;
    default:
      break;
  }

  return 0;
}

int BARS::getBars (Command *command)
{
  // PARMS:
  // <NAME>
  // <COLOR_UP>
  // <COLOR_DOWN>
  // <COLOR_NEUTRAL>

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::getBars: no indicator";
    return 1;
  }

  int size = g_barData->count();
  if (size < 1)
  {
    qDebug() << _plugin << "::getBars: no bars";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::getBars: duplicate name" << name;
    return 1;
  }

  QColor upColor(command->parm("COLOR_UP"));
  if (! upColor.isValid())
  {
    qDebug() << _plugin << "::getBars: invalid COLOR_UP" << command->parm("COLOR_UP");
    return 1;
  }

  QColor downColor(command->parm("COLOR_DOWN"));
  if (! downColor.isValid())
  {
    qDebug() << _plugin << "::getBars: invalid COLOR_DOWN" << command->parm("COLOR_DOWN");
    return 1;
  }

  QColor neutralColor(command->parm("COLOR_NEUTRAL"));
  if (! neutralColor.isValid())
  {
    qDebug() << _plugin << "::getBars: invalid COLOR_NEUTRAL" << command->parm("COLOR_NEUTRAL");
    return 1;
  }

  line = new Curve(Curve::OHLC);

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *b = g_barData->bar(loop);
    if (! b)
      continue;

    CurveBar *bar = new CurveBar;
    bar->setData(0, b->open());
    bar->setData(1, b->high());
    bar->setData(2, b->low());
    bar->setData(3, b->close());
    bar->setColor(neutralColor);
    
    Bar *yb = g_barData->bar(loop - 1);
    if (yb)
    {
      if (b->close() > yb->close())
        bar->setColor(upColor);
      else
      {
        if (b->close() < yb->close())
          bar->setColor(downColor);
      }
    }

    line->setBar(loop, bar);
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

int BARS::getCandles (Command *command)
{
  // PARMS:
  // <NAME>
  // <COLOR_UP>
  // <COLOR_DOWN>
  // <COLOR_NEUTRAL>

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::getCandles: no indicator";
    return 1;
  }

  int size = g_barData->count();
  if (size < 1)
  {
    qDebug() << _plugin << "::getCandles: no bars";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::getCandles: duplicate name" << name;
    return 1;
  }

  QColor upColor(command->parm("COLOR_UP"));
  if (! upColor.isValid())
  {
    qDebug() << _plugin << "::getCandles: invalid COLOR_UP" << command->parm("COLOR_UP");
    return 1;
  }

  QColor downColor(command->parm("COLOR_DOWN"));
  if (! downColor.isValid())
  {
    qDebug() << _plugin << "::getCandles: invalid COLOR_DOWN" << command->parm("COLOR_DOWN");
    return 1;
  }

  QColor neutralColor(command->parm("COLOR_NEUTRAL"));
  if (! neutralColor.isValid())
  {
    qDebug() << _plugin << "::getCandles: invalid COLOR_NEUTRAL" << command->parm("COLOR_NEUTRAL");
    return 1;
  }

  line = new Curve(Curve::Candle);

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *b = g_barData->bar(loop);
    if (! b)
      continue;

    CurveBar *bar = new CurveBar;
    bar->setData(0, b->open());
    bar->setData(1, b->high());
    bar->setData(2, b->low());
    bar->setData(3, b->close());
    bar->setColor(neutralColor);

    Bar *yb = g_barData->bar(loop - 1);
    if (yb)
    {
      if (b->close() > yb->close())
        bar->setColor(upColor);
      else
      {
        if (b->close() < yb->close())
          bar->setColor(downColor);
      }
    }

    line->setBar(loop, bar);
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

int BARS::getBarField (Command *command)
{
  // PARMS:
  // <METHOD>
  // <NAME>

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::getBarField: no indicator";
    return 1;
  }

  if (g_barData->count() < 1)
  {
    qDebug() << _plugin << "::getBarField: no bars";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::getBarField: duplicate name" << name;
    return 1;
  }

  line = g_barData->input(g_barData->inputType(command->parm("METHOD")));
  if (! line)
  {
    qDebug() << _plugin << "::getBarField: no input returned";
    return 1;
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  BARS *o = new BARS;
  return ((ScriptPlugin *) o);
}
