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

#include "ADXR.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "ADXRDialog.h"
#include "InputType.h"

#include <QtDebug>

ADXR::ADXR ()
{
  _plugin = "ADXR";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("ADXR::ADXR: error on TA_Initialize");
}

int ADXR::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  Curve *line = i->line(settings->data("OUTPUT"));
  if (line)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT" << settings->data("OUTPUT");
    return 1;
  }

  int period = settings->getInt("PERIOD");

  Curve *ihigh = i->line("High");
  if (! ihigh)
  {
    InputType it;
    ihigh = it.input(bd, "High");
    if (! ihigh)
    {
      qDebug() << _plugin << "::calculate: no High";
      return 1;
    }

    ihigh->setLabel("High");
    i->setLine("High", ihigh);
  }

  Curve *ilow = i->line("Low");
  if (! ilow)
  {
    InputType it;
    ilow = it.input(bd, "Low");
    if (! ilow)
    {
      qDebug() << _plugin << "::calculate: no Low";
      return 1;
    }

    ilow->setLabel("Low");
    i->setLine("Low", ilow);
  }

  Curve *iclose = i->line("Close");
  if (! iclose)
  {
    InputType it;
    iclose = it.input(bd, "Close");
    if (! iclose)
    {
      qDebug() << _plugin << "::calculate: no Close";
      return 1;
    }

    iclose->setLabel("Close");
    i->setLine("Close", iclose);
  }

  line = getADXR(ihigh, ilow, iclose, period);
  if (! line)
    return 1;

  line->setAllColor(QColor(settings->data("COLOR")));
  line->setLabel(settings->data("OUTPUT"));
  line->setType(settings->data("STYLE"));
  line->setZ(settings->getInt("Z"));
  i->setLine(settings->data("OUTPUT"), line);

  return 0;
}

int ADXR::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
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

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  bool ok;
  int period = 14;
  QString s = command->parm("PERIOD");
  if (! s.isEmpty())
  {
    period = s.toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid period" << s;
      return 1;
    }
  }

  line = getADXR(ihigh, ilow, iclose, period);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * ADXR::getADXR (Curve *ihigh, Curve *ilow, Curve *iclose, int period)
{
  QList<int> keys;
  int size = ihigh->count();
  ihigh->keys(keys);

  if (ilow->count() < size)
  {
    size = ilow->count();
    ilow->keys(keys);
  }

  if (iclose->count() < size)
  {
    size = iclose->count();
    iclose->keys(keys);
  }

  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int ipos = 0;
  int opos = 0;
  for (; ipos < keys.count(); ipos++, opos++)
  {
    CurveBar *hbar = ihigh->bar(keys.at(ipos));
    if (! hbar)
      continue;

    CurveBar *lbar = ilow->bar(keys.at(ipos));
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(keys.at(ipos));
    if (! cbar)
      continue;

    high[opos] = (TA_Real) hbar->data();
    low[opos] = (TA_Real) lbar->data();
    close[opos] = (TA_Real) cbar->data();
  }

  TA_RetCode rc = TA_ADXR(0,
                          size - 1,
                          &high[0],
                          &low[0],
                          &close[0],
                          period,
                          &outBeg,
                          &outNb,
                          &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getADXR: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;
  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  return line;
}

QWidget * ADXR::dialog (QWidget *p, Setting *set)
{
  return new ADXRDialog(p, set);
}

void ADXR::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("blue"));
  set->setData("OUTPUT", _plugin);
  set->setData("STYLE", QString("Line"));
  set->setData("PERIOD", 14);
  set->setData("Z", 0);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  ADXR *o = new ADXR;
  return ((Plugin *) o);
}
