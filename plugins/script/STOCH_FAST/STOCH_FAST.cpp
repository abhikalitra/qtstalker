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

#include "STOCH_FAST.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "MAType.h"
#include "STOCH_FASTDialog.h"
#include "InputType.h"

#include <QtDebug>

STOCH_FAST::STOCH_FAST ()
{
  _plugin = "STOCH_FAST";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("STOCH_FAST::STOCH_FAST: error on TA_Initialize");
}

int STOCH_FAST::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  Curve *kline = i->line(settings->data("OUTPUT_K"));
  if (kline)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT_K" << settings->data("OUTPUT_K");
    return 1;
  }

  Curve *dline = i->line(settings->data("OUTPUT_D"));
  if (dline)
  {
    qDebug() << _plugin << "::calculate: duplicate OUTPUT_D" << settings->data("OUTPUT_D");
    return 1;
  }

  int kperiod = settings->getInt("PERIOD_FASTK");
  int dperiod = settings->getInt("PERIOD_FASTD");

  MAType mat;
  int type = mat.fromString(settings->data("MA_TYPE_FASTD"));

  InputType it;
  QStringList order;
  order << "High" << "Low" << "Close";
  QList<Curve *> list;
  if (it.inputs(list, order, i, bd))
  {
    qDebug() << _plugin << "::calculate: input missing";
    return 1;
  }

  QList<Curve *> lines = getSTOCHF(list, kperiod, dperiod, type);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return 1;
  }

  kline = lines.at(0);
  kline->setAllColor(QColor(settings->data("COLOR_K")));
  kline->setLabel(settings->data("OUTPUT_K"));
  kline->setType(settings->data("STYLE_K"));
  kline->setZ(settings->getInt("Z_K"));
  i->setLine(settings->data("OUTPUT_K"), kline);

  dline = lines.at(1);
  dline->setAllColor(QColor(settings->data("COLOR_D")));
  dline->setLabel(settings->data("OUTPUT_D"));
  dline->setType(settings->data("STYLE_D"));
  dline->setZ(settings->getInt("Z_D"));
  i->setLine(settings->data("OUTPUT_D"), dline);

  return 0;
}

int STOCH_FAST::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // NAME_FASTK
  // NAME_FASTD
  // PERIOD_FASTK
  // PERIOD_FASTD
  // MA_TYPE_FASTD

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

  QString kname = command->parm("NAME_FASTK");
  Curve *line = i->line(kname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_FASTK" << kname;
    return 1;
  }

  QString dname = command->parm("NAME_FASTD");
  line = i->line(dname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_FASTD" << dname;
    return 1;
  }

  bool ok;
  int kperiod = command->parm("PERIOD_FASTK").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_FASTK" << command->parm("PERIOD_FASTK");
    return 1;
  }

  int dperiod = command->parm("PERIOD_FASTD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_FASTD" << command->parm("PERIOD_FASTD");
    return 1;
  }

  MAType mat;
  int type = mat.fromString(command->parm("MA_TYPE_FASTD"));
  if (type == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE_FASTD" << command->parm("MA_TYPE_FASTD");
    return 1;
  }
  
  QList<Curve *> list;
  list << ihigh << ilow << iclose;
  QList<Curve *> lines = getSTOCHF(list, kperiod, dperiod, type);
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

QList<Curve *> STOCH_FAST::getSTOCHF (QList<Curve *> &list, int kperiod, int dperiod, int type)
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

  TA_RetCode rc = TA_STOCHF(0,
                            size - 1,
                            &high[0],
                            &low[0],
                            &close[0],
                            kperiod,
                            dperiod,
                            (TA_MAType) type,
                            &outBeg,
                            &outNb,
                            &out[0],
                            &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getSTOCHF: TA-Lib error" << rc;
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

QWidget * STOCH_FAST::dialog (QWidget *p, Setting *set)
{
  return new STOCH_FASTDialog(p, set);
}

void STOCH_FAST::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR_K", QString("red"));
  set->setData("STYLE_K", QString("Line"));
  set->setData("COLOR_D", QString("yellow"));
  set->setData("STYLE_D", QString("Line"));
  set->setData("PERIOD_FASTK", 5);
  set->setData("PERIOD_FASTD", 3);
  set->setData("MA_TYPE_FASTD", QString("EMA"));
  set->setData("Z_K", 0);
  set->setData("OUTPUT_K", QString("%K"));
  set->setData("Z_D", 0);
  set->setData("OUTPUT_D", QString("%D"));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  STOCH_FAST *o = new STOCH_FAST;
  return ((Plugin *) o);
}
