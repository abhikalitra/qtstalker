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

#include "HT_PHASOR.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "HT_PHASORWidget.h"
#include "InputType.h"

#include <QtDebug>

HT_PHASOR::HT_PHASOR ()
{
  _plugin = "HT_PHASOR";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("HT_PHASOR::HT_PHASOR: error on TA_Initialize");
}

int HT_PHASOR::command (Command *command)
{
  // PARMS:
  // INPUT
  // NAME_PHASE
  // NAME_QUAD

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

  QString pname = command->parm("NAME_PHASE");
  Curve *line = i->line(pname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_PHASE" << pname;
    return 1;
  }

  QString qname = command->parm("NAME_QUAD");
  line = i->line(qname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_QUAD" << qname;
    return 1;
  }

  QList<Curve *> list;
  list << in;
  QList<Curve *> lines = getPHASOR(list);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return 1;
  }

  Curve *pline = lines.at(0);
  pline->setLabel(pname);
  i->setLine(pname, pline);
  
  Curve *qline = lines.at(1);
  qline->setLabel(qname);
  i->setLine(qname, qline);

  command->setReturnCode("0");

  return 0;
}

QList<Curve *> HT_PHASOR::getPHASOR (QList<Curve *> &list)
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

  TA_RetCode rc = TA_HT_PHASOR (0,
                                size - 1,
                                &input[0],
                                &outBeg,
                                &outNb,
                                &out[0],
                                &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getPHASOR: TA-Lib error" << rc;
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

PluginWidget * HT_PHASOR::dialog (QWidget *p)
{
  return new HT_PHASORWidget(p);
}

void HT_PHASOR::defaults (QString &d)
{
  QStringList l;
  l << "PLUGIN=" + _plugin;
  l << "NAME_PHASE=PHASE";
  l << "NAME_QUAD=QUAD";
  l << "INPUT=Close";
  d = l.join(",");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  HT_PHASOR *o = new HT_PHASOR;
  return ((Plugin *) o);
}
