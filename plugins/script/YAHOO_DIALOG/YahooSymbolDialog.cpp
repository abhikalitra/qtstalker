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

#include "YahooSymbolDialog.h"
#include "Globals.h"
#include "YahooAddSymbolDialog.h"
#include "YahooDataBase.h"
#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QDebug>
#include <QIcon>
#include <QSettings>

YahooSymbolDialog::YahooSymbolDialog (QWidget *p) : Dialog (p)
{
  _helpFile = "Yahoo.html";
  _keySize = "yahoo_symbol_dialog_window_size";
  _keyPos = "yahoo_symbol_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << "Yahoo Symbols";
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  selectionChanged();
}

void YahooSymbolDialog::createGUI ()
{
  int pos = 0;
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(5);
  _vbox->insertLayout(pos++, hbox);

  _list = new QListWidget;
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  hbox->addWidget(_list);

  QVBoxLayout *tbbox = new QVBoxLayout;
  tbbox->setSpacing(2);
  tbbox->setMargin(0);
  hbox->addLayout(tbbox);

  QPushButton *b = new QPushButton;
  connect(b, SIGNAL(clicked()), this, SLOT(addSymbol()));
  b->setText(tr("Add..."));
  b->setIcon(QIcon(add_xpm));
  tbbox->addWidget(b);
  
  _deleteButton = new QPushButton;
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteSymbol()));
  _deleteButton->setText(tr("Delete"));
  _deleteButton->setIcon(QIcon(delete_xpm));
  tbbox->addWidget(_deleteButton);
  
  tbbox->addStretch();
}

void YahooSymbolDialog::addSymbol ()
{
  YahooAddSymbolDialog *dialog = new YahooAddSymbolDialog(this);
  connect(dialog, SIGNAL(signalNew()), this, SLOT(loadSettings()));
  dialog->show();
}
  
void YahooSymbolDialog::deleteSymbol ()
{
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    return;

  YahooDataBase db;
  db.transaction();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    Setting symbol;
    symbol.setData("YSYMBOL", l.at(loop)->text());
    db.deleteSymbol(symbol);
  }

  db.commit();
}

void YahooSymbolDialog::loadSettings ()
{
  _list->clear();
  
  YahooDataBase db;
  QStringList l;
  db.symbols(l);
  _list->addItems(l);

  Dialog::loadSettings();
}

void YahooSymbolDialog::done ()
{
  QList<QListWidgetItem *> sel = _list->selectedItems();
  if (! sel.count())
  {
    accept();
    return;
  }

  QStringList l;
  int loop = 0;
  for (; loop < sel.count(); loop++)
    l.append(sel.at(loop)->text());

  emit signalSymbols(l);

  saveSettings();

  accept();
}

void YahooSymbolDialog::selectionChanged ()
{
  QList<QListWidgetItem *> sel = _list->selectedItems();
  if (! sel.count())
    _deleteButton->setEnabled(FALSE);
  else
    _deleteButton->setEnabled(TRUE);
}
