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

#include "CORREL.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "CORRELDialog.h"
#include "InputType.h"
#include "QuoteDataBase.h"

#include <QtDebug>

CORREL::CORREL ()
{
  _plugin = "CORREL";
  _type = _INDICATOR;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CORREL::CORREL: error on TA_Initialize");
}

int CORREL::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();

  int period = settings->getInt(_PERIOD);

  InputType it;
  Curve *in = it.input(bd, settings->data(_INPUT));
  if (! in)
    return 1;

  BarData tbd;
  tbd.setKey(settings->data(_INDEX));
  tbd.setRange(bd->range());
  tbd.setBarLength(bd->barLength());
  tbd.setStartDate(bd->startDate());
  tbd.setEndDate(bd->endDate());

  QuoteDataBase db;
  if (db.getBars(&tbd))
  {
    delete in;
    return 1;
  }

  Curve *in2 = it.input(&tbd, "Close");
  if (! in2)
  {
    delete in;
    return 1;
  }

  if (in->count() < period || in2->count() < period)
  {
    delete in;
    delete in2;
    return 1;
  }

  QList<int> keys;
  int size = in->count();
  if (in2->count() < size)
  {
    size = in2->count();
    in2->keys(keys);
  }
  else
    in->keys(keys);

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    CurveBar *bar2 = in2->bar(keys.at(loop));
    if (! bar2)
      continue;

    input[loop] = (TA_Real) bar->data();
    input2[loop] = (TA_Real) bar2->data();
  }

  delete in;
  delete in2;

  TA_RetCode rc = TA_CORREL(0,
                            size - 1,
                            &input[0],
                            &input2[0],
                            period,
                            &outBeg,
                            &outNb,
                            &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  Curve *line = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  line->setAllColor(QColor(settings->data(_COLOR)));
  line->setLabel(settings->data(_LABEL));
  line->setType((Curve::Type) line->typeFromString(settings->data(_STYLE)));
  line->setZ(0);
  i->setLine(settings->data(_LABEL), line);

  // create ref1 line
  Setting co;
  QString key = "-" + QString::number(i->chartObjectCount() + 1);
  co.setData("Type", QString("HLine"));
  co.setData("ID", key);
  co.setData("RO", 1);
  co.setData("Price", settings->data(_REF1));
  co.setData("Color", settings->data(_COLOR_REF1));
  i->addChartObject(co);

  // create ref2 line
  key = "-" + QString::number(i->chartObjectCount() + 1);
  co.setData("ID", key);
  co.setData("Price", settings->data(_REF2));
  co.setData("Color", settings->data(_COLOR_REF2));
  i->addChartObject(co);
  
  // create ref3 line
  key = "-" + QString::number(i->chartObjectCount() + 1);
  co.setData("ID", key);
  co.setData("Price", settings->data(_REF3));
  co.setData("Color", settings->data(_COLOR_REF3));
  i->addChartObject(co);

  return 0;
}

int CORREL::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // INPUT2
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
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::command: input missing" << command->parm("INPUT");
    return 1;
  }

  Curve *in2 = i->line(command->parm("INPUT2"));
  if (! in2)
  {
    qDebug() << _plugin << "::command: input 2 missing" << command->parm("INPUT2");
    return 1;
  }

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid period" << command->parm("PERIOD");
    return 1;
  }

  if (in->count() < period || in2->count() < period)
    return 1;

  QList<int> keys;
  int size = in->count();
  if (in2->count() < size)
  {
    size = in2->count();
    in2->keys(keys);
  }
  else
    in->keys(keys);

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    CurveBar *bar2 = in2->bar(keys.at(loop));
    if (! bar2)
      continue;

    input[loop] = (TA_Real) bar->data();
    input2[loop] = (TA_Real) bar2->data();
  }

  TA_RetCode rc = TA_CORREL(0,
                            size - 1,
                            &input[0],
                            &input2[0],
                            period,
                            &outBeg,
                            &outNb,
                            &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::command: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

void CORREL::dialog (QWidget *p, Indicator *i)
{
  CORRELDialog *dialog = new CORRELDialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void CORREL::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_COLOR, "red");
  set->setData(_LABEL, _plugin);
  set->setData(_STYLE, "Line");
  set->setData(_PERIOD, 30);
  set->setData(_INPUT, QString("Close"));
  set->setData(_INDEX, QString("YAHOO:SPY"));
  set->setData(_COLOR_REF1, "white");
  set->setData(_REF1, 1);
  set->setData(_COLOR_REF2, "white");
  set->setData(_REF2, 0);
  set->setData(_COLOR_REF3, "white");
  set->setData(_REF3, -1);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  CORREL *o = new CORREL;
  return ((Plugin *) o);
}
