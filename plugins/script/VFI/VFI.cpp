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

#include "VFI.h"
#include "Curve.h"

#include <QtDebug>
#include <cmath>

VFI::VFI ()
{
}

int VFI::command (Command &command)
{
  // VFI,<NAME>,<PERIOD>
  //  0     1      2

  if (command.count() != 3)
  {
    qDebug() << "VFI::command: invalid settings count" << command.count();
    return 1;
  }

  BarData *data = command.barData();
  if (! data)
  {
    qDebug() << "VFI::command: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "VFI::command: no indicator";
    return 1;
  }

  int pos = 1;
  QString name = command.parm(pos);

  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "VFI::command: duplicate name" << name;
    return 1;
  }

  pos++;
  bool ok;
  int period = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "VFI::command: invalid period" << command.parm(pos);
    return 1;
  }

  int size = data->count();

  if (size < period)
    return 1;

  Curve *vfi = new Curve;

  int loop = period;
  for (; loop < size; loop++)
  {
    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    Bar *bar = data->bar(loop - period);
    double close = bar->close();
    double high = bar->high();
    double low = bar->low();
    double typical = (high + low + close) / 3.0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      bar = data->bar(i);
      double ytypical = typical;
      close = bar->close();
      high = bar->high();
      low = bar->low();
      typical = (high + low + close) / 3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta * delta;
      sma_vol += bar->volume();
    }
    inter = 0.2 * sqrt(inter / (double) period) * typical;
    sma_vol /= (double) period;

    bar = data->bar(loop - period);
    close = bar->close();
    high = bar->high();
    low = bar->low();
    typical = (high + low + close) / 3.0;
    double t = 0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      bar = data->bar(i);
      double ytypical = typical;
      double volume = bar->volume();
      close = bar->close();
      high = bar->high();
      low = bar->low();
      typical = (high + low + close) / 3.0;

      if (typical > ytypical + inter)
        t = t + log (1.0 + volume / sma_vol);
      else
      {
        if (typical < ytypical - inter)
          t = t - log (1.0 + volume / sma_vol);
      }
    }

    vfi->setBar(loop, new CurveBar(t));
  }

  vfi->setLabel(name);

  i->setLine(name, vfi);

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  VFI *o = new VFI;
  return ((ScriptPlugin *) o);
}
