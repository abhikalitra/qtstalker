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

#include "SYMBOL.h"
#include "Globals.h"
#include "QuoteDataBase.h"
#include "DateRange.h"

#include <QtDebug>

SYMBOL::SYMBOL ()
{
  _plugin = "SYMBOL";
  _field << "OPEN" << "HIGH" << "LOW" << "CLOSE" << "VOLUME" << "OI";
}

int SYMBOL::command (Command *command)
{
  // PARMS:
  // NAME
  // EXCHANGE
  // SYMBOL
  // LENGTH
  // RANGE
  // FIELD

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
    qDebug() << _plugin << "::command: duplicate NAME" << name;
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
  Bar tbar;
  tbar.lengthList(l);
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
    bd.setBarLength((BarData::BarLength) length);

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

  QuoteDataBase db;
  if (db.getBars(&bd))
  {
    qDebug() << _plugin << "::command: QuoteDataBase error";
    return 1;
  }

  switch ((Field) _field.indexOf(command->parm("FIELD")))
  {
    case _OPEN:
      line = getOpen(bd);
      break;
    case _HIGH:
      line = getHigh(bd);
      break;
    case _LOW:
      line = getLow(bd);
      break;
    case _CLOSE:
      line = getClose(bd);
      break;
    case _VOLUME:
      line = getVolume(bd);
      break;
    case _OI:
      line = getOI(bd);
      break;
    default:
      qDebug() << _plugin << "::command: invalid field" << command->parm("FIELD");
      return 1;
      break;
  }
  
  line->setLabel(name);
  i->setLine(name, line);
  
  command->setReturnCode("0");

  return 0;
}

Curve * SYMBOL::getOpen (BarData &bd)
{
  Curve *line = new Curve;
  int loop = 0;
  for (; loop < bd.count(); loop++)
  {
    Bar *b = bd.bar(loop);
    line->setBar(loop, new CurveBar(b->open()));
  }

  return line;
}

Curve * SYMBOL::getHigh (BarData &bd)
{
  Curve *line = new Curve;
  int loop = 0;
  for (; loop < bd.count(); loop++)
  {
    Bar *b = bd.bar(loop);
    line->setBar(loop, new CurveBar(b->high()));
  }

  return line;
}

Curve * SYMBOL::getLow (BarData &bd)
{
  Curve *line = new Curve;
  int loop = 0;
  for (; loop < bd.count(); loop++)
  {
    Bar *b = bd.bar(loop);
    line->setBar(loop, new CurveBar(b->low()));
  }

  return line;
}

Curve * SYMBOL::getClose (BarData &bd)
{
  Curve *line = new Curve;
  int loop = 0;
  for (; loop < bd.count(); loop++)
  {
    Bar *b = bd.bar(loop);
    line->setBar(loop, new CurveBar(b->close()));
  }

  return line;
}

Curve * SYMBOL::getVolume (BarData &bd)
{
  Curve *line = new Curve;
  int loop = 0;
  for (; loop < bd.count(); loop++)
  {
    Bar *b = bd.bar(loop);
    line->setBar(loop, new CurveBar(b->volume()));
  }

  return line;
}

Curve * SYMBOL::getOI (BarData &bd)
{
  Curve *line = new Curve;
  int loop = 0;
  for (; loop < bd.count(); loop++)
  {
    Bar *b = bd.bar(loop);
    line->setBar(loop, new CurveBar(b->oi()));
  }

  return line;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  SYMBOL *o = new SYMBOL;
  return ((ScriptPlugin *) o);
}
