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
  _plugin = "QUOTE_DATABASE";
  _method << "GET" << "SET" << "DELETE" << "EXCHANGE" << "RENAME" << "SEARCH" << "TRANSACTION" << "COMMIT";
}

int QUOTE_DATABASE::command (Command *command)
{
  // PARMS:
  // METHOD
  

  switch ((Method) _method.indexOf(command->parm("METHOD")))
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
  // PARMS:
  // METHOD (GET)
  // EXCHANGE
  // SYMBOL
  // LENGTH
  // DATE_START
  // DATE_END
  // DATE_RANGE

  BarData *bd = command->barData();
  if (! bd)
  {
    qDebug() << _plugin << "::getBars: missing bar data";
    return 1;
  }

  QString s = command->parm("EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::getBars: invalid EXCHANGE" << s;
    return 1;
  }
  bd->setExchange(s);

  s = command->parm("SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::getBars: invalid SYMBOL" << s;
    return 1;
  }
  bd->setSymbol(s);

  s = command->parm("LENGTH");
  QStringList l;
  Bar tbar;
  tbar.lengthList(l);
  int length = l.indexOf(s);
  if (length == -1)
  {
    qDebug() << _plugin << "::getBars: invalid LENGTH" << command->parm("LENGTH");
    return 1;
  }
  bd->setBarLength((BarData::BarLength) length);
  
  bd->setStartDate(QDateTime::fromString(command->parm("DATE_START"), Qt::ISODate));

  bd->setEndDate(QDateTime::fromString(command->parm("DATE_END"), Qt::ISODate));

  bd->setRange(command->parm("DATE_RANGE").toInt());

  if (_db.getBars(bd))
  {
    qDebug() << _plugin << "::getBars: QuoteDataBase error";
    return 1;
  }

  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::setBars (Command *command)
{
  // PARMS:
  // METHOD (SET)
  // FORMAT - delimited by colon
  // DATA - delimted by colon

  QStringList formatList;
  formatList << "D" << "O" << "H" << "L" << "C" << "V" << "I" << "X" << "S" << "N" << "T";

  // validate format parms
  QStringList format = command->parm("FORMAT").split(":", QString::SkipEmptyParts);

  if (! format.contains("D") || ! format.contains("X") || ! format.contains("S"))
  {
    qDebug() << _plugin << "::setBars: FORMAT must contain at least D,X,S fields" << command->parm("FORMAT");
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
    qDebug() << _plugin << "::setBars: invalid FORMAT parm" << command->parm("FORMAT");
    return 1;
  }

  // validate data fields
  QStringList data = command->parm("DATA").split(":", QString::SkipEmptyParts);

  if (data.count() != format.count())
  {
    qDebug() << _plugin << "::setBars: # of format fields not equal to # DATA fields" << command->parm("DATA");
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
    qDebug() << _plugin << "::setBars: DATA error" << command->parm("DATA");
    return 1;
  }

  if (_db.setBars(&symbol))
  {
    qDebug() << _plugin << "::setBars: QuoteDataBase error";
    delete bar;
    return 1;
  }
  
  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::deleteSymbol (Command *command)
{
  // PARMS:
  // METHOD (DELETE)
  // EXCHANGE
  // SYMBOL

  BarData bd;
  QString s = command->parm("EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::deleteSymbol: invalid EXCHANGE" << command->parm("EXCHANGE");
    return 1;
  }
  bd.setExchange(s);

  s = command->parm("SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::deleteSymbol: invalid SYMBOL" << command->parm("SYMBOL");
    return 1;
  }
  bd.setSymbol(s);

  if (_db.deleteSymbol(&bd))
  {
    qDebug() << _plugin << "::deleteSymbol: QuoteDataBase error";
    return 1;
  }
  
  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::getExchange (Command *command)
{
  // PARMS:
  // EXCHANGE

  QStringList l;
  if (_db.getExchange(l))
  {
    qDebug() << _plugin << "::getExchange: QuoteDataBase error";
    return 1;
  }

  command->setReturnData(_plugin + "_EXCHANGES", l.join(","));

  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::rename (Command *command)
{
  // PARMS:
  // METHOD (RENAME)
  // OLD_EXCHANGE
  // OLD_SYMBOL
  // NEW_EXCHANGE
  // NEW_SYMBOL

  BarData osymbol;
  QString s = command->parm("OLD_EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::rename: invalid OLD_EXCHANGE" << command->parm("OLD_EXCHANGE");
    return 1;
  }
  osymbol.setExchange(s);

  s = command->parm("OLD_SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::rename: invalid OLD_SYMBOL" << command->parm("OLD_SYMBOL");
    return 1;
  }
  osymbol.setSymbol(s);

  BarData nsymbol;
  s = command->parm("NEW_EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::rename: invalid NEW_EXCHANGE" << command->parm("NEW_EXCHANGE");
    return 1;
  }
  nsymbol.setExchange(s);

  s = command->parm("NEW_SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::rename: invalid NEW_SYMBOL" << command->parm("NEW_SYMBOL");
    return 1;
  }
  nsymbol.setSymbol(s);

  if (_db.rename(&osymbol, &nsymbol))
  {
    qDebug() << _plugin << "::rename: QuoteDataBase error";
    return 1;
  }

  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::search (Command *command)
{
  // PARMS:
  // METHOD (SEARCH)
  // EXCHANGE
  // SYMBOL

  BarData bd;
  QString s = command->parm("EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::search: invalid EXCHANGE" << command->parm("EXCHANGE");
    return 1;
  }
  bd.setExchange(s);

  s = command->parm("SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::search: invalid SYMBOL" << command->parm("SYMBOL");
    return 1;
  }
  bd.setSymbol(s);

  QStringList l;
  if (_db.search(&bd, l))
  {
    qDebug() << _plugin << "::search: QuoteDataBase error";
    return 1;
  }

  // now delimit each tuple with a semicolon
  command->setReturnData(_plugin + "_SEARCH", l.join(";"));

  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::transaction (Command *command)
{
  // PARMS:
  // METHOD (TRANSACTION)

  _db.transaction();

  command->setReturnCode("0");

  return 0;
}

int QUOTE_DATABASE::commit (Command *command)
{
  // PARMS:
  // METHOD (COMMIT)

  _db.commit();

  command->setReturnCode("0");

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
