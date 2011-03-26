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

#include "HT_PHASOR.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "HT_PHASORDialog.h"
#include "InputType.h"

#include <QtDebug>

HT_PHASOR::HT_PHASOR ()
{
  _plugin = "HT_PHASOR";
  _type = _INDICATOR;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("HT_PHASOR::HT_PHASOR: error on TA_Initialize");
}

int HT_PHASOR::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();

  InputType itypes;
  Curve *in = itypes.input(bd, settings->data(_INPUT));
  if (! in)
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

  TA_RetCode rc = TA_HT_PHASOR (0,
                                size - 1,
                                &input[0],
                                &outBeg,
                                &outNb,
                                &out[0],
                                &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  Curve *pline = new Curve;
  Curve *qline = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    pline->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    qline->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));

    keyLoop--;
    outLoop--;
  }

  pline->setAllColor(QColor(settings->data(_COLOR_PHASE)));
  pline->setLabel(settings->data(_LABEL_PHASE));
  pline->setType((Curve::Type) pline->typeFromString(settings->data(_STYLE_PHASE)));
  pline->setZ(0);
  i->setLine(settings->data(_LABEL_PHASE), pline);
  
  qline->setAllColor(QColor(settings->data(_COLOR_QUAD)));
  qline->setLabel(settings->data(_LABEL_QUAD));
  qline->setType((Curve::Type) qline->typeFromString(settings->data(_STYLE_QUAD)));
  qline->setZ(1);
  i->setLine(settings->data(_LABEL_QUAD), qline);

  return 0;
}

int HT_PHASOR::command (Command *command)
{
  // PARMS:
  // INPUT
  // NAME_PHASE
  // NAME_QUAD

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

  QString pname = command->parm("NAME_PHASE");
  Curve *line = i->line(pname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_PHASE" << pname;
    return 1;
  }

  QString qname = command->parm("NAME_QUAD");
  line = i->line(qname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_QUAD" << qname;
    return 1;
  }

  if (in->count() < 1)
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

  TA_RetCode rc = TA_HT_PHASOR (0,
                                size - 1,
                                &input[0],
                                &outBeg,
                                &outNb,
                                &out[0],
                                &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::command: TA-Lib error" << rc;
    return 1;
  }

  Curve *pline = new Curve;
  Curve *qline = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    pline->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    qline->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));

    keyLoop--;
    outLoop--;
  }

  pline->setLabel(pname);
  qline->setLabel(qname);

  i->setLine(pname, pline);
  i->setLine(qname, qline);

  command->setReturnCode("0");

  return 0;
}

void HT_PHASOR::dialog (QWidget *p, Indicator *i)
{
  HT_PHASORDialog *dialog = new HT_PHASORDialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void HT_PHASOR::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_COLOR_PHASE, "red");
  set->setData(_COLOR_QUAD, "yellow");
  set->setData(_STYLE_PHASE, "Line");
  set->setData(_STYLE_QUAD, "Line");
  set->setData(_LABEL_PHASE, "PHASE");
  set->setData(_LABEL_QUAD, "QUAD");
  set->setData(_INPUT, "Close");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  HT_PHASOR *o = new HT_PHASOR;
  return ((Plugin *) o);
}
