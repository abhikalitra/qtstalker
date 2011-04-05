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

#include "WILDER_MA.h"
#include "InputType.h"
#include "WILDER_MADialog.h"

#include <QtDebug>

WILDER_MA::WILDER_MA ()
{
  _plugin = "WILDER_MA";
  _type = "INDICATOR";
}

int WILDER_MA::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  int period = settings->getInt("PERIOD");

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

  Curve *ma = getMA(in, period);
  if (! ma)
  {
    if (delFlag)
      delete in;
    return 1;
  }

  if (delFlag)
    delete in;

  ma->setAllColor(QColor(settings->data("COLOR")));
  ma->setLabel(settings->data("OUTPUT"));
  ma->setType(settings->data("STYLE"));
  ma->setZ(settings->getInt("Z"));
  i->setLine(settings->data("OUTPUT"), ma);

  return 0;
}

int WILDER_MA::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
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

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD" << command->parm("PERIOD");
    return 1;
  }

  line = getMA(in, period);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * WILDER_MA::getMA (Curve *in, int period)
{
  if (in->count() < period)
    return 0;

  Curve *line = new Curve;

  QList<int> keys;
  in->keys(keys);

  double t = 0;
  int loop = 0;
  for (; loop < period; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    t += bar->data();
  }
  double yesterday = t / (double) period;
  line->setBar(keys.at(loop), new CurveBar(yesterday));

  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    double t  = (yesterday * (period - 1) + bar->data()) / (double) period;
    yesterday = t;

    line->setBar(keys.at(loop), new CurveBar(t));
  }

  return line;
}

QWidget * WILDER_MA::dialog (QWidget *p, Setting *set)
{
  return new WILDER_MADialog(p, set);
}

void WILDER_MA::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("yellow"));
  set->setData("STYLE", QString("Line"));
  set->setData("PERIOD", 10);
  set->setData("INPUT", QString("Close"));
  set->setData("Z", 0);
  set->setData("OUTPUT", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  WILDER_MA *o = new WILDER_MA;
  return ((Plugin *) o);
}
