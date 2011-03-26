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

#include "LINEARREG.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "LINEARREGDialog.h"
#include "InputType.h"

#include <QtDebug>

LINEARREG::LINEARREG ()
{
  _plugin = "LINEARREG";
  _method << "LINEARREG" << "ANGLE" << "INTERCEPT" "SLOPE" << "TSF";
  _type = _INDICATOR;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("LINEARREG::LINEARREG: error on TA_Initialize");
}

int LINEARREG::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();

  int period = settings->getInt(_PERIOD);

  int method = _method.indexOf(settings->data(_METHOD));

  // create bars
  InputType itypes;
  Curve *bars = itypes.ohlc(bd,
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

  Curve *in = itypes.input(bd, settings->data(_INPUT));
  if (! in)
    return 1;
  
  int size = in->count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  delete in;

  TA_RetCode rc = TA_SUCCESS;
  switch ((Method) method)
  {
    case _LINEARREG:
      rc = TA_LINEARREG(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _ANGLE:
      rc = TA_LINEARREG_ANGLE(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _INTERCEPT:
      rc = TA_LINEARREG_INTERCEPT(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _SLOPE:
      rc = TA_LINEARREG_SLOPE(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _TSF:
      rc = TA_TSF(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

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

int LINEARREG::command (Command *command)
{
  // PARMS:
  // METHOD
  // NAME
  // INPUT
  // PERIOD

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  int method = _method.indexOf(command->parm("METHOD"));
  if (method == -1)
  {
    qDebug() << _plugin << "::command: invalid METHOD" << command->parm("METHOD");
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

  if (in->count() < period)
    return 1;

  int size = in->count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_SUCCESS;
  switch ((Method) method)
  {
    case _LINEARREG:
      rc = TA_LINEARREG(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _ANGLE:
      rc = TA_LINEARREG_ANGLE(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _INTERCEPT:
      rc = TA_LINEARREG_INTERCEPT(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _SLOPE:
      rc = TA_LINEARREG_SLOPE(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _TSF:
      rc = TA_TSF(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

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

void LINEARREG::dialog (QWidget *p, Indicator *i)
{
  LINEARREGDialog *dialog = new LINEARREGDialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void LINEARREG::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_COLOR, "yellow");
  set->setData(_LABEL, _plugin);
  set->setData(_STYLE, "Line");
  set->setData(_METHOD, "LINEARREG");
  set->setData(_INPUT, "Close");
  set->setData(_STYLE_BARS, QString("OHLC"));
  set->setData(_COLOR_BARS_UP, QString("green"));
  set->setData(_COLOR_BARS_DOWN, QString("red"));
  set->setData(_COLOR_BARS_NEUTRAL, QString("dimgray"));
  set->setData(_PERIOD, 14);
}

QStringList LINEARREG::method ()
{
  return _method;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  LINEARREG *o = new LINEARREG;
  return ((Plugin *) o);
}
