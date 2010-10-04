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

#include "SymbolButton.h"
#include "SymbolDialog.h"

SymbolButton::SymbolButton (QWidget *w) : QPushButton (w)
{
  connect(this, SIGNAL(clicked()), this, SLOT(symbolDialog()));
  updateButtonText();
}

Group & SymbolButton::getSymbols ()
{
  return _symbols;
}

void SymbolButton::setSymbols (QString &ex, QString &ss)
{
  _exchangeSearch = ex;
  _symbolSearch = ss;
  updateButtonText();
}

void SymbolButton::setSymbols (Group &d)
{
  _symbols = d;
  updateButtonText();
}

void SymbolButton::symbolDialog ()
{
  SymbolDialog *dialog = new SymbolDialog;
  dialog->setSymbols(_exchangeSearch, _symbolSearch);
  connect(dialog, SIGNAL(signalSymbols(Group)), this, SLOT(symbolDialog2(Group)));
  dialog->show();
}

void SymbolButton::symbolDialog2 (Group g)
{
  _symbols = g;
  updateButtonText();
}

void SymbolButton::updateButtonText ()
{
  setText(QString::number(_symbols.count()) + " " + tr("Symbols"));
}

void SymbolButton::symbolList (QStringList &l)
{
  l.clear();
  _symbols.getStringList(l);
}

void SymbolButton::setSymbolList (QStringList &l)
{
  _symbols.setStringList(l);
  updateButtonText();
}

