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

#include "MACD.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "InputType.h"
#include "MAType.h"
#include "MACDDialog.h"

#include <QtDebug>

MACD::MACD ()
{
  _plugin = "MACD";
  _type = _INDICATOR;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("MACD::MACD: error on TA_Initialize");
}

int MACD::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  int fperiod = settings->getInt("PERIOD_FAST");
  int speriod = settings->getInt("PERIOD_SLOW");
  int sigperiod = settings->getInt("PERIOD_SIG");

  MAType mat;
  int ftype = mat.fromString(settings->data("MA_TYPE_FAST"));
  int stype = mat.fromString(settings->data("MA_TYPE_SLOW"));
  int sigtype = mat.fromString(settings->data("MA_TYPE_SIG"));

  InputType it;
  Curve *in = it.input(bd, settings->data("INPUT"));
  if (! in)
    return 1;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  delete in;

  TA_RetCode rc = TA_MACDEXT(0,
                             size - 1,
                             &input[0],
                             fperiod,
                             (TA_MAType) ftype,
                             speriod,
                             (TA_MAType) stype,
                             sigperiod,
                             (TA_MAType) sigtype,
                             &outBeg,
                             &outNb,
                             &out[0],
                             &out2[0],
                             &out3[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  Curve *macd = new Curve;
  Curve *signal = new Curve;
  Curve *hist = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    macd->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    signal->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));
    hist->setBar(keys.at(keyLoop), new CurveBar(out3[outLoop]));

    keyLoop--;
    outLoop--;
  }

  macd->setAllColor(QColor(settings->data("COLOR_MACD")));
  macd->setLabel(settings->data("OUTPUT_MACD"));
  macd->setType((Curve::Type) macd->typeFromString(settings->data("STYLE_MACD")));
  macd->setZ(settings->getInt("Z_MACD"));
  i->setLine(settings->data("OUTPUT_MACD"), macd);
  
  i->setLine(settings->data("OUTPUT_SIG"), signal);
  signal->setAllColor(QColor(settings->data("COLOR_SIG")));
  signal->setLabel(settings->data("OUTPUT_SIG"));
  signal->setType((Curve::Type) signal->typeFromString(settings->data("STYLE_SIG")));
  signal->setZ(settings->getInt("Z_SIG"));

  hist->setAllColor(QColor(settings->data("COLOR_HIST")));
  hist->setLabel(settings->data("OUTPUT_HIST"));
  hist->setType((Curve::Type) hist->typeFromString(settings->data("STYLE_HIST")));
  hist->setZ(settings->getInt("Z_HIST"));
  i->setLine(settings->data("OUTPUT_HIST"), hist);

  return 0;
}

int MACD::command (Command *command)
{
  // PARMS:
  // INPUT
  // NAME_MACD
  // NAME_SIGNAL
  // NAME_HIST
  // PERIOD_FAST
  // MA_TYPE_FAST
  // PERIOD_SLOW
  // MA_TYPE_SLOW
  // PERIOD_SIGNAL
  // MA_TYPE_SIGNAL

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::command: INPUT missing" << command->parm("INPUT");
    return 1;
  }

  QString mname = command->parm("NAME_MACD");
  Curve *line = i->line(mname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_MACD" << mname;
    return 1;
  }

  QString sname = command->parm("NAME_SIGNAL");
  line = i->line(sname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_SIGNAL" << sname;
    return 1;
  }

  QString hname = command->parm("NAME_HIST");
  line = i->line(hname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_HIST" << hname;
    return 1;
  }

  bool ok;
  int fperiod = command->parm("PERIOD_FAST").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_FAST" << command->parm("PERIOD_FAST");
    return 1;
  }

  MAType mat;
  int ftype = mat.fromString(command->parm("MA_TYPE_FAST"));
  if (ftype == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE_FAST" << command->parm("MA_TYPE_FAST");
    return 1;
  }
  
  int speriod = command->parm("PERIOD_SLOW").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_SLOW" << command->parm("PERIOD_SLOW");
    return 1;
  }

  int stype = mat.fromString(command->parm("MA_TYPE_SLOW"));
  if (stype == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE_SLOW" << command->parm("MA_TYPE_SLOW");
    return 1;
  }

  int sigperiod = command->parm("PERIOD_SIGNAL").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_SIGNAL" << command->parm("PERIOD_SIGNAL");
    return 1;
  }

  int sigtype = mat.fromString(command->parm("MA_TYPE_SIGNAL"));
  if (sigtype == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE_SIGNAL" << command->parm("MA_TYPE_SIGNAL");
    return 1;
  }

  if (in->count() < fperiod || in->count() < speriod || in->count() < sigperiod)
    return 1;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_MACDEXT(0,
                             size - 1,
                             &input[0],
                             fperiod,
                             (TA_MAType) ftype,
                             speriod,
                             (TA_MAType) stype,
                             sigperiod,
                             (TA_MAType) sigtype,
                             &outBeg,
                             &outNb,
                             &out[0],
                             &out2[0],
                             &out3[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::command: TA-Lib error" << rc;
    return 1;
  }

  Curve *macd = new Curve;
  Curve *signal = new Curve;
  Curve *hist = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    macd->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    signal->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));
    hist->setBar(keys.at(keyLoop), new CurveBar(out3[outLoop]));

    keyLoop--;
    outLoop--;
  }

  macd->setLabel(mname);
  signal->setLabel(sname);
  hist->setLabel(hname);

  i->setLine(mname, macd);
  i->setLine(sname, signal);
  i->setLine(hname, hist);

  command->setReturnCode("0");

  return 0;
}

QWidget * MACD::dialog (QWidget *p, Setting *set)
{
  return new MACDDialog(p, set);
}

void MACD::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("INPUT", QString("Close"));
  set->setData("COLOR_MACD", QString("red"));
  set->setData("COLOR_SIG", QString("yellow"));
  set->setData("COLOR_HIST", QString("blue"));
  set->setData("OUTPUT_MACD", QString("MACD"));
  set->setData("OUTPUT_SIG", QString("SIG"));
  set->setData("OUTPUT_HIST", QString("HIST"));
  set->setData("STYLE_MACD", QString("Line"));
  set->setData("STYLE_SIG", QString("Line"));
  set->setData("STYLE_HIST", QString("Histogram Bar"));
  set->setData("PERIOD_FAST", 12);
  set->setData("PERIOD_SLOW", 26);
  set->setData("PERIOD_SIG", 9);
  set->setData("MA_TYPE_FAST", QString("EMA"));
  set->setData("MA_TYPE_SLOW", QString("EMA"));
  set->setData("MA_TYPE_SIG", QString("EMA"));
  set->setData("Z_MACD", 1);
  set->setData("Z_SIG", 2);
  set->setData("Z_HIST", 0);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  MACD *o = new MACD;
  return ((Plugin *) o);
}
