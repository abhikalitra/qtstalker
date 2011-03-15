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

#include "QUOTE_DATABASE_SEARCH.h"
#include "QuoteDataBase.h"

#include <QtDebug>

QUOTE_DATABASE_SEARCH::QUOTE_DATABASE_SEARCH ()
{
  _plugin = "QUOTE_DATABASE_SEARCH";
}

int QUOTE_DATABASE_SEARCH::command (Command *command)
{
  // PARMS:
  // EXCHANGE
  // SYMBOL

  BarData bd;
  QString s = command->parm("EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid EXCHANGE" << command->parm("EXCHANGE");
    return 1;
  }
  bd.setExchange(s);

  s = command->parm("SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid SYMBOL" << command->parm("SYMBOL");
    return 1;
  }
  bd.setSymbol(s);

  QList<BarData> l;
  QuoteDataBase db;
  if (db.search(&bd, l))
  {
    qDebug() << _plugin << "::command: QuoteDataBase error";
    return 1;
  }

  QStringList tl;
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    BarData t = l.at(loop);
    tl << t.key();
  }
  
  // now delimit each item with a semicolon
  command->setReturnData(_plugin + "_SYMBOLS", tl.join(";"));

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  QUOTE_DATABASE_SEARCH *o = new QUOTE_DATABASE_SEARCH;
  return ((ScriptPlugin *) o);
}
