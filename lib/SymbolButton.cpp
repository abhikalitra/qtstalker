/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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




SymbolButton::SymbolButton (QWidget *w, QStringList &l) : QPushButton (w)
{
  QObject::connect(this, SIGNAL(clicked()), this, SLOT(fileDialog()));
  setMaximumHeight(25);
  setSymbols(l);
}

void SymbolButton::getSymbols (QStringList &l)
{
  l = symbols;
}

void SymbolButton::fileDialog ()
{
  SymbolDialog *dialog = new SymbolDialog(this, symbols);

  int rc = dialog->exec();

  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  QStringList l;
  dialog->getSymbols(l);
  setSymbols(l);
  emit symbolChanged();

  delete dialog;
}

void SymbolButton::setSymbols (QStringList &l)
{
  symbols = l;
  setText(QString::number(symbols.count()) + tr(" Symbols"));
}



