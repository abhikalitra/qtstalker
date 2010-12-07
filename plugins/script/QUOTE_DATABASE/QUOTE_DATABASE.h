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

#ifndef QUOTE_DATABASE_HPP
#define QUOTE_DATABASE_HPP

#include <QtSql>

#include "ScriptPlugin.h"
#include "Symbol.h"

class QUOTE_DATABASE : public ScriptPlugin
{
  public:
    enum Method
    {
      _GET,
      _SET,
      _DELETE,
      _EXCHANGE,
      _RENAME,
      _SEARCH,
      _TRANSACTION,
      _COMMIT
    };
    
    QUOTE_DATABASE ();
    void init ();
    int command (Command *);
    int getBars (Command *);
    int setBars (Command *);
    int newSymbol (Symbol *);
    int getSymbol (Symbol *);
    int transaction (Command *);
    int commit (Command *);
    int deleteSymbol (Command *);
    int getExchange (Command *);
    int rename (Command *);
    int search (Command *);

  private:
    QStringList _method;
    QSqlDatabase _db;
};

extern "C"
{
  ScriptPlugin * createScriptPlugin ();
}

#endif
