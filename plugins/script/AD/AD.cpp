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

#include "AD.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "ADDialog.h"
#include "InputType.h"

#include <QtDebug>

AD::AD ()
{
  _plugin = "AD";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("AD::AD: error on TA_Initialize");
}

int AD::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  Curve *line = i->line(settings->data("OUTPUT"));
  if (line)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT" << settings->data("OUTPUT");
    return 1;
  }

  Curve *high = i->line("High");
  if (! high)
  {
    InputType it;
    high = it.input(bd, "High");
    if (! high)
    {
      qDebug() << _plugin << "::calculate: no High";
      return 1;
    }

    high->setLabel("High");
    i->setLine("High", high);
  }

  Curve *low = i->line("Low");
  if (! low)
  {
    InputType it;
    low = it.input(bd, "Low");
    if (! low)
    {
      qDebug() << _plugin << "::calculate: no Low";
      return 1;
    }

    low->setLabel("Low");
    i->setLine("Low", low);
  }

  Curve *close = i->line("Close");
  if (! close)
  {
    InputType it;
    close = it.input(bd, "Close");
    if (! close)
    {
      qDebug() << _plugin << "::calculate: no Close";
      return 1;
    }

    close->setLabel("Close");
    i->setLine("Close", close);
  }

  Curve *vol = i->line("Volume");
  if (! vol)
  {
    InputType it;
    vol = it.input(bd, "Volume");
    if (! vol)
    {
      qDebug() << _plugin << "::calculate: no Volume";
      return 1;
    }

    vol->setLabel("Volume");
    i->setLine("Volume", vol);
  }

  line = getAD(high, low, close, vol);
  if (! line)
    return 1;

  line->setAllColor(QColor(settings->data("COLOR")));
  line->setLabel(settings->data("OUTPUT"));
  line->setType(settings->data("STYLE"));
  line->setZ(settings->getInt("Z"));
  i->setLine(settings->data("OUTPUT"), line);

  return 0;
}

int AD::command (Command *command)
{
  // PARMS
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // INPUT_VOLUME
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
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  line = getAD(ihigh, ilow, iclose, ivol);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * AD::getAD (Curve *ihigh, Curve *ilow, Curve *iclose, Curve *ivol)
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

  if (ivol->count() < size)
  {
    size = ivol->count();
    ivol->keys(keys);
  }

  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real volume[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *hbar = ihigh->bar(keys.at(loop));
    if (! hbar)
      continue;

    CurveBar *lbar = ilow->bar(keys.at(loop));
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(keys.at(loop));
    if (! cbar)
      continue;

    CurveBar *vbar = ivol->bar(keys.at(loop));
    if (! vbar)
      continue;

    high[loop] = (TA_Real) hbar->data();
    low[loop] = (TA_Real) lbar->data();
    close[loop] = (TA_Real) cbar->data();
    volume[loop] = (TA_Real) vbar->data();
  }

  TA_RetCode rc = TA_AD(0,
                        size - 1,
                        &high[0],
                        &low[0],
                        &close[0],
                        &volume[0],
                        &outBeg,
                        &outNb,
                        &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getAD: TA-Lib error" << rc;
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

QWidget * AD::dialog (QWidget *p, Setting *set)
{
  return new ADDialog(p, set);
}

void AD::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("red"));
  set->setData("STYLE", QString("Line"));
  set->setData("OUTPUT", _plugin);
  set->setData("Z", 0);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  AD *o = new AD;
  return ((Plugin *) o);
}
