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
#include "../pics/search.xpm"
#include "../pics/select_all.xpm"
#include "../pics/unselect_all.xpm"

#include <QDebug>
#include <QIcon>
#include <QSettings>
#include <QToolBar>

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

  QToolBar *tb = new QToolBar;
  _vbox->insertWidget(pos++, tb);

  _list = new QListWidget;
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  _vbox->insertWidget(pos++, _list);

  QLabel *label = new QLabel(tr("Search"));
  tb->addWidget(label);

  _search = new QLineEdit;
  _search->setToolTip(tr("Search Pattern"));
  tb->addWidget(_search);

  QToolButton *b = new QToolButton;
  connect(b, SIGNAL(clicked()), this, SLOT(search()));
  b->setToolTip(tr("Perform Search"));
  b->setIcon(QIcon(search_xpm));
  tb->addWidget(b);

  b = new QToolButton;
  connect(b, SIGNAL(clicked()), this, SLOT(addSymbol()));
  b->setToolTip(tr("Add Symbol..."));
  b->setIcon(QIcon(add_xpm));
  tb->addWidget(b);

  _deleteButton = new QToolButton;
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteSymbol()));
  _deleteButton->setToolTip(tr("Delete Symbol"));
  _deleteButton->setIcon(QIcon(delete_xpm));
  tb->addWidget(_deleteButton);

  b = new QToolButton;
  connect(b, SIGNAL(clicked()), _list, SLOT(selectAll()));
  b->setToolTip(tr("Select All"));
  b->setIcon(QIcon(select_all_xpm));
  tb->addWidget(b);

  b = new QToolButton;
  connect(b, SIGNAL(clicked()), _list, SLOT(clearSelection()));
  b->setToolTip(tr("Unselect All"));
  b->setIcon(QIcon(unselect_all_xpm));
  tb->addWidget(b);
}

void YahooSymbolDialog::addSymbol ()
{
  YahooAddSymbolDialog *dialog = new YahooAddSymbolDialog(this);
  connect(dialog, SIGNAL(signalNew()), this, SLOT(search()));
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
    QListWidgetItem *item = l.at(loop);
    
    Setting symbol;
    symbol.setData("SYMBOL", item->text());
    db.deleteSymbol(symbol);
    
    delete item;
  }

  db.commit();
}

void YahooSymbolDialog::loadSettings ()
{
  Dialog::loadSettings();

  search();
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

void YahooSymbolDialog::search ()
{
  _list->clear();

  YahooDataBase db;
  QStringList l;
  if (db.search(_search->text(), l))
  {
    qDebug() << "YahooSymbolDialog::search: db error";
    return;
  }
  
  _list->addItems(l);

  QStringList ml;
  ml << QString::number(l.count()) << tr("Symbols");
  _message->setText(ml.join(" "));
}
