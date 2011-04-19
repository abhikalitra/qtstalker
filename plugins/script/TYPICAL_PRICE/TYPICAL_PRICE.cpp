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

#include "TYPICAL_PRICE.h"
#include "Curve.h"
#include "Globals.h"
#include "TYPICAL_PRICEWidget.h"
#include "InputType.h"

#include <QtDebug>

TYPICAL_PRICE::TYPICAL_PRICE ()
{
  _plugin = "TYPICAL_PRICE";
  _type = "INDICATOR";
}

int TYPICAL_PRICE::command (Command *command)
{
  // PARMS:
  // NAME
  // HIGH
  // LOW
  // CLOSE

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

  Curve *high = i->line(command->parm("HIGH"));
  if (! high)
  {
    qDebug() << _plugin << "::command: invalid HIGH" << command->parm("HIGH");
    return 1;
  }

  Curve *low = i->line(command->parm("LOW"));
  if (! low)
  {
    qDebug() << _plugin << "::command: invalid LOW" << command->parm("LOW");
    return 1;
  }

  Curve *close = i->line(command->parm("CLOSE"));
  if (! close)
  {
    qDebug() << _plugin << "::command: invalid CLOSE" << command->parm("CLOSE");
    return 1;
  }

  InputType it;
  QList<int> keys;
  QList<Curve *> list;
  list << high << low << close;
  it.keys(list, keys);
  
  line = new Curve;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *h = high->bar(keys.at(loop));
    if (! h)
      continue;

    CurveBar *l = low->bar(keys.at(loop));
    if (! l)
      continue;

    CurveBar *c = close->bar(keys.at(loop));
    if (! c)
      continue;
    
    double t = (h->data() + l->data() + c->data()) / 3.0;
    line->setBar(keys.at(loop), new CurveBar(t));
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

PluginWidget * TYPICAL_PRICE::dialog (QWidget *p)
{
  return new TYPICAL_PRICEWidget(p);
}

void TYPICAL_PRICE::defaults (QString &d)
{
  QStringList l;
  l << "PLUGIN=" + _plugin;
  l << "NAME=TP";
  l << "INPUT_HIGH=High";
  l << "INPUT_LOW=Low";
  l << "INPUT_CLOSE=Close";
  d = l.join(",");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  TYPICAL_PRICE *o = new TYPICAL_PRICE;
  return ((Plugin *) o);
}
