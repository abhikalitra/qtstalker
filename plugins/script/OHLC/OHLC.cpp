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

#include "OHLC.h"
#include "Curve.h"
#include "Globals.h"
#include "OHLCDialog.h"
#include "InputType.h"

#include <QtDebug>

OHLC::OHLC ()
{
  _type = "INDICATOR";
  _plugin = "OHLC";
}

int OHLC::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  Curve *line = i->line(settings->data("OUTPUT"));
  if (line)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT" << settings->data("OUTPUT");
    return 1;
  }

  InputType it;
  line = it.ohlc(bd);
  if (! line)
    return 1;
  
  if (settings->data("STYLE") == "Bars")
    line->setType("Bars");
  else
    line->setType("Candle");

  line->setAllColor(QColor(settings->data("COLOR")));
  line->setZ(settings->getInt("Z"));
  line->setLabel(settings->data("OUTPUT"));
  i->setLine(settings->data("OUTPUT"), line);

  return 0;
}

int OHLC::command (Command *command)
{
  // PARMS:
  // INPUT_OPEN
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // NAME
  // COLOR

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate name" << name;
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

  QColor color("#A0A0A0");
  QString s = command->parm("COLOR");
  if (! s.isEmpty())
  {
    color.setNamedColor(s);
    if (! color.isValid())
    {
      qDebug() << _plugin << "::command: invalid COLOR" << command->parm("COLOR");
      return 1;
    }
  }

  QList<Curve *> list;
  list << iopen << ihigh << ilow << iclose;
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
  {
    qDebug() << _plugin << "::command: invalid keys";
    return 1;
  }
  
  line = new Curve;
  line->setType("Bars");

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *obar = iopen->bar(keys.at(loop));
    if (! obar)
      continue;

    CurveBar *hbar = ihigh->bar(keys.at(loop));
    if (! hbar)
      continue;

    CurveBar *lbar = ilow->bar(keys.at(loop));
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(keys.at(loop));
    if (! cbar)
      continue;

    CurveBar *bar = new CurveBar;
    bar->setData(0, obar->data());
    bar->setData(1, hbar->data());
    bar->setData(2, lbar->data());
    bar->setData(3, cbar->data());
    bar->setColor(color);
    line->setBar(keys.at(loop), bar);
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

QWidget * OHLC::dialog (QWidget *p, Setting *set)
{
  return new OHLCDialog(p, set);
}

void OHLC::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("#A0A0A0"));
  set->setData("STYLE", QString("Bars"));
  set->setData("Z", 0);
  set->setData("OUTPUT", QString("OHLC"));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  OHLC *o = new OHLC;
  return ((Plugin *) o);
}
