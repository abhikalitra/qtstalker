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

#include "DIV.h"
#include "Curve.h"
#include "Globals.h"
#include "DIVDialog.h"
#include "InputType.h"

#include <QtDebug>

DIV::DIV ()
{
  _plugin = "DIV";
  _type = _INDICATOR;
}

int DIV::calculate (BarData *bd, Indicator *i, Setting *settings)
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

  QList<int> keys;
  int size = in->count();
  if (in2->count() > size)
  {
    size = in2->count();
    in2->keys(keys);
  }
  else
    in->keys(keys);

  Curve *line = new Curve;
  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    CurveBar *bar2 = in2->bar(keys.at(loop));
    if (! bar2)
      continue;

    line->setBar(keys.at(loop), new CurveBar(bar->data() / bar2->data()));
  }

  if (delFlag)
    delete in;
  
  if (delFlag2)
    delete in2;

  line->setAllColor(QColor(settings->data("COLOR")));
  line->setLabel(settings->data("OUTPUT"));
  line->setType((Curve::Type) line->typeFromString(settings->data("STYLE")));
  line->setZ(settings->getInt("Z"));
  i->setLine(settings->data("OUTPUT"), line);

  return 0;
}

int DIV::command (Command *command)
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

  // verify NAME
  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  // verify INPUT
  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::command: INPUT not found" << command->parm("INPUT");
    return 1;
  }

  // verify INPUT2
  Curve *in2 = i->line(command->parm("INPUT2"));
  if (! in2)
  {
    qDebug() << _plugin << "::command: invalid INPUT2" << command->parm("INPUT2");
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

    line->setBar(keys.at(loop), new CurveBar(bar->data() / bar2->data()));
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

QWidget * DIV::dialog (QWidget *p, Setting *set)
{
  return new DIVDialog(p, set);
}

void DIV::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("red"));
  set->setData("STYLE", QString("Line"));
  set->setData("INPUT", QString("Close"));
  set->setData("INPUT2", QString("Close"));
  set->setData("OUTPUT", _plugin);
  set->setData("Z", 0);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  DIV *o = new DIV;
  return ((Plugin *) o);
}
