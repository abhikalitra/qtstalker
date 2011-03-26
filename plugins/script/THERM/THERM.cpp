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

/* The "Market Thermometer" _indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

/* Alert Notes
1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days
*/


#include "THERM.h"
#include "Curve.h"
#include "Globals.h"
#include "THERMDialog.h"
#include "InputType.h"
#include "MAType.h"

#include <QtDebug>
#include <cmath>

THERM::THERM ()
{
  _plugin = "THERM";
  _type = _INDICATOR;
}

int THERM::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();

  InputType it;
  Curve *high = it.input(bd, "High");
  if (! high)
    return 1;

  Curve *low = it.input(bd, "Low");
  if (! low)
  {
    delete high;
    return 1;
  }

  Curve *line = getTHERM(high, low);

  delete high;
  delete low;

  int smoothing = settings->getInt(_SMOOTHING);
  if (smoothing > 1)
  {
    MAType mat;
    Curve *ma = mat.getMA(line, smoothing, mat.fromString(settings->data(_SMOOTHING_TYPE)));
    if (ma)
    {
      delete line;
      line = ma;
    }
  }

  line->setAllColor(QColor(settings->data(_COLOR)));
  line->setLabel(settings->data(_LABEL));
  line->setType((Curve::Type) line->typeFromString(settings->data(_STYLE)));
  line->setZ(0);
  i->setLine(settings->data(_LABEL), line);

  return 0;
}

int THERM::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // NAME

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

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "THERM::command: duplicate NAME" << name;
    return 1;
  }

  line = getTHERM(ihigh, ilow);

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * THERM::getTHERM (Curve *ihigh, Curve *ilow)
{
  Curve *line = new Curve;

  double thermometer = 0;
  int ipos = 0;
  int opos = 0;
  int end = 0;
  ihigh->keyRange(ipos, end);
  ipos++;
  for (; ipos <= end; ipos++, opos++)
  {
    CurveBar *hbar = ihigh->bar(ipos);
    if (! hbar)
      continue;

    CurveBar *phbar = ihigh->bar(ipos - 1);
    if (! phbar)
      continue;

    CurveBar *lbar = ilow->bar(ipos);
    if (! lbar)
      continue;

    CurveBar *plbar = ilow->bar(ipos - 1);
    if (! plbar)
      continue;

    double high = fabs(hbar->data() - phbar->data());
    double lo = fabs(plbar->data() - lbar->data());

    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    line->setBar(ipos, new CurveBar(thermometer));
  }

  return line;
}

void THERM::dialog (QWidget *p, Indicator *i)
{
  THERMDialog *dialog = new THERMDialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void THERM::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_COLOR, "red");
  set->setData(_LABEL, _plugin);
  set->setData(_STYLE, "Line");
  set->setData(_SMOOTHING, 9);
  set->setData(_SMOOTHING_TYPE, "EMA");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  THERM *o = new THERM;
  return ((Plugin *) o);
}
