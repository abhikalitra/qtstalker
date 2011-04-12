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

#include "AROON.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "AROONDialog.h"
#include "InputType.h"

#include <QtDebug>

AROON::AROON ()
{
  _plugin = "AROON";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("AROON::AROON: error on TA_Initialize");
}

int AROON::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  Curve *upper = i->line(settings->data("OUTPUT_UP"));
  if (upper)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT_UP" << settings->data("OUTPUT_UP");
    return 1;
  }

  Curve *lower = i->line(settings->data("OUTPUT_DOWN"));
  if (lower)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT_DOWN" << settings->data("OUTPUT_DOWN");
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

  QList<Curve *> lines = getAROON(ihigh, ilow, period);
  if (! lines.count())
    return 1;

  upper = lines.at(0);
  upper->setAllColor(QColor(settings->data("COLOR_UP")));
  upper->setLabel(settings->data("OUTPUT_UP"));
  upper->setType(settings->data("STYLE_UP"));
  upper->setZ(settings->getInt("Z_UP"));
  i->setLine(settings->data("OUTPUT_UP"), upper);
  
  if (lines.count() == 2)
  {
    lower = lines.at(1);
    lower->setAllColor(QColor(settings->data("COLOR_DOWN")));
    lower->setLabel(settings->data("OUTPUT_DOWN"));
    lower->setType(settings->data("STYLE_DOWN"));
    lower->setZ(settings->getInt("Z_DOWN"));
    i->setLine(settings->data("OUTPUT_DOWN"), lower);
  }

  return 0;
}

int AROON::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // NAME_UPPER
  // NAME_LOWER
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

  QString uname = command->parm("NAME_UPPER");
  Curve *line = i->line(uname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_UPPER" << uname;
    return 1;
  }

  QString lname = command->parm("NAME_LOWER");
  line = i->line(lname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_LOWER" << lname;
    return 1;
  }

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid period" << command->parm("PERIOD");
    return 1;
  }

  QList<Curve *> lines = getAROON(ihigh, ilow, period);
  if (! lines.count())
    return 1;

  Curve *upper = lines.at(0);
  upper->setLabel(uname);
  i->setLine(uname, upper);

  if (lines.count() == 2)
  {
    Curve *lower = lines.at(1);
    lower->setLabel(lname);
    i->setLine(lname, lower);
  }

  command->setReturnCode("0");

  return 0;
}

QList<Curve *> AROON::getAROON (Curve *ihigh, Curve *ilow, int period)
{
  QList<int> keys;
  int size = ihigh->count();
  ihigh->keys(keys);

  if (ilow->count() < size)
  {
    size = ilow->count();
    ilow->keys(keys);
  }

  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  TA_Real out2[size];
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

    high[opos] = (TA_Real) hbar->data();
    low[opos] = (TA_Real) lbar->data();
  }

  TA_RetCode rc = TA_AROON(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           period,
                           &outBeg,
                           &outNb,
                           &out[0],
                           &out2[0]);

  QList<Curve *> lines;
  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getAROON: TA-Lib error" << rc;
    return lines;
  }

  Curve *upper = new Curve;
  Curve *lower = new Curve;
  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    upper->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    lower->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));
    keyLoop--;
    outLoop--;
  }

  lines.append(upper);
  lines.append(lower);

  return lines;
}

QWidget * AROON::dialog (QWidget *p, Setting *set)
{
  return new AROONDialog(p, set);
}

void AROON::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR_UP", QString("green"));
  set->setData("OUTPUT_UP", QString("AROONU"));
  set->setData("STYLE_UP", QString("Line"));
  set->setData("COLOR_DOWN", QString("red"));
  set->setData("OUTPUT_DOWN", QString("AROOND"));
  set->setData("STYLE_DOWN", QString("Line"));
  set->setData("PERIOD", 14);
  set->setData("Z_UP", 0);
  set->setData("Z_DOWN", 0);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  AROON *o = new AROON;
  return ((Plugin *) o);
}
