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
#include "YahooDataBase.h"
#include "Globals.h"
#include "YahooAddSymbolDialog.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QLayout>
#include <QDebug>
#include <QIcon>

YahooSymbolDialog::YahooSymbolDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": Yahoo " + tr("Edit Symbols"));
  
  createMainPage();
  
  loadSettings();
  
  selectionChanged();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void YahooSymbolDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(5);
  w->setLayout(hbox);

  _list = new QListWidget;
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  hbox->addWidget(_list);

  QVBoxLayout *bbox = new QVBoxLayout;
  bbox->setSpacing(2);
  bbox->setMargin(0);
  hbox->addLayout(bbox);

  QPushButton *b = new QPushButton;
  connect(b, SIGNAL(clicked()), this, SLOT(addSymbol()));
  b->setText(tr("Add..."));
  b->setIcon(QIcon(add_xpm));
  bbox->addWidget(b);
  
  _deleteButton = new QPushButton;
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteSymbol()));
  _deleteButton->setText(tr("Delete"));
  _deleteButton->setIcon(QIcon(delete_xpm));
  
  bbox->addStretch();

  _tabs->addTab(w, tr("Yahoo Symbols"));
}

void YahooSymbolDialog::addSymbol ()
{
  YahooAddSymbolDialog *dialog = new YahooAddSymbolDialog;
  connect(dialog, SIGNAL(signalNew()), this, SLOT(loadSettings()));
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  dialog->show();
}
  
void YahooSymbolDialog::deleteSymbol ()
{
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    return;
  
  YahooDataBase db;
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QListWidgetItem *item = l.at(loop);
    QString s = item->text();
    db.deleteSymbol(s);
    delete item;
  }
}

void YahooSymbolDialog::loadSettings ()
{
  _list->clear();
  
  YahooDataBase db;
  QStringList l;
  db.getSymbols(l);

  _list->addItems(l);
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

