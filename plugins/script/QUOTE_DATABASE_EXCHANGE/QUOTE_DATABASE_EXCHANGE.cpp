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

#include "QUOTE_DATABASE_EXCHANGE.h"
#include "QuoteDataBase.h"

#include <QtDebug>

QUOTE_DATABASE_EXCHANGE::QUOTE_DATABASE_EXCHANGE ()
{
  _plugin = "QUOTE_DATABASE_EXCHANGE";
}

int QUOTE_DATABASE_EXCHANGE::command (Command *command)
{
  // PARMS:
  // EXCHANGE

  QStringList l;
  QuoteDataBase db;
  if (db.getExchange(l))
  {
    qDebug() << _plugin << "::command: QuoteDataBase error";
    return 1;
  }

  command->setReturnData(_plugin + "_EXCHANGES", l.join(","));

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  QUOTE_DATABASE_EXCHANGE *o = new QUOTE_DATABASE_EXCHANGE;
  return ((Plugin *) o);
}
