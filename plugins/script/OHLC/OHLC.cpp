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

#include "OHLC.h"
#include "Curve.h"
#include "Globals.h"
#include "OHLCDialog.h"
#include "MAType.h"
#include "InputType.h"

#include <QtDebug>

OHLC::OHLC ()
{
  _type = _INDICATOR;
  _plugin = "OHLC";
}

int OHLC::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();
  
  QColor upColor(settings->data(_COLOR_UP));
  QColor downColor(settings->data(_COLOR_DOWN));
  QColor neutralColor(settings->data(_COLOR_NEUTRAL));

  Curve *line;
  if (settings->data(_STYLE) == "OHLC")
    line = new Curve(Curve::OHLC);
  else
    line = new Curve(Curve::Candle);
    
  line->setZ(0);

  int loop = 0;
  for (; loop < bd->count(); loop++)
  {
    Bar *bar = bd->bar(loop);
    if (! bar)
      continue;
    
    CurveBar *b = new CurveBar;
    b->setData(0, bar->open());
    b->setData(1, bar->high());
    b->setData(2, bar->low());
    b->setData(3, bar->close());
    b->setColor(neutralColor);

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

  // MA1
  if (settings->getInt(_MA1_CHECK))
  {
    InputType it;
    Curve *in = it.input(bd, settings->data(_MA1_INPUT));
    if (in)
    {
      MAType mat;
      Curve *ma = mat.getMA(in,
			    settings->getInt(_MA1_PERIOD),
			    mat.fromString(settings->data(_MA1_TYPE)));
      ma->setAllColor(QColor(settings->data(_MA1_COLOR)));
      ma->setLabel(settings->data(_MA1_LABEL));
      ma->setType((Curve::Type) line->typeFromString(settings->data(_MA1_STYLE)));
      ma->setZ(1);
      i->setLine(settings->data(_MA1_LABEL), ma);
      delete in;
    }
  }

  // MA2
  if (settings->getInt(_MA2_CHECK))
  {
    InputType it;
    Curve *in = it.input(bd, settings->data(_MA2_INPUT));
    if (in)
    {
      MAType mat;
      Curve *ma = mat.getMA(line,
			    settings->getInt(_MA2_PERIOD),
			    mat.fromString(settings->data(_MA2_TYPE)));
      ma->setAllColor(QColor(settings->data(_MA2_COLOR)));
      ma->setLabel(settings->data(_MA2_LABEL));
      ma->setType((Curve::Type) line->typeFromString(settings->data(_MA2_STYLE)));
      ma->setZ(2);
      i->setLine(settings->data(_MA2_LABEL), ma);
      delete in;
    }
  }

  // MA3
  if (settings->getInt(_MA3_CHECK))
  {
    InputType it;
    Curve *in = it.input(bd, settings->data(_MA3_INPUT));
    if (in)
    {
      MAType mat;
      Curve *ma = mat.getMA(line,
			    settings->getInt(_MA3_PERIOD),
			    mat.fromString(settings->data(_MA3_TYPE)));
      ma->setAllColor(QColor(settings->data(_MA3_COLOR)));
      ma->setLabel(settings->data(_MA3_LABEL));
      ma->setType((Curve::Type) line->typeFromString(settings->data(_MA3_STYLE)));
      ma->setZ(3);
      i->setLine(settings->data(_MA3_LABEL), ma);
      delete in;
    }
  }

  return 0;
}

int OHLC::command (Command *command)
{
  // PARMS:
  // INPUT_OPEN
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // NAME
  // COLOR_UP - default green
  // COLOR_DOWN - default red
  // COLOR_NEUTRAL - default blue

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *iopen = i->line(command->parm("INPUT_OPEN"));
  if (! iopen)
  {
    qDebug() << _plugin << "::command: invalid INPUT_OPEN" << command->parm("INPUT_OPEN");
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

  QColor upColor(Qt::green);
  QString s = command->parm("COLOR_UP");
  if (! s.isEmpty())
  {
    upColor.setNamedColor(s);
    if (! upColor.isValid())
    {
      qDebug() << _plugin << "::command: invalid COLOR_UP" << command->parm("COLOR_UP");
      return 1;
    }
  }

  QColor downColor(Qt::red);
  s = command->parm("COLOR_DOWN");
  if (! s.isEmpty())
  {
    downColor.setNamedColor(s);
    if (! downColor.isValid())
    {
      qDebug() << _plugin << "::command: invalid COLOR_DOWN" << command->parm("COLOR_DOWN");
      return 1;
    }
  }

  QColor neutralColor(Qt::blue);
  s = command->parm("COLOR_NEUTRAL");
  if (! s.isEmpty())
  {
    neutralColor.setNamedColor(s);
    if (! neutralColor.isValid())
    {
      qDebug() << _plugin << "::command: invalid COLOR_NEUTRAL" << command->parm("COLOR_NEUTRAL");
      return 1;
    }
  }

  line = new Curve(Curve::OHLC);

  int ipos = 0;
  int end = 0;
  iclose->keyRange(ipos, end);
  for (; ipos <= end; ipos++)
  {
    CurveBar *obar = iopen->bar(ipos);
    if (! obar)
      continue;

    CurveBar *hbar = ihigh->bar(ipos);
    if (! hbar)
      continue;

    CurveBar *lbar = ilow->bar(ipos);
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(ipos);
    if (! cbar)
      continue;

    CurveBar *bar = new CurveBar;
    bar->setData(0, obar->data());
    bar->setData(1, hbar->data());
    bar->setData(2, lbar->data());
    bar->setData(3, cbar->data());
    bar->setColor(neutralColor);

    CurveBar *ycbar = iclose->bar(ipos - 1);
    if (ycbar)
    {
      if (cbar->data() > ycbar->data())
        bar->setColor(upColor);
      else
      {
        if (cbar->data() < ycbar->data())
          bar->setColor(downColor);
      }
    }

    line->setBar(ipos, bar);
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

void OHLC::dialog (QWidget *p, Indicator *i)
{
  OHLCDialog *dialog = new OHLCDialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void OHLC::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_COLOR_UP, QString("green"));
  set->setData(_COLOR_DOWN, QString("red"));
  set->setData(_COLOR_NEUTRAL, QString("dimgray"));
  set->setData(_LABEL, QString("OHLC"));
  set->setData(_STYLE, QString("OHLC"));
  set->setData(_MA1_PERIOD, 20);
  set->setData(_MA1_TYPE, "EMA");
  set->setData(_MA1_COLOR, "yellow");
  set->setData(_MA1_LABEL, "OHLC_MA1");
  set->setData(_MA1_STYLE, "Line");
  set->setData(_MA1_CHECK, 1);
  set->setData(_MA2_PERIOD, 50);
  set->setData(_MA2_TYPE, "EMA");
  set->setData(_MA2_COLOR, "red");
  set->setData(_MA2_LABEL, "OHLC_MA2");
  set->setData(_MA2_STYLE, "Line");
  set->setData(_MA2_CHECK, 1);
  set->setData(_MA3_PERIOD, 200);
  set->setData(_MA3_TYPE, "EMA");
  set->setData(_MA3_COLOR, "blue");
  set->setData(_MA3_LABEL, "OHLC_MA3");
  set->setData(_MA3_STYLE, "Line");
  set->setData(_MA3_CHECK, 1);
  set->setData(_MA1_INPUT, "Close");
  set->setData(_MA2_INPUT, "Close");
  set->setData(_MA3_INPUT, "Close");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  OHLC *o = new OHLC;
  return ((Plugin *) o);
}
