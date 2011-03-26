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

#include "T3.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "T3Dialog.h"
#include "InputType.h"

#include <QtDebug>

T3::T3 ()
{
  _plugin = "T3";
  _type = _INDICATOR;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("T3::T3: error on TA_Initialize");
}

int T3::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();

  int period = settings->getInt(_PERIOD);
  double vfactor = settings->getDouble(_VFACTOR);

  InputType it;
  Curve *in = it.input(bd, settings->data(_INPUT));
  if (! in)
    return 1;

  // create bars
  Curve *bars = it.ohlc(bd,
			QColor(settings->data(_COLOR_BARS_UP)),
			QColor(settings->data(_COLOR_BARS_DOWN)),
			QColor(settings->data(_COLOR_BARS_NEUTRAL)));
  if (settings->data(_STYLE_BARS) == "OHLC")
    bars->setType(Curve::OHLC);
  else
    bars->setType(Curve::Candle);
  bars->setLabel("BARS");
  bars->setZ(0);
  i->setLine("BARS", bars);

  QList<int> keys;
  in->keys(keys);
  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_T3(0,
                        size - 1,
                        &input[0],
                        period,
                        vfactor,
                        &outBeg,
                        &outNb,
                        &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::calculate: TA-Lib error" << rc;
    return 1;
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

  line->setAllColor(QColor(settings->data(_COLOR)));
  line->setLabel(settings->data(_LABEL));
  line->setType((Curve::Type) line->typeFromString(settings->data(_STYLE)));
  line->setZ(1);
  i->setLine(settings->data(_LABEL), line);

  return 0;
}

int T3::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // PERIOD
  // VFACTOR

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

  double vfactor = command->parm("VFACTOR").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid VFACTOR" << command->parm("VFACTOR");
    return 1;
  }

  if (in->count() < period)
    return 1;

  QList<int> keys;
  in->keys(keys);
  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_T3(0,
                        size - 1,
                        &input[0],
                        period,
                        vfactor,
                        &outBeg,
                        &outNb,
                        &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::command: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

void T3::dialog (QWidget *p, Indicator *i)
{
  T3Dialog *dialog = new T3Dialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void T3::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_COLOR, "yellow");
  set->setData(_LABEL, _plugin);
  set->setData(_STYLE, "Line");
  set->setData(_PERIOD, 5);
  set->setData(_VFACTOR, 0.7);
  set->setData(_INPUT, "Close");
  set->setData(_STYLE_BARS, "OHLC");
  set->setData(_COLOR_BARS_UP, "green");
  set->setData(_COLOR_BARS_DOWN, "red");
  set->setData(_COLOR_BARS_NEUTRAL, "dimgray");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  T3 *o = new T3;
  return ((Plugin *) o);
}
