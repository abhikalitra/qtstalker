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

#include "SAR.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "SARDialog.h"
#include "InputType.h"

#include <QtDebug>

SAR::SAR ()
{
  _plugin = "SAR";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("SAR::SAR: error on TA_Initialize");
}

int SAR::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  Curve *line = i->line(settings->data("OUTPUT"));
  if (line)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT" << settings->data("OUTPUT");
    return 1;
  }

  double init = settings->getDouble("STEP_INITIAL");
  double max = settings->getDouble("STEP_MAX");

  InputType it;
  QStringList order;
  order << "High" << "Low";
  QList<Curve *> list;
  if (it.inputs(list, order, i, bd))
  {
    qDebug() << _plugin << "::calculate: input missing";
    return 1;
  }

  line = getSAR(list, init, max);
  if (! line)
    return 1;

  line->setAllColor(QColor(settings->data("COLOR")));
  line->setLabel(settings->data("OUTPUT"));
  line->setZ(settings->getInt("Z"));
  i->setLine(settings->data("OUTPUT"), line);

  return 0;
}

int SAR::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // NAME
  // STEP_INITIAL
  // STEP_MAX

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

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  bool ok;
  double init = command->parm("STEP_INITIAL").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid STEP_INITIAL" << command->parm("STEP_INITIAL");
    return 1;
  }

  double max = command->parm("STEP_MAX").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid STEP_MAX" << command->parm("STEP_MAX");
    return 1;
  }

  QList<Curve *> list;
  list << ihigh << ilow;
  line = getSAR(list, init, max);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * SAR::getSAR (QList<Curve *> &list, double init, double max)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &high[0], &low[0], &low[0], &low[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SAR(0,
                         size - 1,
                         &high[0],
                         &low[0],
			 init,
			 max,
                         &outBeg,
                         &outNb,
                         &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getSAR: TA-Lib error" << rc;
    return 0;
  }

  QList<Curve *> outs;
  Curve *c = new Curve;
  c->setType("Dot");
  outs.append(c);
  if (it.outputs(outs, keys, outNb, &out[0], &out[0], &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}

QWidget * SAR::dialog (QWidget *p, Setting *set)
{
  return new SARDialog(p, set);
}

void SAR::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("white"));
  set->setData("STEP_INITIAL", 0.02);
  set->setData("STEP_MAX", 0.2);
  set->setData("Z", 0);
  set->setData("OUTPUT", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  SAR *o = new SAR;
  return ((Plugin *) o);
}
