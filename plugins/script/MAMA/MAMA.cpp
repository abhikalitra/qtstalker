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

#include "MAMA.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "MAMADialog.h"
#include "InputType.h"

#include <QtDebug>

MAMA::MAMA ()
{
  _plugin = "MAMA";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("MAMA::MAMA: error on TA_Initialize");
}

int MAMA::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  Curve *mama = i->line(settings->data("OUTPUT_MAMA"));
  if (mama)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT_MAMA" << settings->data("OUTPUT_MAMA");
    return 1;
  }

  Curve *fama = i->line(settings->data("OUTPUT_FAMA"));
  if (fama)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT_FAMA" << settings->data("OUTPUT_FAMA");
    return 1;
  }

  double fastLimit = settings->getDouble("LIMIT_FAST");
  double slowLimit = settings->getDouble("LIMIT_SLOW");

  InputType it;
  QStringList order;
  order << settings->data("INPUT");
  QList<Curve *> list;
  if (it.inputs(list, order, i, bd))
  {
    qDebug() << _plugin << "::calculate: input missing";
    return 1;
  }

  QList<Curve *> lines = getMAMA(list, fastLimit, slowLimit);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return 1;
  }

  mama = lines.at(0);
  mama->setAllColor(QColor(settings->data("COLOR_MAMA")));
  mama->setLabel(settings->data("OUTPUT_MAMA"));
  mama->setType(settings->data("STYLE_MAMA"));
  mama->setZ(settings->getInt("Z_MAMA"));
  i->setLine(settings->data("OUTPUT_MAMA"), mama);
  
  fama = lines.at(1);
  fama->setAllColor(QColor(settings->data("COLOR_FAMA")));
  fama->setLabel(settings->data("OUTPUT_FAMA"));
  fama->setType(settings->data("STYLE_FAMA"));
  fama->setZ(settings->getInt("Z_FAMA"));
  i->setLine(settings->data("OUTPUT_FAMA"), fama);

  return 0;
}

int MAMA::command (Command *command)
{
  // PARMS:
  // INPUT
  // NAME_MAMA
  // NAME_FAMA
  // LIMIT_FAST
  // LIMIT_SLOW

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

  QString mname = command->parm("NAME_MAMA");
  Curve *line = i->line(mname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_MAMA" << mname;
    return 1;
  }

  QString fname = command->parm("NAME_FAMA");
  line = i->line(fname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_FAMA" << fname;
    return 1;
  }

  bool ok;
  double flimit = command->parm("LIMIT_FAST").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid LIMIT_FAST" << command->parm("LIMIT_FAST");
    return 1;
  }

  double slimit = command->parm("LIMIT_SLOW").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid LIMIT_SLOW" << command->parm("LIMIT_FAST");
    return 1;
  }

  QList<Curve *> list;
  list << in;
  QList<Curve *> lines = getMAMA(list, flimit, slimit);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return 1;
  }

  Curve *mama = lines.at(0);
  mama->setLabel(mname);
  i->setLine(mname, mama);

  Curve *fama = lines.at(1);
  fama->setLabel(fname);
  i->setLine(fname, fama);

  command->setReturnCode("0");

  return 0;
}

QList<Curve *> MAMA::getMAMA (QList<Curve *> &list, double flimit, double slimit)
{
  QList<Curve *> lines;
  if (! list.count())
    return lines;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real in[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &in[0], &in[0], &in[0], &in[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_MAMA(0,
                          size - 1,
                          &in[0],
                          flimit,
                          slimit,
                          &outBeg,
                          &outNb,
                          &out[0],
                          &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getMAMA: TA-Lib error" << rc;
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

QWidget * MAMA::dialog (QWidget *p, Setting *set)
{
  return new MAMADialog(p, set);
}

void MAMA::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("INPUT", QString("Close"));
  set->setData("COLOR_MAMA", QString("red"));
  set->setData("COLOR_FAMA", QString("yellow"));
  set->setData("STYLE_MAMA", QString("Line"));
  set->setData("STYLE_FAMA", QString("Line"));
  set->setData("LIMIT_FAST", 0.5);
  set->setData("LIMIT_SLOW", 0.05);
  set->setData("Z_MAMA", 0);
  set->setData("OUTPUT_MAMA", _plugin);
  set->setData("Z_FAMA", 0);
  set->setData("OUTPUT_FAMA", QString("FAMA"));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  MAMA *o = new MAMA;
  return ((Plugin *) o);
}
