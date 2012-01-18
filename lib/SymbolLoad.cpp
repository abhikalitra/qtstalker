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


#include "SymbolLoad.h"
#include "Global.h"
#include "GlobalSymbol.h"
#include "QuoteDataBase.h"
#include "BarLength.h"
#include "SymbolKey.h"

#include <QStringList>
#include <QSettings>

SymbolLoad::SymbolLoad (QObject *p, QString symbol) : QThread (p)
{
  _symbol = symbol;

  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void SymbolLoad::run ()
{
  // do all the stuff we need to do to load a chart

  QMutexLocker locker(&g_currentSymbolMutex);

  SymbolKey skeys;
  g_currentSymbol = Symbol();
  g_currentSymbol.set(skeys.indexToString(SymbolKey::_SYMBOL), Data(_symbol));

  QSettings settings(g_localSettings);
  int length = settings.value("bar_length").toInt();
  g_currentSymbol.set(skeys.indexToString(SymbolKey::_LENGTH), Data(length));
  g_currentSymbol.set(skeys.indexToString(SymbolKey::_RANGE), Data(settings.value("date_range").toInt()));

  QuoteDataBase db;
  if (db.getBars(&g_currentSymbol))
  {
    qDebug() << "SymbolLoad::run: QuoteDataBase error";
    emit signalError();
    return;
  }

  // update settings file
  settings.setValue("current_symbol", _symbol);
  settings.sync();

  emit signalDone(getWindowCaption(length), g_currentSymbol.ekeyCount());
}

QString SymbolLoad::getWindowCaption (int length)
{
  // update the main window text
  QStringList l;
  l << "QtStalker" + g_session + ":";

  SymbolKey keys;
  Data name;
  g_currentSymbol.toData(keys.indexToString(SymbolKey::_SYMBOL), name);
  if (! name.toString().isEmpty())
    l << name.toString();

  Data symbol;
  g_currentSymbol.toData(keys.indexToString(SymbolKey::_SYMBOL), symbol);
  if (! symbol.toString().isEmpty())
    l << "(" + symbol.toString() + ")";

  QStringList bl;
  BarLength b;
  bl = b.list();
  l << bl.at(length);

  return l.join(" ");
}
