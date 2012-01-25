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

#include "SymbolDelete.h"
#include "QuoteDataBase.h"
#include "Symbol.h"
#include "DialogConfirm.h"
#include "KeySymbol.h"

#include <QtDebug>
#include <QTimer>

SymbolDelete::SymbolDelete (QObject *p, QStringList symbols) : QObject (p)
{
  _symbols = symbols;
}

void SymbolDelete::run ()
{
  QTimer::singleShot(0, this, SLOT(remove()));
}

void SymbolDelete::remove ()
{
  if (! _symbols.count())
  {
    done();
    return;
  }

  DialogConfirm *dialog = new DialogConfirm(0, QString(), Entity());
  dialog->setMessage(tr("Confirm symbol delete"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(remove2()));
  connect(dialog, SIGNAL(rejected()), this, SLOT(done()));
  dialog->show();
}

void SymbolDelete::remove2 ()
{
  QuoteDataBase db;
  db.transaction();

  KeySymbol keys;
  QStringList l2;
  int loop = 0;
  for (; loop < _symbols.size(); loop++)
  {
    Symbol bd;
    bd.set(keys.indexToString(KeySymbol::_SYMBOL), Data(_symbols.at(loop)));
    db.deleteSymbol(&bd);
  }

  db.commit();

  emit signalDone();

  done();
}

void SymbolDelete::done ()
{
  deleteLater();
}
