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

#include "CMO.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "CMODialog.h"
#include "MAType.h"
#include "InputType.h"

#include <QtDebug>

CMO::CMO ()
{
  _plugin = "CMO";
  _type = _INDICATOR;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CMO::CMO: error on TA_Initialize");
}

int CMO::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();
  
  int period = settings->getInt(_PERIOD);

  InputType itypes;
  Curve *in = itypes.input(bd, settings->data(_INPUT));
  if (! in)
    return 1;

  if (in->count() < period)
    return 1;

  TA_Real input[in->count()];
  TA_Real out[in->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_CMO(0,
                         keys.count() - 1,
                         &input[0],
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

  // MA
  MAType mat;
  Curve *ma = mat.getMA(line,
			settings->getInt(_MA_PERIOD),
			mat.fromString(settings->data(_MA_TYPE)));
  ma->setAllColor(QColor(settings->data(_MA_COLOR)));
  ma->setLabel(settings->data(_MA_LABEL));
  ma->setType((Curve::Type) line->typeFromString(settings->data(_MA_STYLE)));
  ma->setZ(1);
  i->setLine(settings->data(_MA_LABEL), ma);

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
  
  return 0;
}

int CMO::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
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

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid period" << command->parm("PERIOD");
    return 1;
  }

  if (in->count() < period)
    return 1;

  TA_Real input[in->count()];
  TA_Real out[in->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_CMO(0,
                         keys.count() - 1,
                         &input[0],
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

void CMO::dialog (QWidget *p, Indicator *i)
{
  CMODialog *dialog = new CMODialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void CMO::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_COLOR, "red");
  set->setData(_LABEL, _plugin);
  set->setData(_STYLE, "Line");
  set->setData(_PERIOD, 20);
  set->setData(_COLOR_REF1, "white");
  set->setData(_REF1, 50);
  set->setData(_COLOR_REF2, "white");
  set->setData(_REF2, -50);
  set->setData(_MA_PERIOD, 9);
  set->setData(_MA_TYPE, "EMA");
  set->setData(_MA_COLOR, "yellow");
  set->setData(_MA_LABEL, "CMO_MA");
  set->setData(_MA_STYLE, "Line");
  set->setData(_INPUT, "Close");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  CMO *o = new CMO;
  return ((Plugin *) o);
}
