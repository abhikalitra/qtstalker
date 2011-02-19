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

#include "QUOTE_DATABASE_SET.h"

#include <QtDebug>

QUOTE_DATABASE_SET::QUOTE_DATABASE_SET ()
{
  _plugin = "QUOTE_DATABASE_SET";
  _db.transaction();
}

int QUOTE_DATABASE_SET::command (Command *command)
{
  // PARMS:
  // DATE
  // OPEN
  // HIGH
  // LOW
  // CLOSE
  // VOLUME
  // INTEREST
  // EXCHANGE
  // SYMBOL
  // NAME
  // TYPE
  // SAVE
  // DATE_FORMAT

  // verify SAVE command
  QString s = command->parm("SAVE");
  if (s.length())
  {
    _db.commit();
    command->setReturnCode("0");
    return 0;
  }

  BarData symbol;
  
  // verify exchange
  s = command->parm("EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: EXCHANGE missing";
    return 1;
  }
  symbol.setExchange(s);

  // verify symbol
  s = command->parm("SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: SYMBOL missing";
    return 1;
  }
  symbol.setSymbol(s);

  // verify date
  QString format = command->parm("DATE_FORMAT");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: DATE_FORMAT missing";
    return 1;
  }

  QDateTime dt = QDateTime::fromString(command->parm("DATE"), format);
  if (! dt.isValid())
  {
    qDebug() << _plugin << "::command: invalid DATE or DATE_FORMAT" << format << command->parm("DATE");
    return 1;
  }
  
  Bar *bar = new Bar;

  if (bar->setDates(dt, dt))
  {
    qDebug() << _plugin << "::command: invalid DATE" << dt;
    delete bar;
    return 1;
  }

  // verify open
  if (bar->setOpen(command->parm("OPEN")))
  {
    qDebug() << _plugin << "::command: invalid OPEN" << command->parm("OPEN");
    delete bar;
    return 1;
  }

  // verify high
  if (bar->setHigh(command->parm("HIGH")))
  {
    qDebug() << _plugin << "::command: invalid HIGH" << command->parm("HIGH");
    delete bar;
    return 1;
  }

  // verify low
  if (bar->setLow(command->parm("LOW")))
  {
    qDebug() << _plugin << "::command: invalid LOW" << command->parm("LOW");
    delete bar;
    return 1;
  }

  // verify close
  if (bar->setClose(command->parm("CLOSE")))
  {
    qDebug() << _plugin << "::command: invalid CLOSE" << command->parm("CLOSE");
    delete bar;
    return 1;
  }

  // verify volume
  if (bar->setVolume(command->parm("VOLUME")))
  {
    qDebug() << _plugin << "::command: invalid VOLUME" << command->parm("VOLUME");
    delete bar;
    return 1;
  }

  // optional parms
  
  // verify oi
  s = command->parm("INTEREST");
  if (! s.isEmpty())
  {
    if (bar->setOI(s))
      qDebug() << _plugin << "::command: invalid INTEREST" << s;
  }

  // verify name
  s = command->parm("NAME");
  if (! s.isEmpty())
    symbol.setName(s);

  // verify type
  s = command->parm("TYPE");
  if (! s.isEmpty())
    symbol.setType(s);

  symbol.append(bar);
  
  if (_db.setBars(&symbol))
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

ScriptPlugin * createScriptPlugin ()
{
  QUOTE_DATABASE_SET *o = new QUOTE_DATABASE_SET;
  return ((ScriptPlugin *) o);
}
