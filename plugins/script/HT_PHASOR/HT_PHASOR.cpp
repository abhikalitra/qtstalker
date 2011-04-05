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
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("HT_PHASOR::HT_PHASOR: error on TA_Initialize");
}

int HT_PHASOR::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  int delFlag = FALSE;
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

    delFlag++;
  }

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

  if (delFlag)
    delete in;

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

  pline->setAllColor(QColor(settings->data("COLOR_PHASE")));
  pline->setLabel(settings->data("OUTPUT_PHASE"));
  pline->setType(settings->data("STYLE_PHASE"));
  pline->setZ(settings->getInt("Z_PHASE"));
  i->setLine(settings->data("OUTPUT_PHASE"), pline);
  
  qline->setAllColor(QColor(settings->data("COLOR_QUAD")));
  qline->setLabel(settings->data("OUTPUT_QUAD"));
  qline->setType(settings->data("STYLE_QUAD"));
  qline->setZ(settings->getInt("Z_QUAD"));
  i->setLine(settings->data("OUTPUT_QUAD"), qline);

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

QWidget * HT_PHASOR::dialog (QWidget *p, Setting *set)
{
  return new HT_PHASORDialog(p, set);
}

void HT_PHASOR::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR_PHASE", QString("red"));
  set->setData("COLOR_QUAD", QString("yellow"));
  set->setData("STYLE_PHASE", QString("Line"));
  set->setData("STYLE_QUAD", QString("Line"));
  set->setData("INPUT", QString("Close"));
  set->setData("OUTPUT_PHASE", _plugin);
  set->setData("Z_PHASE", 0);
  set->setData("OUTPUT_QUAD", _plugin);
  set->setData("Z_QUAD", 0);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  HT_PHASOR *o = new HT_PHASOR;
  return ((Plugin *) o);
}
