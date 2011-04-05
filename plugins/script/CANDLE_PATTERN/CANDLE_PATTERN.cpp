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

#include "CANDLE_PATTERN.h"
#include "Curve.h"
#include "Globals.h"
#include "CandlePatternDialog.h"
#include "CandleType.h"
#include "InputType.h"

#include <QtDebug>

CANDLE_PATTERN::CANDLE_PATTERN ()
{
  _plugin = "CANDLE_PATTERN";
  _type = "INDICATOR";
}

int CANDLE_PATTERN::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  QColor c(settings->data("COLOR"));
  InputType it;
  Curve *line = it.ohlc(bd, c, c, c);
  if (! line)
    return 1;

  line->setType("Candle");
  line->setLabel("CANDLES");
  line->setZ(settings->getInt("Z"));
  i->setLine("CANDLES", line);

  double pen = settings->getDouble("PEN");

  CandleType ct;
  QStringList l = settings->data("PATTERN").split(",", QString::SkipEmptyParts);
  int loop = 0;
  for (; loop < l.count(); loop += 2)
  {
    Curve *tline = ct.getPattern(bd, ct.fromString(l.at(loop)), pen);
    if (! tline)
      continue;

    int loop2 = 0;
    for (; loop2 < tline->count(); loop2++)
    {
      CurveBar *cb = tline->bar(loop2);
      if (! cb)
	continue;

      if (cb->data() != 0)
      {
	CurveBar *cb2 = line->bar(loop2);
	if (! cb2)
	  continue;
	
	cb2->setColor(QColor(l.at(loop + 1)));
      }
    }

    delete tline;
  }
  
  return 0;  
}

int CANDLE_PATTERN::command (Command *command)
{
  // PARMS:
  // INPUT_OPEN
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // METHOD
  // NAME
  // PENETRATION

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
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  double pen = 0;
  QString s = command->parm("PENETRATION");
  if (! s.isEmpty())
  {
    bool ok;
    pen = s.toDouble(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid PENETRATION" << command->parm("PENETRATION");
      return 1;
    }
  }

  CandleType ct;
  int method = ct.fromString(command->parm("METHOD"));
  if (method == -1)
  {
    qDebug() << _plugin << "::command: invalid METHOD" << command->parm("METHOD");
    return 1;
  }
  
  BarData bd;
  int ipos = 0;
  int opos = 0;
  int end = 0;
  iclose->keyRange(ipos, end);
  for (; ipos <= end; ipos++, opos++)
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

    Bar *b = new Bar;
    b->setOpen(obar->data());
    b->setHigh(hbar->data());
    b->setLow(lbar->data());
    b->setClose(cbar->data());
    bd.append(b);
  }

  line = ct.getPattern(&bd, method, pen);
  if (! line)
  {
    qDebug() << _plugin << "::command: CandleType error";
    return 1;
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

QWidget * CANDLE_PATTERN::dialog (QWidget *p, Setting *set)
{
  return new CandlePatternDialog(p, set);
}

void CANDLE_PATTERN::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("dimgray"));
  set->setData("METHOD", QString());
  set->setData("PEN", 50);
  set->setData("Z", 0);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  CANDLE_PATTERN *o = new CANDLE_PATTERN;
  return ((Plugin *) o);
}
