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

#ifndef YAHOO_DATABASE_HPP
#define YAHOO_DATABASE_HPP

#include <QtSql>

#include "ScriptPlugin.h"

class YAHOO_DATABASE : public ScriptPlugin
{
  Q_OBJECT
  
  public:
    enum Method
    {
      _LOAD,
      _SAVE,
      _DELETE,
      _SYMBOLS,
      _TRANSACTION,
      _COMMIT
    };
    
    YAHOO_DATABASE ();
    int command (Command *);
    int load (Command *);
    int save (Command *);
    int deleteSymbol (Command *);
    int symbols (Command *);
    int transaction (Command *);
    int commit (Command *);

  private:
    QStringList _method;
    QSqlDatabase _db;
};

extern "C"
{
  ScriptPlugin * createScriptPlugin ();
}

#endif
