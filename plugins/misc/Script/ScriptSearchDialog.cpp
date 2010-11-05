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

#include "ScriptSearchDialog.h"
#include "ScriptDataBase.h"
#include "Globals.h"

#include "../pics/search.xpm"

#include <QtDebug>
#include <QPushButton>
#include <QFormLayout>

ScriptSearchDialog::ScriptSearchDialog ()
{
  QString s = "Qtstalker" + g_session + ": " + tr("Select Script");
  setWindowTitle(s);

  createMainPage();

  selectionChanged();

  // set the default button
  QPushButton *b = _buttonBox->button(QDialogButtonBox::Cancel);
  b->setDefault(TRUE);

  searchButtonPressed();
}

void ScriptSearchDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  w->setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setMargin(0);
  form->setSpacing(2);
  vbox->addLayout(form);

  _search = new QLineEdit("*");
  _search->setToolTip(tr("Enter a partial search like %import% or * for all"));
  form->addRow(tr("Search For"), _search);

  QPushButton *b = new QPushButton;
  b->setIcon(QIcon(search_xpm));
  b->setToolTip(tr("Perform search"));
  connect(b, SIGNAL(clicked()), this, SLOT(searchButtonPressed()));
  form->addRow(tr("Perform Search"), b);

  _list = new QListWidget;
  _list->setSortingEnabled(TRUE);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  vbox->addWidget(_list);
  
  _tabs->addTab(w, tr("Items"));
}

void ScriptSearchDialog::selectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (sl.count())
    status = 1;

  QPushButton *b = _buttonBox->button(QDialogButtonBox::Ok);
  b->setEnabled(status);
}

void ScriptSearchDialog::searchButtonPressed ()
{
  ScriptDataBase db;
  QStringList l;
  db.getScriptSearch(_search->text(), l);

  _list->clear();
  _list->addItems(l);
}

void ScriptSearchDialog::done ()
{
  QList<QListWidgetItem *> sel = _list->selectedItems();
  if (sel.count())
  {
    QStringList l;
    int loop = 0;
    for (; loop < sel.count(); loop++)
    {
      QListWidgetItem *item = sel.at(loop);
      l.append(item->text());
    }

    emit signalSelect(l);
  }

  accept();
}

