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

#include "PO.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "PODialog.h"
#include "InputType.h"
#include "MAType.h"

#include <QtDebug>

PO::PO ()
{
  _plugin = "PO";
  _method << "APO" << "PPO";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("PO::PO: error on TA_Initialize");
}

int PO::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  Curve *line = i->line(settings->data("OUTPUT"));
  if (line)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT" << settings->data("OUTPUT");
    return 1;
  }

  int fast = settings->getInt("PERIOD_FAST");
  int slow = settings->getInt("PERIOD_SLOW");

  MAType mat;
  int type = mat.fromString(settings->data("MA_TYPE"));

  int method = _method.indexOf(settings->data("METHOD"));

  InputType it;
  QStringList order;
  order << settings->data("INPUT");
  QList<Curve *> list;
  if (it.inputs(list, order, i, bd))
  {
    qDebug() << _plugin << "::calculate: input missing";
    return 1;
  }

  line = getPO(list, fast, slow, type, method);
  if (! line)
    return 1;

  line->setAllColor(QColor(settings->data("COLOR")));
  line->setLabel(settings->data("OUTPUT"));
  line->setType(settings->data("STYLE"));
  line->setZ(settings->getInt("Z"));
  i->setLine(settings->data("OUTPUT"), line);

  return 0;
}

int PO::command (Command *command)
{
  // PARMS:
  // METHOD
  // NAME
  // INPUT
  // PERIOD_FAST
  // PERIOD_SLOW
  // MA_TYPE

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  int method = _method.indexOf(command->parm("METHOD"));
  if (method == -1)
  {
    qDebug() << _plugin << "::command: invalid METHOD" << command->parm("METHOD");
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
  int fast = command->parm("PERIOD_FAST").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_FAST" << command->parm("PERIOD_FAST");
    return 1;
  }

  int slow = command->parm("PERIOD_SLOW").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_SLOW" << command->parm("PERIOD_SLOW");
    return 1;
  }

  MAType mat;
  int type = mat.fromString(command->parm("MA_TYPE"));
  if (type == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE" << command->parm("MA_TYPE");
    return 1;
  }

  QList<Curve *> list;
  list << in;
  line = getPO(list, fast, slow, type, method);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * PO::getPO (QList<Curve *> &list, int fast, int slow, int type, int method)
{
  if (! list.count())
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real in[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &in[0], &in[0], &in[0], &in[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SUCCESS;
  switch ((Method) method)
  {
    case _APO:
      rc = TA_APO(0, size - 1, &in[0], fast, slow, (TA_MAType) type, &outBeg, &outNb, &out[0]);
      break;
    case _PPO:
      rc = TA_PPO(0, size - 1, &in[0], fast, slow, (TA_MAType) type, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getMOM: TA-Lib error" << rc;
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

QWidget * PO::dialog (QWidget *p, Setting *set)
{
  return new PODialog(p, set);
}

void PO::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("yellow"));
  set->setData("STYLE", QString("HistogramBar"));
  set->setData("PERIOD_FAST", 12);
  set->setData("PERIOD_SLOW", 26);
  set->setData("MA_TYPE", QString("EMA"));
  set->setData("INPUT", QString("Close"));
  set->setData("METHOD", QString("APO"));
  set->setData("Z", 0);
  set->setData("OUTPUT", _plugin);
}

QStringList PO::method ()
{
  return _method;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  PO *o = new PO;
  return ((Plugin *) o);
}
