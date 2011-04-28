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

#include "STOCH_SLOW.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "MAType.h"
#include "InputType.h"

#include <QtDebug>

STOCH_SLOW::STOCH_SLOW ()
{
  _plugin = "STOCH_SLOW";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("STOCH_SLOW::STOCH_SLOW: error on TA_Initialize");
}

int STOCH_SLOW::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // NAME_SLOWK
  // NAME_SLOWD
  // PERIOD_FASTK
  // PERIOD_SLOWK
  // MA_TYPE_SLOWK
  // PERIOD_SLOWD
  // MA_TYPE_SLOWD

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

  QString kname = command->parm("NAME_SLOWK");
  Curve *line = i->line(kname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_SLOWK" << kname;
    return 1;
  }

  QString dname = command->parm("NAME_SLOWD");
  line = i->line(dname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_SLOWD" << dname;
    return 1;
  }

  bool ok;
  int fkperiod = command->parm("PERIOD_FASTK").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_FASTK" << command->parm("PERIOD_FASTK");
    return 1;
  }

  int skperiod = command->parm("PERIOD_SLOWK").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_SLOWK" << command->parm("PERIOD_SLOWK");
    return 1;
  }

  MAType mat;
  int kma = mat.fromString(command->parm("MA_TYPE_SLOWK"));
  if (kma == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE_SLOWK" << command->parm("MA_TYPE_SLOWK");
    return 1;
  }

  int sdperiod = command->parm("PERIOD_SLOWD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_SLOWD" << command->parm("PERIOD_SLOWD");
    return 1;
  }

  int dma = mat.fromString(command->parm("MA_TYPE_SLOWD"));
  if (dma == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE_SLOWD" << command->parm("MA_TYPE_SLOWD");
    return 1;
  }

  QList<Curve *> list;
  list << ihigh << ilow << iclose;
  QList<Curve *> lines = getSTOCHS(list, fkperiod, skperiod, sdperiod, kma, dma);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return 1;
  }

  Curve *kline = lines.at(0);
  kline->setLabel(kname);
  i->setLine(kname, kline);

  Curve *dline = lines.at(1);
  dline->setLabel(dname);
  i->setLine(dname, dline);

  command->setReturnCode("0");

  return 0;
}

QList<Curve *> STOCH_SLOW::getSTOCHS (QList<Curve *> &list, int fkperiod, int skperiod, int sdperiod, int kma, int dma)
{
  QList<Curve *> lines;
  if (! list.count())
    return lines;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &high[0], &low[0], &close[0], &close[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_STOCH(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           &close[0],
                           fkperiod,
                           skperiod,
                           (TA_MAType) kma,
                           sdperiod,
                           (TA_MAType) dma,
                           &outBeg,
                           &outNb,
                           &out[0],
                           &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getSTOCHS: TA-Lib error" << rc;
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

void STOCH_SLOW::settings (Setting *set)
{
  set->clear();

  QStringList keys;
  keys << "NAME_SLOWK"  << "NAME_SLOWD" << "INPUT_HIGH" << "INPUT_LOW" << "INPUT_CLOSE";
  keys << "PERIOD_FASTK" << "PERIOD_SLOWK" << "PERIOD_SLOWD" << "MA_TYPE_SLOWK" << "MA_TYPE_SLOWD";
  set->setData("KEYS", keys.join(","));

  set->setData("PLUGIN", _plugin);
  set->setData("PLUGIN_TYPE", QString("INDICATOR"));
  set->setData("NAME_SLOWK", QString("%K"));
  set->setData("NAME_SLOWK:TYPE", QString("TEXT"));
  set->setData("NAME_SLOWD", QString("%D"));
  set->setData("NAME_SLOWD:TYPE", QString("TEXT"));
  set->setData("INPUT_HIGH", QString("High"));
  set->setData("INPUT_HIGH:TYPE", QString("TEXT"));
  set->setData("INPUT_LOW", QString("Low"));
  set->setData("INPUT_LOW:TYPE", QString("TEXT"));
  set->setData("INPUT_CLOSE", QString("Close"));
  set->setData("INPUT_CLOSE:TYPE", QString("TEXT"));
  set->setData("PERIOD_FASTK", 5);
  set->setData("PERIOD_FASTK:TYPE", QString("INTEGER"));
  set->setData("PERIOD_SLOWK", 3);
  set->setData("PERIOD_SLOWK:TYPE", QString("INTEGER"));
  set->setData("PERIOD_SLOWD", 3);
  set->setData("PERIOD_SLOWD:TYPE", QString("INTEGER"));
  set->setData("MA_TYPE_SLOWK", QString("EMA"));
  set->setData("MA_TYPE_SLOWK:TYPE", QString("LIST"));
  MAType mat;
  set->setData("MA_TYPE_SLOWK:LIST", mat.list().join(","));
  set->setData("MA_TYPE_SLOWD", QString("EMA"));
  set->setData("MA_TYPE_SLOWD:TYPE", QString("LIST"));
  set->setData("MA_TYPE_SLOWD:LIST", mat.list().join(","));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  STOCH_SLOW *o = new STOCH_SLOW;
  return ((Plugin *) o);
}
