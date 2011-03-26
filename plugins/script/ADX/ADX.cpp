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

#include "ADX.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "ADXDialog.h"

#include <QtDebug>

ADX::ADX ()
{
  _plugin = "ADX";
  _type = _INDICATOR;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("ADX::ADX: error on TA_Initialize");
}

int ADX::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();
  int period = settings->getInt(_PERIOD);

  int size = bd->count();
  TA_Real adxOut[size];
  TA_Real adxrOut[size];
  TA_Real pdiOut[size];
  TA_Real mdiOut[size];
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

  // ADX
  TA_RetCode rc = TA_ADX(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &adxOut[0]);
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
    line->setBar(dataLoop, new CurveBar(adxOut[outLoop]));
    dataLoop--;
    outLoop--;
  }

  line->setAllColor(QColor(settings->data(_ADX_COLOR)));
  line->setLabel(settings->data(_ADX_LABEL));
  line->setType((Curve::Type) line->typeFromString(settings->data(_ADX_STYLE)));
  line->setZ(2);
  i->setLine(settings->data(_ADX_LABEL), line);

  // MDI
  rc = TA_MINUS_DI(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &mdiOut[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  dataLoop = size - 1;
  outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(mdiOut[outLoop]));
    dataLoop--;
    outLoop--;
  }

  line->setAllColor(QColor(settings->data(_MDI_COLOR)));
  line->setLabel(settings->data(_MDI_LABEL));
  line->setType((Curve::Type) line->typeFromString(settings->data(_MDI_STYLE)));
  line->setZ(0);
  i->setLine(settings->data(_MDI_LABEL), line);
  
  // PDI
  rc = TA_PLUS_DI(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &pdiOut[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  dataLoop = size - 1;
  outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(pdiOut[outLoop]));
    dataLoop--;
    outLoop--;
  }

  line->setAllColor(QColor(settings->data(_PDI_COLOR)));
  line->setLabel(settings->data(_PDI_LABEL));
  line->setType((Curve::Type) line->typeFromString(settings->data(_PDI_STYLE)));
  line->setZ(1);
  i->setLine(settings->data(_PDI_LABEL), line);

  // ADXR
  rc = TA_ADXR(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &adxrOut[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  dataLoop = size - 1;
  outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(adxrOut[outLoop]));
    dataLoop--;
    outLoop--;
  }

  line->setAllColor(QColor(settings->data(_ADXR_COLOR)));
  line->setLabel(settings->data(_ADXR_LABEL));
  line->setType((Curve::Type) line->typeFromString(settings->data(_ADXR_STYLE)));
  line->setZ(3);
  i->setLine(settings->data(_ADXR_LABEL), line);

  return 0;
}

int ADX::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // NAME
  // PERIOD

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
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  bool ok;
  int period = 14;
  QString s = command->parm("PERIOD");
  if (! s.isEmpty())
  {
    period = s.toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid period" << s;
      return 1;
    }
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

  TA_RetCode rc = TA_ADX(0,
                         size - 1,
                         &high[0],
                         &low[0],
                         &close[0],
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

void ADX::dialog (QWidget *p, Indicator *i)
{
  ADXDialog *dialog = new ADXDialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void ADX::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_ADX_COLOR, QString("blue"));
  set->setData(_ADX_LABEL, _plugin);
  set->setData(_ADX_STYLE, QString("Line"));
  set->setData(_ADXR_COLOR, QString("yellow"));
  set->setData(_ADXR_LABEL, QString("ADXR"));
  set->setData(_ADXR_STYLE, QString("Line"));
  set->setData(_MDI_COLOR, QString("red"));
  set->setData(_MDI_LABEL, QString("MDI"));
  set->setData(_MDI_STYLE, QString("Line"));
  set->setData(_PDI_COLOR, QString("green"));
  set->setData(_PDI_LABEL, QString("PDI"));
  set->setData(_PDI_STYLE, QString("Line"));
  set->setData(_PERIOD, 14);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  ADX *o = new ADX;
  return ((Plugin *) o);
}
