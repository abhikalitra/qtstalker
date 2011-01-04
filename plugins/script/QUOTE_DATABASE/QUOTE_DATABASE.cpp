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

#include "QUOTE_DATABASE.h"
#include "Strip.h"
#include "DateRange.h"
#include "Bar.h"

#include <QtDebug>
#include <QDateTime>

QUOTE_DATABASE::QUOTE_DATABASE ()
{
  _method << "GET" << "SET" << "DELETE" << "EXCHANGE" << "RENAME" << "SEARCH" << "TRANSACTION" << "COMMIT";
}

int QUOTE_DATABASE::command (Command *command)
{
  if (command->count() < 2)
  {
    qDebug() << "QUOTE_DATABASE::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _GET:
      return getBars(command);
      break;
    case _SET:
      return setBars(command);
      break;
    case _DELETE:
      return deleteSymbol(command);
      break;
    case _EXCHANGE:
      return getExchange(command);
      break;
    case _RENAME:
      return rename(command);
      break;
    case _SEARCH:
      return search(command);
      break;
    case _TRANSACTION:
      return transaction(command);
      break;
    case _COMMIT:
      return commit(command);
      break;
    default:
      break;
  }

  return 0;
}

int QUOTE_DATABASE::getBars (Command *command)
{
  // QUOTE_DATABASE,<METHOD>,<EXCHANGE>,<SYMBOL>,<LENGTH>,<START DATE>,<END DATE>,<DATE RANGE>
  //        0          1         2         3         4          5          6           7

  if (command->count() != 8)
  {
    qDebug() << "QUOTE_DATABASE::getBars: invalid parm count" << command->count();
    return 1;
  }

  BarData *bd = command->barData();
  if (! bd)
  {
    qDebug() << "QUOTE_DATABASE::getBars: missing bar data";
    return 1;
  }

  int pos = 2;
  QString s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::getBars: invalid exchange";
    return 1;
  }
  bd->setExchange(s);

  pos++;
  s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::getBars: invalid symbol";
    return 1;
  }
  bd->setSymbol(s);

  pos++;
  s = command->parm(pos);
  QStringList l;
  Bar tbar;
  tbar.lengthList(l);
  int length = l.indexOf(s);
  if (length == -1)
  {
    qDebug() << "QUOTE_DATABASE::getBars: invalid length" << command->parm(pos);
    return 1;
  }
  bd->setBarLength((BarData::BarLength) length);
  
  pos++;
  bd->setStartDate(QDateTime::fromString(command->parm(pos), Qt::ISODate));

  pos++;
  bd->setEndDate(QDateTime::fromString(command->parm(pos), Qt::ISODate));

  pos++;
  bd->setRange(command->parm(pos).toInt());

  if (_db.getBars(bd))
  {
    qDebug() << "QUOTE_DATABASE::getBars: QuoteDataBase error";
    return 1;
  }

  command->setReturnData("0");

  return 0;
}

int QUOTE_DATABASE::setBars (Command *command)
{
  // QUOTE_DATABASE,<METHOD>,<FORMAT>,<DATA>
  //       0           1         2      3
  // format delimited by a colon
  // data delimted by a colon between fields

  if (command->count() != 4)
  {
    qDebug() << "QUOTE_DATABASE::setBars: invalid parm count" << command->count();
    return 1;
  }

  QStringList formatList;
  formatList << "D" << "O" << "H" << "L" << "C" << "V" << "I" << "X" << "S" << "N" << "T";

  // validate format parms
  QStringList format = command->parm(2).split(":", QString::SkipEmptyParts);

  if (! format.contains("D") || ! format.contains("X") || ! format.contains("S"))
  {
    qDebug() << "QUOTE_DATABASE::setBars: format must contain at least D,X,S fields" << command->parm(2);
    return 1;
  }

  int tloop = 0;
  int flag = 0;
  for (; tloop < format.count(); tloop++)
  {
    if (formatList.indexOf(format.at(tloop)) == -1)
    {
      flag = 1;
      break;
    }
  }
    
  if (flag)
  {
    qDebug() << "QUOTE_DATABASE::setBars: invalid format parm" << command->parm(2);
    return 1;
  }

  // validate data fields
  QStringList data = command->parm(3).split(":", QString::SkipEmptyParts);

  if (data.count() != format.count())
  {
    qDebug() << "QUOTE_DATABASE::setBars: # of format fields not equal to # data fields" << command->parm(3);
    return 1;
  }

  BarData symbol;
  Bar *bar = new Bar;
  flag = 0;
  for (tloop = 0; tloop < format.count(); tloop++)
  {
    switch (formatList.indexOf(format.at(tloop)))
    {
      case 0: // D
        flag = bar->setDates(data.at(tloop), data.at(tloop));
        break;
      case 1: // O
        flag = bar->setOpen(data.at(tloop));
        break;
      case 2: // H
        flag = bar->setHigh(data.at(tloop));
        break;
      case 3: // L
        flag = bar->setLow(data.at(tloop));
        break;
      case 4: // C
        flag = bar->setClose(data.at(tloop));
        break;
      case 5: // V
        flag = bar->setVolume(data.at(tloop));
        break;
      case 6: // I
        flag = bar->setOI(data.at(tloop));
        break;
      case 7: // X
        symbol.setExchange(data.at(tloop));
        break;
      case 8: // S
        symbol.setSymbol(data.at(tloop));
        break;
      case 9: // N
        symbol.setName(data.at(tloop));
        break;
      case 10: // T
        symbol.setType(data.at(tloop));
        break;
      default:
        break;
    }
  }

  if (flag)
  {
    delete bar;
    qDebug() << "QUOTE_DATABASE::setBars: data error" << command->parm(3);
    return 1;
  }

  if (_db.setBars(&symbol))
  {
    qDebug() << "QUOTE_DATABASE::setBars: QuoteDataBase error";
    delete bar;
    return 1;
  }
  
  command->setReturnData("0");

  return 0;
}

