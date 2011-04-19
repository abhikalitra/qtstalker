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
#include "Globals.h"
#include "VFIWidget.h"
#include "InputType.h"

#include <QtDebug>
#include <cmath>

VFI::VFI ()
{
  _plugin = "VFI";
  _type = "INDICATOR";
}

int VFI::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // INPUT_VOLUME
  // NAME
  // PERIOD

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
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

  Curve *ivol = i->line(command->parm("INPUT_VOLUME"));
  if (! ivol)
  {
    qDebug() << _plugin << "::command: invalid INPUT_VOLUME" << command->parm("INPUT_VOLUME");
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD" << command->parm("PERIOD");
    return 1;
  }

  QList<Curve *> list;
  list << ihigh << ilow << iclose << ivol;
  line = getVFI(list, period);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * VFI::getVFI (QList<Curve *> &list, int period)
{
  if (list.count() != 4)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;
  
  Curve *vfi = new Curve;
  Curve *ihigh = list.at(0);
  Curve *ilow = list.at(1);
  Curve *iclose = list.at(2);
  Curve *ivol = list.at(3);
  int loop = period;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *hbar = ihigh->bar(keys.at(loop - period));
    if (! hbar)
      continue;

    CurveBar *lbar = ilow->bar(keys.at(loop - period));
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(keys.at(loop - period));
    if (! cbar)
      continue;

    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    double close = cbar->data();
    double high = hbar->data();
    double low = lbar->data();
    double typical = (high + low + close) / 3.0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      hbar = ihigh->bar(keys.at(i));
      if (! hbar)
        continue;

      lbar = ilow->bar(keys.at(i));
      if (! lbar)
        continue;

      cbar = iclose->bar(keys.at(i));
      if (! cbar)
        continue;

      CurveBar *vbar = ivol->bar(keys.at(i));
      if (! vbar)
        continue;

      double ytypical = typical;
      close = cbar->data();
      high = hbar->data();
      low = lbar->data();
      typical = (high + low + close) / 3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta * delta;
      sma_vol += vbar->data();
    }
    inter = 0.2 * sqrt(inter / (double) period) * typical;
    sma_vol /= (double) period;

    hbar = ihigh->bar(keys.at(loop - period));
    if (! hbar)
      continue;

    lbar = ilow->bar(keys.at(loop - period));
    if (! lbar)
      continue;

    cbar = iclose->bar(keys.at(loop - period));
    if (! cbar)
      continue;

    close = cbar->data();
    high = hbar->data();
    low = lbar->data();
    typical = (high + low + close) / 3.0;
    double t = 0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      hbar = ihigh->bar(keys.at(i));
      if (! hbar)
        continue;

      lbar = ilow->bar(keys.at(i));
      if (! lbar)
        continue;

      cbar = iclose->bar(keys.at(i));
      if (! cbar)
        continue;

      CurveBar *vbar = ivol->bar(keys.at(i));
      if (! vbar)
        continue;

      double ytypical = typical;
      double volume = vbar->data();
      close = cbar->data();
      high = hbar->data();
      low = lbar->data();
      typical = (high + low + close) / 3.0;

      if (typical > ytypical + inter)
        t = t + log (1.0 + volume / sma_vol);
      else
      {
        if (typical < ytypical - inter)
          t = t - log (1.0 + volume / sma_vol);
      }
    }

    vfi->setBar(keys.at(loop), new CurveBar(t));
  }

  return vfi;
}

PluginWidget * VFI::dialog (QWidget *p)
{
  return new VFIWidget(p);
}

void VFI::defaults (QString &d)
{
  QStringList l;
  l << "PLUGIN=" + _plugin;
  l << "NAME=" + _plugin;
  l << "INPUT_HIGH=High";
  l << "INPUT_LOW=Low";
  l << "INPUT_CLOSE=Close";
  l << "INPUT_VOLUME=Volume";
  l << "PERIOD=10";
  d = l.join(",");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  VFI *o = new VFI;
  return ((Plugin *) o);
}
