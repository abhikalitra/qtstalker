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

#include "HT_SINE.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "HT_SINEDialog.h"
#include "InputType.h"

#include <QtDebug>

HT_SINE::HT_SINE ()
{
  _plugin = "HT_SINE";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("HT_SINE::HT_SINE: error on TA_Initialize");
}

int HT_SINE::calculate (BarData *bd, Indicator *i, Setting *settings)
{
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

  TA_RetCode rc = TA_HT_SINE (0,
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

  Curve *sline = new Curve;
  Curve *lline = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    sline->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    lline->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));

    keyLoop--;
    outLoop--;
  }

  sline->setAllColor(QColor(settings->data("COLOR_SINE")));
  sline->setLabel(settings->data("OUTPUT_SINE"));
  sline->setType(settings->data("STYLE_SINE"));
  sline->setZ(settings->getInt("Z_SINE"));
  i->setLine(settings->data("OUTPUT_SINE"), sline);
  
  lline->setAllColor(QColor(settings->data("COLOR_LEAD")));
  lline->setLabel(settings->data("OUTPUT_LEAD"));
  lline->setType(settings->data("STYLE_LEAD"));
  lline->setZ(settings->getInt("Z_LEAD"));
  i->setLine(settings->data("OUTPUT_LEAD"), lline);

  return 0;
}

int HT_SINE::command (Command *command)
{
  // PARMS:
  // INPUT
  // NAME_SINE
  // NAME_LEAD

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

  QString sname = command->parm("NAME_SINE");
  Curve *line = i->line(sname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_SINE" << sname;
    return 1;
  }

  QString lname = command->parm("NAME_LEAD");
  line = i->line(lname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_LEAD" << lname;
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

  TA_RetCode rc = TA_HT_SINE (0,
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

  Curve *sline = new Curve;
  Curve *lline = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    sline->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    lline->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));

    keyLoop--;
    outLoop--;
  }

  sline->setLabel(sname);
  lline->setLabel(lname);

  i->setLine(sname, sline);
  i->setLine(lname, lline);

  command->setReturnCode("0");

  return 0;
}

QWidget * HT_SINE::dialog (QWidget *p, Setting *set)
{
  return new HT_SINEDialog(p, set);
}

void HT_SINE::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR_SINE", QString("red"));
  set->setData("COLOR_LEAD", QString("yellow"));
  set->setData("STYLE_SINE", QString("Line"));
  set->setData("STYLE_LEAD", QString("Line"));
  set->setData("INPUT", QString("Close"));
  set->setData("OUTPUT_SINE", _plugin);
  set->setData("Z_SINE", 0);
  set->setData("OUTPUT_LEAD", _plugin);
  set->setData("Z_LEAD", 0);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  HT_SINE *o = new HT_SINE;
  return ((Plugin *) o);
}
