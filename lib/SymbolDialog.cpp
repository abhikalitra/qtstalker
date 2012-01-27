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

#include "SymbolDialog.h"
#include "QuoteDataBase.h"
#include "KeySymbol.h"
#include "WindowTitle.h"

#include "../pics/search.xpm"
#include "../pics/select_all.xpm"
#include "../pics/unselect_all.xpm"
#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QtDebug>
#include <QGroupBox>
#include <QToolBar>

SymbolDialog::SymbolDialog (QWidget *p) : Dialog (p)
{
  _keySize = "symbol_dialog_window_size";
  _keyPos = "symbol_dialog_window_position";

  WindowTitle wt;
  setWindowTitle(wt.title(tr("Select Symbols"), QString()));

  createGUI();

  loadSettings();

  searchSelectionChanged();
  symbolSelectionChanged();

  _search->setFocus();
  
  okButtonStatus();
}

void SymbolDialog::createGUI ()
{
  // search area
  int pos = 1;
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(0);
  _vbox->insertLayout(pos++, hbox);
  
  // search edit
  _search = new WidgetLineEdit(this);
  _search->setText("%");
  _search->setToolTip(tr("Enter a partial search like %OOG% or % for all"));
  hbox->addWidget(_search, 1);

  // search button
  _searchButton = new QPushButton;
  _searchButton->setIcon(QIcon(search_xpm));
  _searchButton->setToolTip(tr("Perform search"));
  connect(_searchButton, SIGNAL(clicked()), this, SLOT(searchButtonPressed()));
  connect(_search, SIGNAL(signalStatus(bool)), _searchButton, SLOT(setEnabled(bool)));
  hbox->addWidget(_searchButton);
  
  // list area
  hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(0);
  _vbox->insertLayout(pos++, hbox);

  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Search Results"));
  hbox->addWidget(gbox);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(0);
  gbox->setLayout(tvbox);

  QStringList l;
  l << tr("Symbol") << tr("Name");

  _searchList = new SymbolListWidget;
  _searchList->setSortingEnabled(FALSE);
  _searchList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_searchList, SIGNAL(itemSelectionChanged()), this, SLOT(searchSelectionChanged()));
//  connect(_searchList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  tvbox->addWidget(_searchList);

  tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(0);
  hbox->addLayout(tvbox);
  tvbox->addStretch(1);

  QToolBar *tb = new QToolBar;
  tb->setOrientation(Qt::Vertical);
  tvbox->addWidget(tb);

  _addButton = new QToolButton;
  _addButton->setIcon(QIcon(add_xpm));
  _addButton->setToolTip(tr("Add selected to symbols"));
  connect(_addButton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
  tb->addWidget(_addButton);

  _deleteButton = new QToolButton;
  _deleteButton->setIcon(QIcon(delete_xpm));
  _deleteButton->setToolTip(tr("Remove selected from symbols"));
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteButtonPressed()));
  tb->addWidget(_deleteButton);

  QToolButton *b = new QToolButton;
  b->setIcon(QIcon(select_all_xpm));
  b->setToolTip(tr("Select all symbols"));
  connect(b, SIGNAL(clicked()), _searchList, SLOT(selectAll()));
  tb->addWidget(b);

  b = new QToolButton;
  b->setIcon(QIcon(unselect_all_xpm));
  b->setToolTip(tr("Unselect all symbols"));
  connect(b, SIGNAL(clicked()), _searchList, SLOT(clearSelection()));
  tb->addWidget(b);

  tvbox->addStretch(1);

  gbox = new QGroupBox;
  gbox->setTitle(tr("Selected Symbols"));
  hbox->addWidget(gbox);

  tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(0);
  gbox->setLayout(tvbox);

  _symbolList = new SymbolListWidget;
  _symbolList->setSortingEnabled(FALSE);
  _symbolList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_symbolList, SIGNAL(itemSelectionChanged()), this, SLOT(symbolSelectionChanged()));
  tvbox->addWidget(_symbolList);
  
  // save some unused space
  _message->hide();
}

void SymbolDialog::searchSelectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> sl = _searchList->selectedItems();
  if (sl.count())
    status = 1;

  _addButton->setEnabled(status);
}

void SymbolDialog::symbolSelectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> sl = _symbolList->selectedItems();
  if (sl.count())
    status = 1;

  _deleteButton->setEnabled(status);
}

void SymbolDialog::done ()
{
  saveSettings();

  _symbols.clear();
  int loop = 0;
  for (; loop < _symbolList->count(); loop++)
    _symbols << _symbolList->item(loop)->text();

  emit signalDone(QString(), _search->text(), _symbols);

  accept();
}

void SymbolDialog::addButtonPressed ()
{
  QList<QListWidgetItem *> sl = _searchList->selectedItems();

  KeySymbol keys;
  int loop = 0;
  for (; loop < sl.size(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);
    
    Symbol symbol;
    symbol.set(keys.indexToString(KeySymbol::_SYMBOL), Data(item->text()));
    symbol.set(keys.indexToString(KeySymbol::_NAME), Data(item->toolTip()));
    
    _symbolList->addSymbol(symbol);
  }
  
  okButtonStatus();
}

void SymbolDialog::deleteButtonPressed ()
{
  QList<QListWidgetItem *> sl = _symbolList->selectedItems();

  int loop = 0;
  for (; loop < sl.size(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);
    delete item;
  }

  symbolSelectionChanged();
  okButtonStatus();
}

void SymbolDialog::searchButtonPressed ()
{
  QuoteDataBase db;
  QList<Symbol> l;
  db.search(_search->text(), l);
  
  _searchList->setSymbols(l);
}

QStringList SymbolDialog::symbols ()
{
  return _symbols;
}

void SymbolDialog::okButtonStatus ()
{
  _okButton->setEnabled((bool) _symbolList->count());
}

