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

#include "FI.h"
#include "Curve.h"
#include "Globals.h"
#include "FIDialog.h"
#include "MAType.h"

#include <QtDebug>

FI::FI ()
{
  _plugin = "FI";
  _type = "INDICATOR";
}

int FI::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  int period = settings->getInt("SMOOTHING");

  MAType mat;
  int type = mat.fromString(settings->data("SMOOTHING_TYPE"));

  Curve *line = new Curve;

  int loop = 1;
  double force = 0;
  for (; loop < bd->count(); loop++)
  {
    Bar *bar = bd->bar(loop);
    if (! bar)
      continue;

    Bar *ybar = bd->bar(loop - 1);
    if (! ybar)
      continue;

    double cdiff = bar->close() - ybar->close();
    force = bar->volume() * cdiff;

    line->setBar(loop, new CurveBar(force));
  }

  if (period > 1)
  {
    Curve *ma = mat.getMA(line, period, type);
    if (! ma)
    {
      delete line;
      return 1;
    }

    delete line;
    line = ma;
  }

  line->setAllColor(QColor(settings->data("COLOR")));
  line->setLabel(settings->data("OUTPUT"));
  line->setType(settings->data("STYLE"));
  line->setZ(settings->getInt("Z"));
  i->setLine(settings->data("OUTPUT"), line);

  return 0;
}

int FI::command (Command *command)
{
  // PARMS:
  // INPUT_CLOSE
  // INPUT_VOLUME
  // NAME
  // PERIOD
  // MA_TYPE

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *iclose = i->line(command->parm("INPUT_CLOSE"));
  if (! iclose)
  {
    qDebug() << _plugin << "::command: invalid INPUT_CLOSE" << command->parm("INPUT_CLOSE");
    return 1;
  }

  Curve *ivol = i->line(command->parm("INPUT_VOLUME"));
  if (! ivol)
  {
    qDebug() << _plugin << "::command: invalid INPUT_VOLUME" << command->parm("INPUT_VOLUME");
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid period" << command->parm("PERIOD");
    return 1;
  }

  MAType mat;
  int type = mat.fromString(command->parm("MA_TYPE"));
  if (type == -1)
  {
    qDebug() << _plugin << "::command: invalid ma type" << command->parm("MA_TYPE");
    return 1;
  }

  line = new Curve;

  int ipos = 0;
  int end = 0;
  iclose->keyRange(ipos, end);
  ipos++;
  double force = 0;
  for (; ipos <= end; ipos++)
  {
    CurveBar *cbar = iclose->bar(ipos);
    if (! cbar)
      continue;

    CurveBar *cybar = iclose->bar(ipos - 1);
    if (! cybar)
      continue;

    CurveBar *vbar = ivol->bar(ipos);
    if (! vbar)
      continue;

    double cdiff = cbar->data() - cybar->data();
    force = vbar->data() * cdiff;

    line->setBar(ipos, new CurveBar(force));
  }

  if (period > 1)
  {
    Curve *ma = mat.getMA(line, period, type);
    if (! ma)
    {
      delete line;
      return 1;
    }

    delete line;
    line = ma;
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

QWidget * FI::dialog (QWidget *p, Setting *set)
{
  return new FIDialog(p, set);
}

void FI::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", QString("yellow"));
  set->setData("STYLE", QString("HistogramBar"));
  set->setData("SMOOTHING", 2);
  set->setData("SMOOTHING_TYPE", QString("EMA"));
  set->setData("OUTPUT", _plugin);
  set->setData("Z", 0);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  FI *o = new FI;
  return ((Plugin *) o);
}
