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

#include "BETA.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "BETAWidget.h"
#include "InputType.h"
#include "QuoteDataBase.h"

#include <QtDebug>

BETA::BETA ()
{
  _plugin = "BETA";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("BETA::BETA: error on TA_Initialize");
}

int BETA::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // INDEX
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

  Curve *in2 = i->line(command->parm("INDEX"));
  if (! in2)
  {
    BarData *bd = command->barData();
    if (! bd)
    {
      qDebug() << _plugin << "::command: no bardata";
      return 1;
    }
    
    BarData tbd;
    tbd.setKey(command->parm("INDEX"));
    tbd.setRange(bd->range());
    tbd.setBarLength(bd->barLength());
    tbd.setStartDate(bd->startDate());
    tbd.setEndDate(bd->endDate());

    QuoteDataBase db;
    if (db.getBars(&tbd))
      return 1;

    InputType it;
    in2 = it.input(&tbd, "Close");
    if (! in2)
    {
      qDebug() << _plugin << "::command: INDEX missing" << command->parm("INDEX");
      return 1;
    }
  }

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD" << command->parm("PERIOD");
    return 1;
  }

  QList<Curve *> list;
  list << in << in2;
  line = getBETA(list, period);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * BETA::getBETA (QList<Curve *> &list, int period)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &input[0], &input2[0], &input2[0], &input2[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_BETA(0,
                          size - 1,
                          &input[0],
                          &input2[0],
                          period,
                          &outBeg,
                          &outNb,
                          &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getBETA: TA-Lib error" << rc;
    return 0;
  }

  QList<Curve *> outs;
  Curve *c = new Curve;
  outs.append(c);
  if (it.outputs(outs, keys, outNb, &out[0], &out[0], &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}

PluginWidget * BETA::dialog (QWidget *p)
{
  return new BETAWidget(p);
}

void BETA::defaults (QString &d)
{
  QStringList l;
  l << "PLUGIN=" + _plugin;
  l << "NAME=" + _plugin;
  l << "INPUT=Close";
  l << "INDEX=YAHOO:SPY";
  l << "PERIOD=5";
  d = l.join(",");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  BETA *o = new BETA;
  return ((Plugin *) o);
}
