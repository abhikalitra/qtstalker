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

#include <QtDebug>

STOCH_FAST::STOCH_FAST ()
{
  _plugin = "STOCH_FAST";
  _type = _INDICATOR;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("STOCH_FAST::STOCH_FAST: error on TA_Initialize");
}

int STOCH_FAST::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();

  int kperiod = settings->getInt(_PERIOD_FASTK);
  int dperiod = settings->getInt(_PERIOD_FASTD);

  MAType mat;
  int type = mat.fromString(settings->data(_MA_TYPE_FASTD));
  
  int size = bd->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];

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
    qDebug() << _plugin << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  Curve *kline = new Curve;
  Curve *dline = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    kline->setBar(dataLoop, new CurveBar(out[outLoop]));
    dline->setBar(dataLoop, new CurveBar(out2[outLoop]));

    dataLoop--;
    outLoop--;
  }

  kline->setAllColor(QColor(settings->data(_COLOR_K)));
  kline->setLabel(settings->data(_LABEL_K));
  kline->setType((Curve::Type) kline->typeFromString(settings->data(_STYLE_K)));
  kline->setZ(0);
  i->setLine(settings->data(_LABEL_K), kline);
  
  dline->setAllColor(QColor(settings->data(_COLOR_D)));
  dline->setLabel(settings->data(_LABEL_D));
  dline->setType((Curve::Type) dline->typeFromString(settings->data(_STYLE_D)));
  dline->setZ(1);
  i->setLine(settings->data(_LABEL_D), dline);

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
  
  int size = iclose->count();

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];

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
    qDebug() << _plugin << "::command: TA-Lib error" << rc;
    return 1;
  }

  Curve *kline = new Curve;
  Curve *dline = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    kline->setBar(dataLoop, new CurveBar(out[outLoop]));
    dline->setBar(dataLoop, new CurveBar(out2[outLoop]));

    dataLoop--;
    outLoop--;
  }

  kline->setLabel(kname);
  dline->setLabel(dname);

  i->setLine(kname, kline);
  i->setLine(dname, dline);

  command->setReturnCode("0");

  return 0;
}

void STOCH_FAST::dialog (QWidget *p, Indicator *i)
{
  STOCH_FASTDialog *dialog = new STOCH_FASTDialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void STOCH_FAST::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_COLOR_K, "red");
  set->setData(_LABEL_K, "%K");
  set->setData(_STYLE_K, "Line");
  set->setData(_COLOR_D, "yellow");
  set->setData(_LABEL_D, "%D");
  set->setData(_STYLE_D, "Line");
  set->setData(_PERIOD_FASTK, 5);
  set->setData(_PERIOD_FASTD, 3);
  set->setData(_COLOR_REF1, "white");
  set->setData(_REF1, 20);
  set->setData(_COLOR_REF2, "white");
  set->setData(_REF2, 80);
  set->setData(_MA_TYPE_FASTD, "EMA");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  STOCH_FAST *o = new STOCH_FAST;
  return ((Plugin *) o);
}
