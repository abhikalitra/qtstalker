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

#include "YahooAddSymbolDialog.h"
#include "Globals.h"
#include "QuoteDataBase.h"

#include <QtDebug>

YahooAddSymbolDialog::YahooAddSymbolDialog (QWidget *p) : Dialog (p)
{
  _helpFile = "Yahoo.html";
  _keySize = "yahoo_add_symbol_dialog_window_size";
  _keyPos = "yahoo_add_symbol_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << "Yahoo " + tr("Add Symbols");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  buttonStatus();

  _symbols->setFocus();
}

void YahooAddSymbolDialog::createGUI ()
{
  _symbols = new LineEdit(this);
  _symbols->setToolTip(tr("Enter Yahoo symbols separated by a space"));
  connect(_symbols, SIGNAL(textChanged(const QString &)), this, SLOT(buttonStatus()));
  _form->addRow(tr("Yahoo Symbols"), _symbols);
}

void YahooAddSymbolDialog::buttonStatus ()
{
  int status = 0;
  if (_symbols->text().length())
    status++;

  _okButton->setEnabled(status);
}

void YahooAddSymbolDialog::done ()
{
  QuoteDataBase db;
  db.transaction();

  QString s = _symbols->text();
  s = s.trimmed();
  s = s.toUpper();
  QStringList l = s.split(" ");

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    BarData bd;
    bd.setExchange("YAHOO");
    bd.setSymbol(l.at(loop));
    db.newSymbol(&bd);
  }

  db.commit();

  saveSettings();

  emit signalNew();

  accept();
}
