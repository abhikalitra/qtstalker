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

#include "MAMA.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "MAMADialog.h"
#include "InputType.h"

#include <QtDebug>

MAMA::MAMA ()
{
  _plugin = "MAMA";
  _type = _INDICATOR;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("MAMA::MAMA: error on TA_Initialize");
}

int MAMA::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();

  double fastLimit = settings->getDouble(_LIMIT_FAST);
  double slowLimit = settings->getDouble(_LIMIT_SLOW);

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

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  delete in;

  TA_RetCode rc = TA_MAMA(0,
                          size - 1,
                          &input[0],
                          fastLimit,
                          slowLimit,
                          &outBeg,
                          &outNb,
                          &out[0],
                          &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  Curve *mama = new Curve;
  Curve *fama = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    mama->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    fama->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));

    keyLoop--;
    outLoop--;
  }

  mama->setAllColor(QColor(settings->data(_COLOR_MAMA)));
  mama->setLabel(settings->data(_LABEL_MAMA));
  mama->setType((Curve::Type) mama->typeFromString(settings->data(_STYLE_MAMA)));
  mama->setZ(1);
  i->setLine(settings->data(_LABEL_MAMA), mama);
  
  fama->setAllColor(QColor(settings->data(_COLOR_FAMA)));
  fama->setLabel(settings->data(_LABEL_FAMA));
  fama->setType((Curve::Type) fama->typeFromString(settings->data(_STYLE_FAMA)));
  fama->setZ(2);
  i->setLine(settings->data(_LABEL_FAMA), fama);

  return 0;
}

int MAMA::command (Command *command)
{
  // PARMS:
  // INPUT
  // NAME_MAMA
  // NAME_FAMA
  // LIMIT_FAST
  // LIMIT_SLOW

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

  QString mname = command->parm("NAME_MAMA");
  Curve *line = i->line(mname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_MAMA" << mname;
    return 1;
  }

  QString fname = command->parm("NAME_FAMA");
  line = i->line(fname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_FAMA" << fname;
    return 1;
  }

  bool ok;
  double flimit = command->parm("LIMIT_FAST").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid LIMIT_FAST" << command->parm("LIMIT_FAST");
    return 1;
  }

  double slimit = command->parm("LIMIT_SLOW").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid LIMIT_SLOW" << command->parm("LIMIT_FAST");
    return 1;
  }

  if (in->count() < flimit || in->count() < slimit)
    return 1;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_MAMA(0,
                          size - 1,
                          &input[0],
                          flimit,
                          slimit,
                          &outBeg,
                          &outNb,
                          &out[0],
                          &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::command: TA-Lib error" << rc;
    return 1;
  }

  Curve *mama = new Curve;
  Curve *fama = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    mama->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    fama->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));

    keyLoop--;
    outLoop--;
  }

  mama->setLabel(mname);
  fama->setLabel(fname);

  i->setLine(mname, mama);
  i->setLine(fname, fama);

  command->setReturnCode("0");

  return 0;
}

void MAMA::dialog (QWidget *p, Indicator *i)
{
  MAMADialog *dialog = new MAMADialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void MAMA::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_INPUT, "Close");
  set->setData(_COLOR_MAMA, "red");
  set->setData(_COLOR_FAMA, "yellow");
  set->setData(_LABEL_MAMA, "MAMA");
  set->setData(_LABEL_FAMA, "FAMA");
  set->setData(_STYLE_MAMA, "Line");
  set->setData(_STYLE_FAMA, "Line");
  set->setData(_LIMIT_FAST, 0.5);
  set->setData(_LIMIT_SLOW, 0.05);
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
  MAMA *o = new MAMA;
  return ((Plugin *) o);
}
