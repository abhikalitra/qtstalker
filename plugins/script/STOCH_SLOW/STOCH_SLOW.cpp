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
#include "STOCHSDialog.h"
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

int STOCH_SLOW::calculate (BarData *bd, Indicator *i, Setting *settings)
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

  int fkperiod = settings->getInt("PERIOD_FASTK");
  int skperiod = settings->getInt("PERIOD_SLOWK");
  int sdperiod = settings->getInt("PERIOD_SLOWD");

  MAType mat;
  int kma = mat.fromString(settings->data("MA_TYPE_SLOWK"));
  int dma = mat.fromString(settings->data("MA_TYPE_SLOWD"));

  Curve *ihigh = i->line("High");
  if (! ihigh)
  {
    InputType it;
    ihigh = it.input(bd, "High");
    if (! ihigh)
    {
      qDebug() << _plugin << "::calculate: no High";
      return 1;
    }

    ihigh->setLabel("High");
    i->setLine("High", ihigh);
  }

  Curve *ilow = i->line("Low");
  if (! ilow)
  {
    InputType it;
    ilow = it.input(bd, "Low");
    if (! ilow)
    {
      qDebug() << _plugin << "::calculate: no Low";
      return 1;
    }

    ilow->setLabel("Low");
    i->setLine("Low", ilow);
  }

  Curve *iclose = i->line("Close");
  if (! iclose)
  {
    InputType it;
    iclose = it.input(bd, "Close");
    if (! iclose)
    {
      qDebug() << _plugin << "::calculate: no Close";
      return 1;
    }

    iclose->setLabel("Close");
    i->setLine("Close", iclose);
  }

  QList<Curve *> lines = getSTOCHS(ihigh, ilow, iclose, fkperiod, skperiod, sdperiod, kma, dma);
  if (! lines.count())
    return 1;

  kline = lines.at(0);
  kline->setAllColor(QColor(settings->data("COLOR_K")));
  kline->setLabel(settings->data("OUTPUT_K"));
  kline->setType(settings->data("STYLE_K"));
  kline->setZ(settings->getInt("Z_K"));
  i->setLine(settings->data("OUTPUT_K"), kline);

  if (lines.count() == 2)
  {
    dline = lines.at(1);
    dline->setAllColor(QColor(settings->data("COLOR_D")));
    dline->setLabel(settings->data("OUTPUT_D"));
    dline->setType(settings->data("STYLE_D"));
    dline->setZ(settings->getInt("Z_D"));
    i->setLine(settings->data("OUTPUT_D"), dline);
  }

  return 0;
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

  QList<Curve *> lines = getSTOCHS(ihigh, ilow, iclose, fkperiod, skperiod, sdperiod, kma, dma);
  if (! lines.count())
    return 1;

  Curve *kline = lines.at(0);
  kline->setLabel(kname);
  i->setLine(kname, kline);

  if (lines.count() == 2)
  {
    Curve *dline = lines.at(1);
    dline->setLabel(dname);
    i->setLine(dname, dline);
  }

  command->setReturnCode("0");

  return 0;
}

QList<Curve *> STOCH_SLOW::getSTOCHS (Curve *ihigh, Curve *ilow, Curve *iclose, int fkperiod,
				      int skperiod, int sdperiod, int kma, int dma)
{
  QList<int> keys;
  int size = ihigh->count();
  ihigh->keys(keys);

  if (ilow->count() < size)
  {
    size = ilow->count();
    ilow->keys(keys);
  }

  if (iclose->count() < size)
  {
    size = iclose->count();
    iclose->keys(keys);
  }

  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int ipos = 0;
  int opos = 0;
  for (; ipos < keys.count(); ipos++, opos++)
  {
    CurveBar *hbar = ihigh->bar(keys.at(ipos));
    if (! hbar)
      continue;

    CurveBar *lbar = ilow->bar(keys.at(ipos));
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(keys.at(ipos));
    if (! cbar)
      continue;

    high[opos] = (TA_Real) hbar->data();
    low[opos] = (TA_Real) lbar->data();
    close[opos] = (TA_Real) cbar->data();
  }

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

  QList<Curve *> lines;
  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getSTOCHS: TA-Lib error" << rc;
    return lines;
  }

  Curve *kline = new Curve;
  Curve *dline = new Curve;
  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    kline->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    dline->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));
    keyLoop--;
    outLoop--;
  }

  lines.append(kline);
  lines.append(dline);

  return lines;
}

QWidget * STOCH_SLOW::dialog (QWidget *p, Setting *set)
{
  return new STOCHSDialog(p, set);
}

void STOCH_SLOW::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR_K", QString("red"));
  set->setData("STYLE_K", QString("Line"));
  set->setData("COLOR_D", QString("yellow"));
  set->setData("STYLE_D", QString("Line"));
  set->setData("PERIOD_FASTK", 5);
  set->setData("PERIOD_SLOWK", 3);
  set->setData("PERIOD_SLOWD", 3);
  set->setData("MA_TYPE_SLOWK", QString("EMA"));
  set->setData("MA_TYPE_SLOWD", QString("EMA"));
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
  STOCH_SLOW *o = new STOCH_SLOW;
  return ((Plugin *) o);
}