int QUOTE_DATABASE::deleteSymbol (Command *command)
{
  // QUOTE_DATABASE,DELETE,<EXCHANGE>,<SYMBOL>
  //         0         1        2        3

  if (command->count() != 4)
  {
    qDebug() << "QUOTE_DATABASE::deleteSymbol: invalid parm count" << command->count();
    return 1;
  }

  BarData bd;
  int pos = 2;
  QString s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::deleteSymbol: invalid exchange" << command->parm(pos);
    return 1;
  }
  bd.setExchange(s);

  pos++;
  s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::deleteSymbol: invalid symbol" << command->parm(pos);
    return 1;
  }
  bd.setSymbol(s);

  if (_db.deleteSymbol(&bd))
  {
    qDebug() << "QUOTE_DATABASE::deleteSymbol: QuoteDataBase error";
    return 1;
  }
  
  command->setReturnData("0");

  return 0;
}

int QUOTE_DATABASE::getExchange (Command *command)
{
  // QUOTE_DATABASE,EXCHANGE
  //       0           1

  QStringList l;
  if (_db.getExchange(l))
  {
    qDebug() << "QUOTE_DATABASE::getExchange: QuoteDataBase error";
    return 1;
  }

  command->setReturnData(l.join(","));

  return 0;
}

int QUOTE_DATABASE::rename (Command *command)
{
  // QUOTE_DATABASE,RENAME,<OLD EXCHANGE>,<OLD SYMBOL>,<NEW EXCHANGE>,<NEW SYMBOL>
  //          0       1           2             3             4            5

  if (command->count() != 6)
  {
    qDebug() << "QUOTE_DATABASE::rename: invalid parm count" << command->count();
    return 1;
  }

  BarData osymbol;
  int pos = 2;
  QString s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::rename: invalid old exchange" << command->parm(pos);
    return 1;
  }
  osymbol.setExchange(s);

  pos++;
  s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::rename: invalid old symbol" << command->parm(pos);
    return 1;
  }
  osymbol.setSymbol(s);

  BarData nsymbol;
  pos++;
  s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::rename: invalid new exchange" << command->parm(pos);
    return 1;
  }
  nsymbol.setExchange(s);

  pos++;
  s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::rename: invalid new symbol" << command->parm(pos);
    return 1;
  }
  nsymbol.setSymbol(s);

  if (_db.rename(&osymbol, &nsymbol))
  {
    qDebug() << "QUOTE_DATABASE::rename: QuoteDataBase error";
    return 1;
  }

  command->setReturnData("0");

  return 0;
}

int QUOTE_DATABASE::search (Command *command)
{
  // QUOTE_DATABASE,SEARCH,<EXCHANGE>,<SYMBOL>
  //       0          1         2         3

  if (command->count() != 4)
  {
    qDebug() << "QUOTE_DATABASE::search: invalid parm count" << command->count();
    return 1;
  }

  BarData bd;
  int pos = 2;
  QString s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::search: invalid exchange" << command->parm(pos);
    return 1;
  }
  bd.setExchange(s);

  pos++;
  s = command->parm(pos);
  if (s.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::search: invalid symbol" << command->parm(pos);
    return 1;
  }
  bd.setSymbol(s);

  QStringList l;
  if (_db.search(&bd, l))
  {
    qDebug() << "QUOTE_DATABASE::search: QuoteDataBase error";
    return 1;
  }

  // now delimit each tuple with a semicolon
  command->setReturnData(l.join(";"));

  return 0;
}

int QUOTE_DATABASE::transaction (Command *command)
{
  // QUOTE_DATABASE,TRANSACTION
  //        0            1

  _db.transaction();

  command->setReturnData("0");

  return 0;
}

int QUOTE_DATABASE::commit (Command *command)
{
  // QUOTE_DATABASE,COMMIT
  //        0         1

  _db.commit();

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  QUOTE_DATABASE *o = new QUOTE_DATABASE;
  return ((ScriptPlugin *) o);
}
