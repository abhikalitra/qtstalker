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

#include "QUOTE_DATABASE_RENAME.h"
#include "QuoteDataBase.h"

#include <QtDebug>

QUOTE_DATABASE_RENAME::QUOTE_DATABASE_RENAME ()
{
  _plugin = "QUOTE_DATABASE_RENAME";
}

int QUOTE_DATABASE_RENAME::command (Command *command)
{
  // PARMS:
  // OLD_EXCHANGE
  // OLD_SYMBOL
  // NEW_EXCHANGE
  // NEW_SYMBOL

  BarData osymbol;
  QString s = command->parm("OLD_EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid OLD_EXCHANGE" << command->parm("OLD_EXCHANGE");
    return 1;
  }
  osymbol.setExchange(s);

  s = command->parm("OLD_SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid OLD_SYMBOL" << command->parm("OLD_SYMBOL");
    return 1;
  }
  osymbol.setSymbol(s);

  BarData nsymbol;
  s = command->parm("NEW_EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid NEW_EXCHANGE" << command->parm("NEW_EXCHANGE");
    return 1;
  }
  nsymbol.setExchange(s);

  s = command->parm("NEW_SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid NEW_SYMBOL" << command->parm("NEW_SYMBOL");
    return 1;
  }
  nsymbol.setSymbol(s);

  QuoteDataBase db;
  if (db.rename(&osymbol, &nsymbol))
  {
    qDebug() << _plugin << "::command: QuoteDataBase error";
    return 1;
  }

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  QUOTE_DATABASE_RENAME *o = new QUOTE_DATABASE_RENAME;
  return ((Plugin *) o);
}
