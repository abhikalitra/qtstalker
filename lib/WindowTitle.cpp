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

#include "WindowTitle.h"
#include "Global.h"
#include "GlobalSymbol.h"
#include "KeySymbol.h"
#include "TypeBarLength.h"

#include <QStringList>
#include <QObject>
#include <QtDebug>

WindowTitle::WindowTitle ()
{
}

QString WindowTitle::title (QString name, QString item)
{
  QStringList l;
  l << "QtStalker:" + g_session;
  l << "-";
  l << name;
  
  if (! item.isEmpty())
    l << "(" + item + ")";
  
  return l.join(" ");
}

QString WindowTitle::mainWindowTitle (int length)
{
  QStringList l;
  l << "QtStalker:" + g_session;
  l << "-";
  
  KeySymbol keys;
  Data name;
  g_currentSymbol.toData(keys.indexToString(KeySymbol::_NAME), name);
  if (! name.toString().isEmpty())
    l << name.toString();

  Data symbol;
  g_currentSymbol.toData(keys.indexToString(KeySymbol::_SYMBOL), symbol);
  if (! symbol.toString().isEmpty())
    l << "(" + symbol.toString() + ")";

  TypeBarLength b;
  l << b.indexToString(length);

  return l.join(" ");
}

QString WindowTitle::dataWindowTitle ()
{
  KeySymbol keys;
  Data symbol, name;
  g_currentSymbol.toData(keys.indexToString(KeySymbol::_SYMBOL), symbol);
  g_currentSymbol.toData(keys.indexToString(KeySymbol::_SYMBOL), name);
  
  QStringList l;
  l << "QtStalker:" + g_session;
  l << "-";
  l << symbol.toString();
  l << "(" + name.toString() + ")";
  l << "-";
  l << QObject::tr("Indicators");

  return l.join(" ");
}
