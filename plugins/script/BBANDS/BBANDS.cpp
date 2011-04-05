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

#include "BBANDS.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "BBANDSDialog.h"
#include "MAType.h"
#include "InputType.h"

#include <QtDebug>

BBANDS::BBANDS ()
{
  _plugin = "BBANDS";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("BBANDS::BBANDS: error on TA_Initialize");
}

int BBANDS::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  int period = settings->getInt("PERIOD");
  double udev = settings->getDouble("DEVIATION_UP");
  double ldev = settings->getDouble("DEVIATION_DOWN");
  
  MAType types;
  int type = types.fromString(settings->data("MA_TYPE"));

  int delFlag = FALSE;
  Curve *input = i->line(settings->data("INPUT"));
  if (! input)
  {
    InputType it;
    input = it.input(bd, settings->data("INPUT"));
    if (! input)
    {
      qDebug() << _plugin << "::calculate: no input" << settings->data("INPUT");
      return 1;
    }

    delFlag++;
  }

  int size = input->count();
  TA_Real in[size];
  TA_Real uout[size];
  TA_Real mout[size];
  TA_Real lout[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  input->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = input->bar(keys.at(loop));
    in[loop] = (TA_Real) bar->data();
  }

  if (delFlag)
    delete input;

  TA_RetCode rc = TA_BBANDS(0,
                            keys.count() - 1,
                            &in[0],
                            period,
                            udev,
                            ldev,
                            (TA_MAType) type,
                            &outBeg,
                            &outNb,
                            &uout[0],
                            &mout[0],
                            &lout[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  Curve *upper = new Curve;
  Curve *middle = new Curve;
  Curve *lower = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    upper->setBar(keys.at(keyLoop), new CurveBar(uout[outLoop]));
    middle->setBar(keys.at(keyLoop), new CurveBar(mout[outLoop]));
    lower->setBar(keys.at(keyLoop), new CurveBar(lout[outLoop]));

    keyLoop--;
    outLoop--;
  }

  upper->setAllColor(QColor(settings->data("COLOR_UP")));
  upper->setLabel(settings->data("OUTPUT_UP"));
  upper->setType(settings->data("STYLE_UP"));
  upper->setZ(settings->getInt("Z_UP"));
  i->setLine(settings->data("OUTPUT_UP"), upper);
  
  middle->setAllColor(QColor(settings->data("COLOR_MID")));
  middle->setLabel(settings->data("OUTPUT_MID"));
  middle->setType(settings->data("STYLE_MID"));
  middle->setZ(settings->getInt("Z_UP"));
  i->setLine(settings->data("OUTPUT_MID"), middle);
  
  lower->setAllColor(QColor(settings->data("COLOR_DOWN")));
  lower->setLabel(settings->data("OUTPUT_DOWN"));
  lower->setType(settings->data("STYLE_DOWN"));
  lower->setZ(settings->getInt("Z_UP"));
  i->setLine(settings->data("OUTPUT_DOWN"), lower);

  return 0;
}

int BBANDS::command (Command *command)
{
  // PARMS:
  // INPUT
  // NAME_UPPER
  // NAME_MIDDLE
  // NAME_LOWER
  // PERIOD
  // MA_TYPE
  // DEV_UP
  // DEV_DOWN

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *input = i->line(command->parm("INPUT"));
  if (! input)
  {
    qDebug() << _plugin << "::command: INPUT missing" << command->parm("INPUT");
    return 1;
  }

  QString uname = command->parm("NAME_UPPER");
  Curve *line = i->line(uname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_UPPER" << uname;
    return 1;
  }

  QString mname = command->parm("NAME_MIDDLE");
  line = i->line(mname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_MIDDLE" << mname;
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
    qDebug() << _plugin << "::command: invalid PERIOD" << command->parm("PERIOD");
    return 1;
  }

  if (input->count() < period)
    return 1;

  MAType types;
  int type = types.fromString(command->parm("MA_TYPE"));
  if (type == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE" << command->parm("MA_TYPE");
    return 1;
  }
  
  double udev = command->parm("DEV_UP").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid DEV_UP" << command->parm("DEV_UP");
    return 1;
  }

  double ldev = command->parm("DEV_DOWN").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid DEV_DOWN" << command->parm("DEV_DOWN");
    return 1;
  }

  int size = input->count();
  TA_Real in[size];
  TA_Real uout[size];
  TA_Real mout[size];
  TA_Real lout[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  input->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = input->bar(keys.at(loop));
    in[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_BBANDS(0,
                            keys.count() - 1,
                            &in[0],
                            period,
                            udev,
                            ldev,
                            (TA_MAType) type,
                            &outBeg,
                            &outNb,
                            &uout[0],
                            &mout[0],
                            &lout[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::command: TA-Lib error" << rc;
    return 1;
  }

  Curve *upper = new Curve;
  Curve *middle = new Curve;
  Curve *lower = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    upper->setBar(keys.at(keyLoop), new CurveBar(uout[outLoop]));
    middle->setBar(keys.at(keyLoop), new CurveBar(mout[outLoop]));
    lower->setBar(keys.at(keyLoop), new CurveBar(lout[outLoop]));

    keyLoop--;
    outLoop--;
  }

  upper->setLabel(uname);
  middle->setLabel(mname);
  lower->setLabel(lname);

  i->setLine(uname, upper);
  i->setLine(mname, middle);
  i->setLine(lname, lower);

  command->setReturnCode("0");

  return 0;
}

QWidget * BBANDS::dialog (QWidget *p, Setting *set)
{
  return new BBANDSDialog(p, set);
}

void BBANDS::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR_UP", QString("red"));
  set->setData("OUTPUT_UP", QString("BBU"));
  set->setData("STYLE_UP", QString("Line"));
  set->setData("COLOR_MID", QString("yellow"));
  set->setData("OUTPUT_MID", QString("BBM"));
  set->setData("STYLE_MID", QString("Line"));
  set->setData("COLOR_DOWN", QString("green"));
  set->setData("OUTPUT_DOWN", QString("BBL"));
  set->setData("STYLE_DOWN", QString("Line"));
  set->setData("DEVIATION_DOWN", 2);
  set->setData("DEVIATION_UP", 2);
  set->setData("INPUT", QString("Close"));
  set->setData("PERIOD", 20);
  set->setData("MA_TYPE", QString("EMA"));
  set->setData("Z_UP", 1);
  set->setData("Z_MID", 1);
  set->setData("Z_DOWN", 1);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  BBANDS *o = new BBANDS;
  return ((Plugin *) o);
}
