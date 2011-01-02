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

#include "YAHOO_DATABASE.h"

#include <QtDebug>

YAHOO_DATABASE::YAHOO_DATABASE ()
{
  _method << "LOAD" << "SAVE" << "DELETE" << "SYMBOLS" << "TRANSACTION" << "COMMIT";

  _db = QSqlDatabase::database("data");
  QSqlQuery q(_db);

  // create the table
  QString s = "CREATE TABLE IF NOT EXISTS YahooSymbols (";
  s.append("ysymbol TEXT PRIMARY KEY UNIQUE"); // yahoo symbol
  s.append(", symbol TEXT"); // actual symbol
  s.append(", exchange TEXT"); // actual exchange
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "YAHOO_DATABASE::YAHOO_DATABASE: " << q.lastError().text();
}

int YAHOO_DATABASE::command (Command *command)
{
  // YAHOO_DATABASE,<METHOD>
  //         0          1
  
  if (command->count() < 2)
  {
    qDebug() << "YAHOO_DATABASE::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _LOAD:
      return load(command);
      break;
    case _SAVE:
      return save(command);
      break;
    case _DELETE:
      return deleteSymbol(command);
      break;
    case _SYMBOLS:
      return symbols(command);
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

int YAHOO_DATABASE::load (Command *command)
{
  // YAHOO_DATABASE,<METHOD>,<YSYMBOL>
  //       0            1       2

  if (command->count() != 3)
  {
    qDebug() << "YAHOO_DATABASE::load: invalid parm count" << command->count();
    return 1;
  }

  QString ysymbol = command->parm(2);

  QSqlQuery q(_db);

  QString s = "SELECT exchange,symbol FROM YahooSymbols WHERE ysymbol='" + ysymbol + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "YAHOO_DATABASE::load:" << q.lastError().text();
    return 1;
  }

  QStringList l;
  if (q.next())
    l << q.value(0).toString() << q.value(1).toString();

  command->setReturnData(l.join(","));

  return 0;
}

int YAHOO_DATABASE::save (Command *command)
{
  // YAHOO_DATABASE,<METHOD>,<YSYMBOL>,<EXCHANGE>,<SYMBOL>
  //        0           1        2         3         4

  if (command->count() != 5)
  {
    qDebug() << "YAHOO_DATABASE::save: invalid parm count" << command->count();
    return 1;
  }

  QSqlQuery q(_db);

  QString s = "INSERT OR REPLACE INTO YahooSymbols (ysymbol,exchange,symbol) VALUES (";
  s.append("'" + command->parm(2) + "'");
  s.append(",'" + command->parm(3) + "'");
  s.append(",'" + command->parm(4) + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "YAHOO_DATABASE::save:" << q.lastError().text();
    return 1;
  }

  command->setReturnData("0");

  return 0;
}

int YAHOO_DATABASE::deleteSymbol (Command *command)
{
  // YAHOO_DATABASE,<METHOD>,<YSYMBOL>
  //        0           1        2    

  if (command->count() != 3)
  {
    qDebug() << "YAHOO_DATABASE::deleteSymbol: invalid parm count" << command->count();
    return 1;
  }

  QSqlQuery q(_db);

  QString s = "DELETE FROM YahooSymbols WHERE ysymbol='" + command->parm(2) + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "YAHOO_DATABASE::deleteSymbol:" << q.lastError().text();
    return 1;
  }

  command->setReturnData("0");
  
  return 0;
}

int YAHOO_DATABASE::symbols (Command *command)
{
  // YAHOO_DATABASE,<METHOD>
  //        0          1

  if (command->count() != 2)
  {
    qDebug() << "YAHOO_DATABASE::symbols: invalid parm count" << command->count();
    return 1;
  }

  QSqlQuery q(_db);

  QString s = "SELECT ysymbol FROM YahooSymbols ORDER BY ysymbol ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "YAHOO_DATABASE::symbols: " << q.lastError().text();
    return 1;
  }

  QStringList l;
  while (q.next())
    l << q.value(0).toString();

  command->setReturnData(l.join(","));

  return 0;
}

int YAHOO_DATABASE::transaction (Command *command)
{
  // YAHOO_DATABASE,<METHOD>
  //        0          1

  if (command->count() != 2)
  {
    qDebug() << "YAHOO_DATABASE::transaction: invalid parm count" << command->count();
    return 1;
  }

  _db.transaction();
  
  command->setReturnData("0");

  return 0;
}

int YAHOO_DATABASE::commit (Command *command)
{
  // YAHOO_DATABASE,<METHOD>
  //        0          1

  if (command->count() != 2)
  {
    qDebug() << "YAHOO_DATABASE::commit: invalid parm count" << command->count();
    return 1;
  }

  _db.commit();

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  YAHOO_DATABASE *o = new YAHOO_DATABASE;
  return ((ScriptPlugin *) o);
}
