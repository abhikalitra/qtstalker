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

#include "SymbolFunctions.h"
#include "QuoteDataBase.h"
#include "Symbol.h"
#include "KeySymbol.h"
#include "GlobalSymbol.h"
#include "Global.h"

#include <QtDebug>
#include <QMutexLocker>
#include <QSettings>

SymbolFunctions::SymbolFunctions ()
{
}

int SymbolFunctions::load (QString symbol)
{
  QMutexLocker locker(&g_currentSymbolMutex);

  KeySymbol skeys;
  g_currentSymbol = Symbol();
  g_currentSymbol.set(skeys.indexToString(KeySymbol::_SYMBOL), Data(symbol));

  QSettings settings(g_localSettings);
  int length = settings.value("bar_length").toInt();
  g_currentSymbol.set(skeys.indexToString(KeySymbol::_LENGTH), Data(length));
  g_currentSymbol.set(skeys.indexToString(KeySymbol::_RANGE), Data(settings.value("date_range").toInt()));

  QuoteDataBase db;
  if (db.getBars(g_currentSymbol))
  {
    qDebug() << "SymbolLoad::run: QuoteDataBase error";
    return 1;
  }

  // update settings file
  settings.setValue("current_symbol", symbol);
  settings.sync();

  return 0;
}

int SymbolFunctions::remove (QStringList l)
{
  QuoteDataBase db;
  db.transaction();

  KeySymbol keys;
  int loop = 0;
  for (; loop < l.size(); loop++)
  {
    Symbol bd;
    bd.set(keys.indexToString(KeySymbol::_SYMBOL), Data(l.at(loop)));
    db.deleteSymbol(&bd);
  }

  db.commit();

  return 0;
}
