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

#include "MEDIAN_PRICE.h"
#include "Curve.h"
#include "Globals.h"
#include "InputType.h"
#include "MEDIAN_PRICEDialog.h"

#include <QtDebug>

MEDIAN_PRICE::MEDIAN_PRICE ()
{
  _plugin = "MEDIAN_PRICE";
  _type = _INDICATOR;
}

int MEDIAN_PRICE::calculate (BarData *bd, Indicator *i, Setting *settings)
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

  int delFlag2 = FALSE;
  Curve *in2 = i->line(settings->data("INPUT2"));
  if (! in2)
  {
    InputType it;
    in2 = it.input(bd, settings->data("INPUT2"));
    if (! in2)
    {
      qDebug() << _plugin << "::calculate: no input" << settings->data("INPUT2");
      if (delFlag)
	delete in;
      return 1;
    }

    delFlag2++;
  }
  
  Curve *line = new Curve;
  QList<int> keys;
  int size = in->count();
  if (in2->count() > size)
  {
    size = in2->count();
    in2->keys(keys);
  }
  else
    in->keys(keys);

  line = new Curve;
  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    CurveBar *bar2 = in2->bar(keys.at(loop));
    if (! bar2)
      continue;

    double t = (bar->data() + bar2->data()) / 2.0;
    line->setBar(keys.at(loop), new CurveBar(t));
  }

  line->setAllColor(QColor(settings->data("COLOR")));
  line->setLabel(settings->data("OUTPUT"));
  line->setType((Curve::Type) line->typeFromString(settings->data("STYLE")));
  line->setZ(settings->getInt("Z"));
  i->setLine(settings->data("OUTPUT"), line);

  return 0;
}

int MEDIAN_PRICE::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // INPUT2

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
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::command: INPUT missing" << command->parm("INPUT");
    return 1;
  }

  Curve *in2 = i->line(command->parm("INPUT2"));
  if (! in2)
  {
    qDebug() << _plugin << "::command: INPUT2 missing" << command->parm("INPUT2");
    return 1;
  }

  QList<int> keys;
  int size = in->count();
  if (in2->count() > size)
  {
    size = in2->count();
    in2->keys(keys);
  }
  else
    in->keys(keys);

  line = new Curve;
  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    CurveBar *bar2 = in2->bar(keys.at(loop));
    if (! bar2)
      continue;

    double t = (bar->data() + bar2->data()) / 2.0;
    line->setBar(keys.at(loop), new CurveBar(t));
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

QWidget * MEDIAN_PRICE::dialog (QWidget *p, Setting *set)
{
  return new MEDIAN_PRICEDialog(p, set);
}

void MEDIAN_PRICE::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("red"));
  set->setData("STYLE", QString("Line"));
  set->setData("INPUT", QString("High"));
  set->setData("INPUT2", QString("Low"));
  set->setData("OUTPUT", _plugin);
  set->setData("Z", 0);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  MEDIAN_PRICE *o = new MEDIAN_PRICE;
  return ((Plugin *) o);
}
