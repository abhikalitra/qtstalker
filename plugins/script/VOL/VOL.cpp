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

#include "VOL.h"
#include "Globals.h"
#include "VOLDialog.h"
#include "MAType.h"
#include "ta_libc.h"

#include <QtDebug>

VOL::VOL ()
{
  _type = _INDICATOR;
  _plugin = "VOL";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("VOL::VOL: error on TA_Initialize");
}

int VOL::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();
  
  QColor upColor(settings->data(_COLOR_UP));
  QColor downColor(settings->data(_COLOR_DOWN));
  QColor neutralColor(settings->data(_COLOR_NEUTRAL));

  Curve *line = new Curve(Curve::HistogramBar);
  line->setZ(0);

  int loop = 0;
  for (; loop < bd->count(); loop++)
  {
    Bar *bar = bd->bar(loop);
    if (! bar)
      continue;

    CurveBar *b = new CurveBar;
    b->setData(bar->volume());

    Bar *ybar = bd->bar(loop - 1);
    if (ybar)
    {
      if (bar->close() > ybar->close())
        b->setColor(upColor);
      else
      {
        if (bar->close() < ybar->close())
          b->setColor(downColor);
      }
    }

    line->setBar(loop, b);
  }

  line->setLabel(settings->data(_LABEL));
  i->setLine(settings->data(_LABEL), line);

  // MA
  MAType types;
  Curve *ma = getMA(line, settings->getInt(_MA_PERIOD), types.fromString(settings->data(_MA_TYPE)));
  if (! ma)
    return 0;

  ma->setColor(QColor(settings->data(_MA_COLOR)));
  ma->setLabel(settings->data(_MA_LABEL));
  ma->setType((Curve::Type) ma->typeFromString(settings->data(_MA_STYLE)));
  ma->setZ(1);
  i->setLine(settings->data(_MA_LABEL), ma);
  
  return 0;
}

Curve * VOL::getMA (Curve *in, int period, int method)
{
  if (in->count() < period)
    return 0;

  QList<int> keys;
  in->keys(keys);

  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_MA(0, size - 1, &input[0], period, (TA_MAType) method, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "VOL::getMA: TA-Lib error" << rc;
    return 0;
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

  return line;
}

int VOL::command (Command *command)
{
  // PARMS:
  // METHOD
  // NAME

  if (g_barData->count() < 1)
  {
    qDebug() << _plugin << "::command: no bars";
    return 1;
  }

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

  MAType mat;
  int method = mat.fromString(command->parm("METHOD"));
  if (method == -1)
  {
    qDebug() << _plugin << "::command: invalid METHOD" << command->parm("METHOD");
    return 1;
  }
  
  line = new Curve;
  int loop = 0;
  for (; loop < g_barData->count(); loop++)
  {
    Bar *b = g_barData->bar(loop);

    switch (method)
    {
      case 0: // date
      {
	CurveBar *cb = new CurveBar;
	cb->setDateTime(b->date());
        line->setBar(loop, cb);
	break;
      }
      case 1: // open
        line->setBar(loop, new CurveBar(b->open()));
	break;
      case 2: // high
        line->setBar(loop, new CurveBar(b->high()));
	break;
      case 3: // low
        line->setBar(loop, new CurveBar(b->low()));
	break;
      case 4: // close
        line->setBar(loop, new CurveBar(b->close()));
	break;
      case 5: // volume
        line->setBar(loop, new CurveBar(b->volume()));
	break;
      case 6: // oi
        line->setBar(loop, new CurveBar(b->oi()));
	break;
    }
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

void VOL::dialog (QWidget *p, Indicator *i)
{
  VOLDialog *dialog = new VOLDialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void VOL::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_COLOR_UP, QString("green"));
  set->setData(_COLOR_DOWN, QString("red"));
  set->setData(_COLOR_NEUTRAL, QString("dimgray"));
  set->setData(_LABEL, QString("VOL"));
  set->setData(_MA_COLOR, QString("yellow"));
  set->setData(_MA_LABEL, QString("VOLMA"));
  set->setData(_MA_STYLE, QString("Line"));
  set->setData(_MA_TYPE, QString("EMA"));
  set->setData(_MA_PERIOD, QString("10"));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  VOL *o = new VOL;
  return ((Plugin *) o);
}
