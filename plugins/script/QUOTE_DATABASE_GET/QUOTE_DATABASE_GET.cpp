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

#include "QUOTE_DATABASE_GET.h"
#include "Globals.h"
#include "QuoteDataBase.h"
#include "DateRange.h"
#include "BarLength.h"

#include <QtDebug>

QUOTE_DATABASE_GET::QUOTE_DATABASE_GET ()
{
  _plugin = "QUOTE_DATABASE_GET";
}

int QUOTE_DATABASE_GET::command (Command *command)
{
  // PARMS:
  // NAME_OPEN
  // NAME_HIGH
  // NAME_LOW
  // NAME_CLOSE
  // NAME_VOLUME
  // NAME_OI
  // EXCHANGE
  // SYMBOL
  // LENGTH
  // RANGE

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  BarData bd;
  QString s = command->parm("EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid exchange" << s;
    return 1;
  }
  bd.setExchange(s);

  s = command->parm("SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid symbol" << s;
    return 1;
  }
  bd.setSymbol(s);

  QStringList l;
  BarLength bl;
  l = bl.list();
  s = command->parm("LENGTH");
  int length = l.indexOf(s);
  if (length == -1)
  {
    if (s != "-1")
    {
      qDebug() << _plugin << "::command: invalid LENGTH" << command->parm("LENGTH");
      return 1;
    }

    bd.setBarLength(g_barData->barLength());
  }
  else
    bd.setBarLength((BarLength::Length) length);

  DateRange dr;
  int range = dr.toType(command->parm("RANGE"));
  if (range == -1)
  {
    if (command->parm("RANGE") != "-1")
    {
      qDebug() << _plugin << "::command: invalid RANGE" << command->parm("RANGE");
      return 1;
    }
    
    bd.setRange(g_barData->range());
  }
  else
    bd.setRange(range);

  Curve *oline = 0;
  s = command->parm("NAME_OPEN");
  if (! s.isEmpty())
  {
    oline = i->line(s);
    if (oline)
    {
      qDebug() << _plugin << "::command: duplicate NAME_OPEN" << s;
      return 1;
    }

    oline = new Curve;
    oline->setLabel(s);
    i->setLine(s, oline);
  }

  Curve *hline = 0;
  s = command->parm("NAME_HIGH");
  if (! s.isEmpty())
  {
    hline = i->line(s);
    if (hline)
    {
      qDebug() << _plugin << "::command: duplicate NAME_HIGH" << s;
      return 1;
    }

    hline = new Curve;
    hline->setLabel(s);
    i->setLine(s, hline);
  }

  Curve *lline = 0;
  s = command->parm("NAME_LOW");
  if (! s.isEmpty())
  {
    lline = i->line(s);
    if (lline)
    {
      qDebug() << _plugin << "::command: duplicate NAME_LOW" << s;
      return 1;
    }

    lline = new Curve;
    lline->setLabel(s);
    i->setLine(s, lline);
  }

  Curve *cline = 0;
  s = command->parm("NAME_CLOSE");
  if (! s.isEmpty())
  {
    cline = i->line(s);
    if (cline)
    {
      qDebug() << _plugin << "::command: duplicate NAME_CLOSE" << s;
      return 1;
    }

    cline = new Curve;
    cline->setLabel(s);
    i->setLine(s, cline);
  }

  Curve *vline = 0;
  s = command->parm("NAME_VOLUME");
  if (! s.isEmpty())
  {
    vline = i->line(s);
    if (vline)
    {
      qDebug() << _plugin << "::command: duplicate NAME_VOLUME" << s;
      return 1;
    }

    vline = new Curve;
    vline->setLabel(s);
    i->setLine(s, vline);
  }

  Curve *iline = 0;
  s = command->parm("NAME_OI");
  if (! s.isEmpty())
  {
    iline = i->line(s);
    if (iline)
    {
      qDebug() << _plugin << "::command: duplicate NAME_OI" << s;
      return 1;
    }

    iline = new Curve;
    iline->setLabel(s);
    i->setLine(s, iline);
  }

  QuoteDataBase db;
  if (db.getBars(&bd))
  {
    qDebug() << _plugin << "::command: QuoteDataBase error";
    return 1;
  }

  int loop = 0;
  for (; loop < bd.count(); loop++)
  {
    Bar *b = bd.bar(loop);
    if (! b)
      continue;

    if (oline)
      oline->setBar(loop, new CurveBar(b->open()));
      
    if (hline)
      hline->setBar(loop, new CurveBar(b->high()));

    if (lline)
      lline->setBar(loop, new CurveBar(b->low()));

    if (cline)
      cline->setBar(loop, new CurveBar(b->close()));

    if (vline)
      vline->setBar(loop, new CurveBar(b->volume()));

    if (iline)
      iline->setBar(loop, new CurveBar(b->oi()));
  }
  
  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  QUOTE_DATABASE_GET *o = new QUOTE_DATABASE_GET;
  return ((ScriptPlugin *) o);
}
