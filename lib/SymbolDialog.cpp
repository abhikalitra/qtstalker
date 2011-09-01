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
#include "Symbol.h"
#include "../pics/search.xpm"

#include <QtDebug>
#include <QTreeWidgetItem>
#include <QGroupBox>

SymbolDialog::SymbolDialog (QWidget *p) : Dialog (p)
{
  _keySize = "symbol_dialog_window_size";
  _keyPos = "symbol_dialog_window_position";

  setWindowTitle("Qtstalker: " + tr("Select Symbols"));

  createGUI();

  loadExchanges();

  loadSettings();

  searchSelectionChanged();
  symbolSelectionChanged();

  _search->setFocus();
}

void SymbolDialog::createGUI ()
{
  _exchanges = new QComboBox;
  _exchanges->setToolTip(tr("Select a specific exchange or * for all"));
  _form->addRow(tr("Exchange"), _exchanges);

  _search = new LineEdit(this);
  _search->setText("*");
  _search->setToolTip(tr("Enter a partial search like %OOG% or * for all"));
  _form->addRow(tr("Symbol pattern"), _search);

  _searchButton = new QPushButton;
  _searchButton->setIcon(QIcon(search_xpm));
  _searchButton->setToolTip(tr("Perform search"));
  connect(_searchButton, SIGNAL(clicked()), this, SLOT(searchButtonPressed()));
  _form->addRow(tr("Search"), _searchButton);

  int pos = 1;
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
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

  _searchList = new QTreeWidget;
  _searchList->setSortingEnabled(FALSE);
  _searchList->setRootIsDecorated(FALSE);
  _searchList->setHeaderLabels(l);
  _searchList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_searchList, SIGNAL(itemSelectionChanged()), this, SLOT(searchSelectionChanged()));
//  connect(_searchList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  tvbox->addWidget(_searchList);

  tvbox = new QVBoxLayout;
  tvbox->setSpacing(2);
  hbox->addLayout(tvbox);

  tvbox->addStretch(1);

  QPushButton *b = new QPushButton;
  b->setText(tr("Select"));
  b->setToolTip(tr("Select All Items"));
  connect(b, SIGNAL(clicked()), _searchList, SLOT(selectAll()));
  tvbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("Unselect"));
  b->setToolTip(tr("Unselect All Items"));
  connect(b, SIGNAL(clicked()), _searchList, SLOT(clearSelection()));
  tvbox->addWidget(b);

  _addButton = new QPushButton;
  _addButton->setText(">>");
  connect(_addButton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
  tvbox->addWidget(_addButton);

  _deleteButton = new QPushButton;
  _deleteButton->setText("<<");
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteButtonPressed()));
  tvbox->addWidget(_deleteButton);

  tvbox->addStretch(1);

  gbox = new QGroupBox;
  gbox->setTitle(tr("Symbols"));
  hbox->addWidget(gbox);

  tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(0);
  gbox->setLayout(tvbox);

  _symbolList = new QTreeWidget;
  _symbolList->setSortingEnabled(FALSE);
  _symbolList->setRootIsDecorated(FALSE);
  _symbolList->setHeaderLabels(l);
  _symbolList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_symbolList, SIGNAL(itemSelectionChanged()), this, SLOT(symbolSelectionChanged()));
//  connect(_symbolList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  tvbox->addWidget(_symbolList);
}

void SymbolDialog::searchSelectionChanged ()
{
  int status = 0;
  QList<QTreeWidgetItem *> sl = _searchList->selectedItems();
  if (sl.count())
    status = 1;

  _addButton->setEnabled(status);
}

void SymbolDialog::symbolSelectionChanged ()
{
  int status = 0;
  QList<QTreeWidgetItem *> sl = _symbolList->selectedItems();
  if (sl.count())
    status = 1;

  _deleteButton->setEnabled(status);
}

void SymbolDialog::done ()
{
  saveSettings();

  _symbols.clear();
  int loop = 0;
  for (; loop < _symbolList->topLevelItemCount(); loop++)
    _symbols << _symbolList->topLevelItem(loop)->text(0);

  emit signalDone(_exchanges->currentText(), _search->text(), _symbols);

  accept();
}

void SymbolDialog::addButtonPressed ()
{
  QList<QTreeWidgetItem *> sl = _searchList->selectedItems();

  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QTreeWidgetItem *item = sl.at(loop);

    QTreeWidgetItem *nitem = new QTreeWidgetItem(_symbolList);
    nitem->setText(0, item->text(0));
    nitem->setText(1, item->text(1));
  }

  for (loop = 0; loop < _symbolList->columnCount(); loop++)
    _symbolList->resizeColumnToContents(loop);
}

void SymbolDialog::deleteButtonPressed ()
{
  QList<QTreeWidgetItem *> sl = _symbolList->selectedItems();

  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QTreeWidgetItem *item = sl.at(loop);
    delete item;
  }

  for (loop = 0; loop < _symbolList->columnCount(); loop++)
    _symbolList->resizeColumnToContents(loop);

  symbolSelectionChanged();
}

void SymbolDialog::searchButtonPressed ()
{
  Data *symbol = new Symbol;
  symbol->set(Symbol::_EXCHANGE, _exchanges->currentText());
  QString s = _search->text();
  if (s.isEmpty())
    s = "*";
  symbol->set(Symbol::_SYMBOL, s);

  QuoteDataBase db;
  QList<Data *> l;
  db.search(symbol, l);
  delete symbol;

  _searchList->clear();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    Data *bd = l.at(loop);
    QTreeWidgetItem *item = new QTreeWidgetItem(_searchList);
    item->setText(0, bd->get(Symbol::_EXCHANGE) + ":" + bd->get(Symbol::_SYMBOL));
    item->setText(1, bd->get(Symbol::_NAME));
  }

  qDeleteAll(l);

  for (loop = 0; loop < _searchList->columnCount(); loop++)
    _searchList->resizeColumnToContents(loop);
}

void SymbolDialog::loadExchanges ()
{
  QuoteDataBase db;
  QStringList l;
  db.getExchange(l);

  l.prepend("*");

  _exchanges->clear();
  _exchanges->addItems(l);
  _exchanges->setCurrentIndex(0);
}

QStringList SymbolDialog::symbols ()
{
  return _symbols;
}
