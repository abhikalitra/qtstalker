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

#include "BOP.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "BOPDialog.h"
#include "InputType.h"

#include <QtDebug>

BOP::BOP ()
{
  _plugin = "BOP";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("BOP::BOP: error on TA_Initialize");
}

int BOP::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  Curve *line = i->line(settings->data("OUTPUT"));
  if (line)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT" << settings->data("OUTPUT");
    return 1;
  }

  InputType it;
  QStringList order;
  order << "Open" << "High" << "Low" << "Close";
  QList<Curve *> list;
  if (it.inputs(list, order, i, bd))
  {
    qDebug() << _plugin << "::calculate: input missing";
    return 1;
  }

  line = getBOP(list);
  if (! line)
    return 1;

  line->setAllColor(QColor(settings->data("COLOR")));
  line->setLabel(settings->data("OUTPUT"));
  line->setType(settings->data("STYLE"));
  line->setZ(settings->getInt("Z"));
  i->setLine(settings->data("OUTPUT"), line);

  return 0;
}

int BOP::command (Command *command)
{
  // PARMS:
  // INPUT_OPEN
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // NAME

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *iopen = i->line(command->parm("INPUT_OPEN"));
  if (! iopen)
  {
    qDebug() << _plugin << "::command: invalid INPUT_OPEN" << command->parm("INPUT_OPEN");
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

  Curve *iclose = i->line(command->parm("INPUT_CLOSE"));
  if (! iclose)
  {
    qDebug() << _plugin << "::command: invalid INPUT_CLOSE" << command->parm("INPUT_CLOSE");
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  QList<Curve *> list;
  list << iopen << ihigh << ilow << iclose;
  line = getBOP(list);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * BOP::getBOP (QList<Curve *> &list)
{
  if (list.count() != 4)
    return 0;
  
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;
  
  int size = keys.count();
  TA_Real out[size];
  TA_Real open[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &open[0], &high[0], &low[0], &close[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_BOP(0,
                         size - 1,
                         &open[0],
                         &high[0],
                         &low[0],
                         &close[0],
                         &outBeg,
                         &outNb,
                         &out[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getBOP: TA-Lib error" << rc;
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

QWidget * BOP::dialog (QWidget *p, Setting *set)
{
  return new BOPDialog(p, set);
}

void BOP::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("red"));
  set->setData("STYLE", QString("HistogramBar"));
  set->setData("OUTPUT", _plugin);
  set->setData("Z", 0);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  BOP *o = new BOP;
  return ((Plugin *) o);
}
