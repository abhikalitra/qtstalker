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
#include "HT_SINEWidget.h"
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

  QList<Curve *> list;
  list << in;
  QList<Curve *> lines = getSINE(list);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return 1;
  }

  Curve *sline = lines.at(0);
  sline->setLabel(sname);
  i->setLine(sname, sline);

  Curve *lline = lines.at(1);
  lline->setLabel(lname);
  i->setLine(lname, lline);

  command->setReturnCode("0");

  return 0;
}

QList<Curve *> HT_SINE::getSINE (QList<Curve *> &list)
{
  QList<Curve *> lines;
  if (! list.count())
    return lines;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &input[0], &input[0], &input[0], &input[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_HT_SINE(0,
                             size - 1,
                             &input[0],
                             &outBeg,
                             &outNb,
                             &out[0],
                             &out2[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getSINE: TA-Lib error" << rc;
    return lines;
  }

  Curve *c = new Curve;
  lines.append(c);
  c = new Curve;
  lines.append(c);
  if (it.outputs(lines, keys, outNb, &out[0], &out2[0], &out2[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}

PluginWidget * HT_SINE::dialog (QWidget *p)
{
  return new HT_SINEWidget(p);
}

void HT_SINE::defaults (QString &d)
{
  QStringList l;
  l << "PLUGIN=" + _plugin;
  l << "NAME_SINE=SINE";
  l << "NAME_LEAD=LEAD";
  l << "INPUT=Close";
  d = l.join(",");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  HT_SINE *o = new HT_SINE;
  return ((Plugin *) o);
}
