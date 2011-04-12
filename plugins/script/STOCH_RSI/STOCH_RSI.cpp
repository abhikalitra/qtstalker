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

#include "STOCH_RSI.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "STOCHRSIDialog.h"
#include "InputType.h"

#include <QtDebug>

STOCH_RSI::STOCH_RSI ()
{
  _plugin = "STOCH_RSI";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("STOCH_RSI::STOCH_RSI: error on TA_Initialize");
}

int STOCH_RSI::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  Curve *line = i->line(settings->data("OUTPUT"));
  if (line)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT" << settings->data("OUTPUT");
    return 1;
  }

  int period = settings->getInt("PERIOD");

  Curve *in = i->line(settings->data("INPUT"));
  if (! in)
  {
    InputType it;
    in = it.input(bd, settings->data("INPUT"));
    if (! in)
    {
      qDebug() << _plugin << "::calculate: no input" << settings->data("INPUT");
      return 1;
    }

    in->setZ(-1);
    i->setLine(settings->data("INPUT"), in);
  }

  line = getSTOCHRSI(in, period);
  if (! line)
    return 1;

  line->setAllColor(QColor(settings->data("COLOR")));
  line->setLabel(settings->data("OUTPUT"));
  line->setType(settings->data("STYLE"));
  line->setZ(settings->getInt("Z"));
  i->setLine(settings->data("OUTPUT"), line);

  return 0;
}

int STOCH_RSI::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // PERIOD

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
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::command: INPUT missing" << command->parm("INPUT");
    return 1;
  }

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD" << command->parm("PERIOD");
    return 1;
  }

  line = getSTOCHRSI(in, period);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * STOCH_RSI::getSTOCHRSI (Curve *in, int period)
{
  QList<int> keys;
  in->keys(keys);
  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_STOCHRSI(0,
                              size - 1,
                              &input[0],
                              period,
                              period,
                              1,
                              (TA_MAType) 0,
                              &outBeg,
                              &outNb,
                              &out[0],
                              &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getSTOCHRSI: TA-Lib error" << rc;
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

QWidget * STOCH_RSI::dialog (QWidget *p, Setting *set)
{
  return new STOCHRSIDialog(p, set);
}

void STOCH_RSI::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("red"));
  set->setData("STYLE", QString("Line"));
  set->setData("INPUT", QString("Close"));
  set->setData("PERIOD", 14);
  set->setData("Z", 0);
  set->setData("OUTPUT", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  STOCH_RSI *o = new STOCH_RSI;
  return ((Plugin *) o);
}
