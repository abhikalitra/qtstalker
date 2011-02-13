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
  _plugin = "YAHOO_DATABASE";
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
  // PARMS:
  // METHOD

  switch ((Method) _method.indexOf(command->parm("METHOD")))
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
  // PARMS:
  // METHOD (LOAD)
  // YSYMBOL

  QString ysymbol = command->parm("YSYMBOL");

  QSqlQuery q(_db);

  QString s = "SELECT exchange,symbol FROM YahooSymbols WHERE ysymbol='" + ysymbol + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << _plugin << "::load:" << q.lastError().text();
    return 1;
  }

  if (q.next())
  {
    command->setReturnData(_plugin + "_EXCHANGE", q.value(0).toString());
    command->setReturnData(_plugin + "_SYMBOL", q.value(1).toString());
  }

  command->setReturnCode("0");

  return 0;
}

int YAHOO_DATABASE::save (Command *command)
{
  // PARMS:
  // METHOD (SAVE)
  // YSYMBOL
  // EXCHANGE
  // SYMBOL

  QSqlQuery q(_db);

  QString s = "INSERT OR REPLACE INTO YahooSymbols (ysymbol,exchange,symbol) VALUES (";
  s.append("'" + command->parm("YSYMBOL") + "'");
  s.append(",'" + command->parm("EXCHANGE") + "'");
  s.append(",'" + command->parm("SYMBOL") + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << _plugin << "::save:" << q.lastError().text();
    return 1;
  }

  command->setReturnCode("0");

  return 0;
}

int YAHOO_DATABASE::deleteSymbol (Command *command)
{
  // PARMS:
  // METHOD (DELETE)
  // YSYMBOL

  QSqlQuery q(_db);

  QString s = "DELETE FROM YahooSymbols WHERE ysymbol='" + command->parm("YSYMBOL") + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << _plugin << "::deleteSymbol:" << q.lastError().text();
    return 1;
  }

  command->setReturnCode("0");
  
  return 0;
}

int YAHOO_DATABASE::symbols (Command *command)
{
  // PARMS:
  // METHOD (SYMBOLS)

  QSqlQuery q(_db);

  QString s = "SELECT ysymbol FROM YahooSymbols ORDER BY ysymbol ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << _plugin << "::symbols: " << q.lastError().text();
    return 1;
  }

  QStringList l;
  while (q.next())
    l << q.value(0).toString();

  command->setReturnData(_plugin + "_SYMBOLS", l.join(","));

  command->setReturnCode("0");

  return 0;
}

int YAHOO_DATABASE::transaction (Command *command)
{
  // PARMS:
  // METHOD (TRANSACTION)

  _db.transaction();
  
  command->setReturnCode("0");

  return 0;
}

int YAHOO_DATABASE::commit (Command *command)
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
  YAHOO_DATABASE *o = new YAHOO_DATABASE;
  return ((ScriptPlugin *) o);
}
