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

#include "ULTOSC.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "ULTOSCDialog.h"

#include <QtDebug>

ULTOSC::ULTOSC ()
{
  _plugin = "ULTOSC";
  _type = _INDICATOR;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("ULTOSC::ULTOSC: error on TA_Initialize");
}

int ULTOSC::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();

  int sp = settings->getInt(_PERIOD_SHORT);
  int mp = settings->getInt(_PERIOD_MED);
  int lp = settings->getInt(_PERIOD_LONG);

  int size = bd->count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < bd->count(); loop++)
  {
    Bar *bar = bd->bar(loop);
    if (! bar)
      continue;

    high[loop] = (TA_Real) bar->high();
    low[loop] = (TA_Real) bar->low();
    close[loop] = (TA_Real) bar->close();
  }

  TA_RetCode rc = TA_ULTOSC(0,
                            size - 1,
                            &high[0],
                            &low[0],
                            &close[0],
                            sp,
                            mp,
                            lp,
                            &outBeg,
                            &outNb,
                            &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  Curve *line = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
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

int ULTOSC::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // NAME
  // PERIOD_SHORT
  // PERIOD_MED
  // PERIOD_LONG

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *ihigh = i->line(command->parm("INPUT_HIGH"));
  if (! ihigh)
  {
    qDebug() << _plugin << "::command: invalid INPUT_HIGH" << command->parm("INPUT_HIGH");
    return 1;
  }

  Curve *ilow = i->line(command->parm("INPUT_LOW"));
  if (! ilow)
  {
    qDebug() << _plugin << "::command: invalid INPUT_LOW" << command->parm("INPUT_LOW");
    return 1;
  }

  Curve *iclose = i->line(command->parm("INPUT_CLOSE"));
  if (! iclose)
  {
    qDebug() << _plugin << "::command: invalid INPUT_CLOSE" << command->parm("INPUT_CLOSE");
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  bool ok;
  int sp = command->parm("PERIOD_SHORT").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_SHORT" << command->parm("PERIOD_SHORT");
    return 1;
  }

  int mp = command->parm("PERIOD_MEDIUM").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_MEDIUM" << command->parm("PERIOD_MEDIUM");
    return 1;
  }

  int lp = command->parm("PERIOD_LONG").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_LONG" << command->parm("PERIOD_LONG");
    return 1;
  }

  int size = iclose->count();

  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int ipos = 0;
  int opos = 0;
  int end = 0;
  iclose->keyRange(ipos, end);
  for (; ipos <= end; ipos++, opos++)
  {
    CurveBar *hbar = ihigh->bar(ipos);
    if (! hbar)
      continue;

    CurveBar *lbar = ilow->bar(ipos);
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(ipos);
    if (! cbar)
      continue;

    high[opos] = (TA_Real) hbar->data();
    low[opos] = (TA_Real) lbar->data();
    close[opos] = (TA_Real) cbar->data();
  }

  TA_RetCode rc = TA_ULTOSC(0,
                            size - 1,
                            &high[0],
                            &low[0],
                            &close[0],
                            sp,
                            mp,
                            lp,
                            &outBeg,
                            &outNb,
                            &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::command: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

void ULTOSC::dialog (QWidget *p, Indicator *i)
{
  ULTOSCDialog *dialog = new ULTOSCDialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void ULTOSC::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_COLOR, "red");
  set->setData(_LABEL, _plugin);
  set->setData(_STYLE, "Line");
  set->setData(_PERIOD_SHORT, 7);
  set->setData(_PERIOD_MED, 14);
  set->setData(_PERIOD_LONG, 28);
  set->setData(_COLOR_REF1, "white");
  set->setData(_REF1, 30);
  set->setData(_COLOR_REF2, "white");
  set->setData(_REF2, 50);
  set->setData(_COLOR_REF3, "white");
  set->setData(_REF3, 70);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  ULTOSC *o = new ULTOSC;
  return ((Plugin *) o);
}
