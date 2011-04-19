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

#include "ARITHMETIC.h"
#include "Curve.h"
#include "Globals.h"
#include "InputType.h"
#include "ARITHMETICWidget.h"

#include <QtDebug>

ARITHMETIC::ARITHMETIC ()
{
  _plugin = "ARITHMETIC";
  _type = "INDICATOR";
}

int ARITHMETIC::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // INPUT2
  // METHOD

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
    bool ok;
    double value = command->parm("INPUT2").toDouble(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid INPUT2" << command->parm("INPUT2");
      return 1;
    }

    in2 = new Curve;
    QList<int> keys;
    in->keys(keys);
    int loop2 = 0;
    for (; loop2 < keys.count(); loop2++)
      in2->setBar(keys.at(loop2), new CurveBar(value));
    in2->setLabel(QString::number(value));
    i->setLine(QString::number(value), in2);
  }

  QStringList l;
  l << "ADD" << "DIV" << "MULT" << "SUB";
  int method = l.indexOf(command->parm("METHOD"));
  if (method == -1)
  {
    qDebug() << _plugin << "::command: invalid METHOD" << command->parm("METHOD");
    return 1;
  }

  line = getARITHMETIC(in, in2, method);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * ARITHMETIC::getARITHMETIC (Curve *in, Curve *in2, int method)
{
  QList<int> keys;
  int size = in->count();
  in->keys(keys);

  if (in2->count() < size)
  {
    size = in2->count();
    in2->keys(keys);
  }

  Curve *line = new Curve;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    CurveBar *bar2 = in2->bar(keys.at(loop));
    if (! bar2)
      continue;

    switch (method)
    {
      case 0: // add
        line->setBar(keys.at(loop), new CurveBar(bar->data() + bar2->data()));
	break;
      case 1: // div
        line->setBar(keys.at(loop), new CurveBar(bar->data() / bar2->data()));
	break;
      case 2: // mult
        line->setBar(keys.at(loop), new CurveBar(bar->data() * bar2->data()));
	break;
      case 3: // sub
        line->setBar(keys.at(loop), new CurveBar(bar->data() - bar2->data()));
	break;
      default:
	break;
    }
  }

  return line;
}

PluginWidget * ARITHMETIC::dialog (QWidget *p)
{
  return new ARITHMETICWidget(p);
}

void ARITHMETIC::defaults (QString &d)
{
  QStringList l;
  l << "PLUGIN=" + _plugin;
  l << "NAME=" + _plugin;
  l << "INPUT=Close";
  l << "INPUT2=Close";
  l << "METHOD=ADD";
  d = l.join(",");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  ARITHMETIC *o = new ARITHMETIC;
  return ((Plugin *) o);
}
