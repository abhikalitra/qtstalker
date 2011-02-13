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

#include "QUOTE_DATABASE_DELETE.h"
#include "QuoteDataBase.h"

#include <QtDebug>

QUOTE_DATABASE_DELETE::QUOTE_DATABASE_DELETE ()
{
  _plugin = "QUOTE_DATABASE_DELETE";
}

int QUOTE_DATABASE_DELETE::command (Command *command)
{
  // PARMS:
  // NAME - semicolon delimited EXCHANGE:SYMBOL list

  QStringList l = command->parm("NAME").split(";");

  QuoteDataBase db;
  db.transaction();
  
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    BarData bd;
    if (bd.setKey(l.at(loop)))
    {
      qDebug() << _plugin << "::command: invalid EXCHANGE:SYMBOL" << l.at(loop);
      continue;
    }
    
    if (db.deleteSymbol(&bd))
      qDebug() << _plugin << "::command: QuoteDataBase error deleting" << l.at(loop);
  }

  db.commit();
  
  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  QUOTE_DATABASE_DELETE *o = new QUOTE_DATABASE_DELETE;
  return ((ScriptPlugin *) o);
}
