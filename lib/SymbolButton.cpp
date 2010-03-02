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

SymbolButton::SymbolButton (QWidget *w) : QPushButton (w)
{
  QObject::connect(this, SIGNAL(clicked()), this, SLOT(symbolDialog()));
  setMaximumHeight(25);
  updateButtonText();
}

void SymbolButton::getSymbols (QStringList &l)
{
  l = symbolList;
}

void SymbolButton::setSymbols (QStringList &l)
{
  symbolList = l;
  updateButtonText();
}

void SymbolButton::symbolDialog ()
{
  SymbolDialog *dialog = new SymbolDialog;
  dialog->setSymbols(symbolList);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  dialog->getSymbols(symbolList);

  updateButtonText();
  
  delete dialog;  
}

void SymbolButton::updateButtonText ()
{
  setText(QString::number(symbolList.count()) + " " + tr("Symbols"));
}


