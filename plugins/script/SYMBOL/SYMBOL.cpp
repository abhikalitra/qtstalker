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
  _method << "CURRENT" << "SYMBOL";
}

int SYMBOL::command (Command *command)
{
  // SYMBOL,<METHOD>
  //   0       1
  
  if (command->count() < 2)
  {
    qDebug() << "SYMBOL::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _CURRENT:
      return current(command);
      break;
    case _SYMBOL:
      return symbol(command);
      break;
    default:
      break;
  }

  return 0;
}

int SYMBOL::current (Command *command)
{
  // SYMBOL,CURRENT
  //   0       1

  command->setReturnData(g_barData->key());

  return 0;
}

int SYMBOL::symbol (Command *command)
{
  // SYMBOL,SYMBOL,<NAME>,<EXCHANGE>,<SYMBOL>,<LENGTH>,<RANGE>
  //   0      1       2        3        4        5        6

  if (command->count() != 7)
  {
    qDebug() << "SYMBOL::symbol: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "SYMBOL::symbol: no indicator";
    return 1;
  }

  int pos = 2;
  QString name = command->parm(pos);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "SYMBOL::symbol: duplicate name" << name;
    return 1;
  }

  BarData bd;
  pos++;
  QString s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "SYMBOL::symbol: invalid exchange";
    return 1;
  }
  bd.setExchange(s);

  pos++;
  s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "SYMBOL::symbol: invalid symbol";
    return 1;
  }
  bd.setSymbol(s);

  pos++;
  QStringList l;
  Bar tbar;
  tbar.lengthList(l);
  s = command->parm(pos);
  int length = l.indexOf(s);
  if (length == -1)
  {
    if (s != "-1")
    {
      qDebug() << "SYMBOL::symbol: invalid length" << command->parm(pos);
      return 1;
    }

    bd.setBarLength(g_barData->barLength());
  }
  else
    bd.setBarLength((BarData::BarLength) length);

  pos++;
  DateRange dr;
  int range = dr.toType(command->parm(pos));
  if (range == -1)
  {
    if (command->parm(pos) != "-1")
    {
      qDebug() << "SYMBOL::symbol: invalid range" << command->parm(pos);
      return 1;
    }
    
    bd.setRange(g_barData->range());
  }
  else
    bd.setRange(range);

  QuoteDataBase db;
  if (db.getBars(&bd))
  {
    qDebug() << "SYMBOL::symbol: QuoteDataBase error";
    return 1;
  }

  line = bd.input(BarData::Close);
  if (! line)
  {
    qDebug() << "SYMBOL::symbol: no input returned";
    return 1;
  }

  line->setLabel(name);
  i->setLine(name, line);
  
  command->setReturnData("0");

  return 0;
}

/*
int SYMBOL::symbol (Command *command)
{
  // SYMBOL,SYMBOL,<NAME>,<EXCHANGE>,<SYMBOL>,<LENGTH>,<START DATE>,<END DATE>,<DATE RANGE>,<FIELD>
  //   0      1       2        3        4         5          6          7           8          9

  if (command->count() != 10)
  {
    qDebug() << "SYMBOL::symbol: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "SYMBOL::symbol: no indicator";
    return 1;
  }

  int pos = 2;
  QString name = command->parm(pos);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "SYMBOL::symbol: duplicate name" << name;
    return 1;
  }

  BarData bd;
  pos++;
  QString s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "SYMBOL::symbol: invalid exchange";
    return 1;
  }
  bd.setExchange(s);

  pos++;
  s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "SYMBOL::symbol: invalid symbol";
    return 1;
  }
  bd.setSymbol(s);

  pos++;
  s = command->parm(pos);
  QStringList l;
  Bar tbar;
  tbar.lengthList(l);
  int length = l.indexOf(s);
  if (length == -1)
  {
    qDebug() << "SYMBOL::symbol: invalid length" << command->parm(pos);
    return 1;
  }
  bd.setBarLength((BarData::BarLength) length);

  pos++;
  bd.setStartDate(QDateTime::fromString(command->parm(pos), Qt::ISODate));

  pos++;
  bd.setEndDate(QDateTime::fromString(command->parm(pos), Qt::ISODate));

  pos++;
  bd.setRange(command->parm(pos).toInt());

  pos++;
  bd.inputFields(l);
  QString field = command->parm(pos);
  if (l.indexOf(field) == -1)
  {
    qDebug() << "SYMBOL::symbol: invalid field";
    return 1;
  }

  QuoteDataBase db;
  if (db.getBars(&bd))
  {
    qDebug() << "SYMBOL::symbol: QuoteDataBase error";
    return 1;
  }

  line = bd.input(bd.inputType(field));
  if (! line)
  {
    qDebug() << "SYMBOL::symbol: no input returned";
    return 1;
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnData("0");

  return 0;
}
*/

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  SYMBOL *o = new SYMBOL;
  return ((ScriptPlugin *) o);
}
