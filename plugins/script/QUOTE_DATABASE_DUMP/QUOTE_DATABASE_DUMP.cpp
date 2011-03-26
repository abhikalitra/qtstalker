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

#include "QUOTE_DATABASE_DUMP.h"
#include "Globals.h"
#include "QuoteDataBase.h"
#include "DateRange.h"
#include "BarLength.h"
#include "Strip.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QtDebug>

QUOTE_DATABASE_DUMP::QUOTE_DATABASE_DUMP ()
{
  _plugin = "QUOTE_DATABASE_DUMP";
}

int QUOTE_DATABASE_DUMP::command (Command *command)
{
  // PARMS:
  // EXCHANGE
  // SYMBOL
  // DIRECTORY

  BarData bd;
  QString s = command->parm("EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid exchange" << s;
    return 1;
  }
  bd.setExchange(s);

  s = command->parm("SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid symbol" << s;
    return 1;
  }
  bd.setSymbol(s);

  bd.setBarLength(BarLength::_NONE);
  bd.setRange(DateRange::_ALL);

  // create base directory if needed
  QFileInfo fi(command->parm("DIRECTORY"));
  QString path = fi.absolutePath();
  
  QDir dir;
  if (! dir.mkpath(path))
  {
    qDebug() << _plugin << "::command: error creating directory" << path;
    return 1;
  }

  // create exchange directory
  path.append("/");
  path.append(bd.exchange());
  if (! dir.mkpath(path))
  {
    qDebug() << _plugin << "::command: error creating directory" << path;
    return 1;
  }

  path.append("/");
  path.append(bd.symbol());
  
  QFile file(path);
  if (! file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    qDebug() << _plugin << "::command: error opening csv file" << path;
    return 1;
  }
  QTextStream out(&file);

  QuoteDataBase db;
  if (db.getBars(&bd))
  {
    qDebug() << _plugin << "::command: QuoteDataBase error";
    return 1;
  }

  Strip strip;
  int loop = 0;
  for (; loop < bd.count(); loop++)
  {
    Bar *b = bd.bar(loop);
    if (! b)
      continue;

    QStringList l;
    l << bd.exchange() << bd.symbol() << bd.name();

    b->dateTimeString(s);
    l << s;

    strip.strip(b->open(), 4, s);
    l << s;
    
    strip.strip(b->high(), 4, s);
    l << s;

    strip.strip(b->low(), 4, s);
    l << s;

    strip.strip(b->close(), 4, s);
    l << s;

    strip.strip(b->volume(), 4, s);
    l << s;

    strip.strip(b->oi(), 4, s);
    l << s;

    out << l.join(",") << "\n";
  }

  file.close();
  
  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  QUOTE_DATABASE_DUMP *o = new QUOTE_DATABASE_DUMP;
  return ((Plugin *) o);
}
