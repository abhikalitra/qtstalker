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

#include "GroupEditDialog.h"
#include "Globals.h"
#include "GroupDataBase.h"
#include "SymbolDialog.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"
#include "../pics/select_all.xpm"
#include "../pics/unselect_all.xpm"

#include <QtDebug>
#include <QSettings>
#include <QToolBar>

GroupEditDialog::GroupEditDialog (QWidget *p, QString n) : Dialog (p)
{
  _name = n;
  _keySize = "group_edit_dialog_window_size";
  _keyPos = "group_edit_dialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("Edit Group") << _name;
  setWindowTitle(l.join(" "));

  createGUI();

  GroupDataBase db;
  db.load(_name, l);
  
  _list->clear();
  _list->addItems(l);

  loadSettings();

  selectionChanged();
}

void GroupEditDialog::createGUI ()
{
  int pos = 0;
  QLabel *label = new QLabel(tr("Group Symbols"));
  _vbox->insertWidget(pos++, label);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(0);
  _vbox->insertLayout(pos++, hbox);

  _list = new QListWidget;
  _list->setSortingEnabled(TRUE);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  hbox->addWidget(_list);

  QToolBar *tb = new QToolBar;
  tb->setOrientation(Qt::Vertical);
  hbox->addWidget(tb);

  QToolButton *b = new QToolButton;
  b->setIcon(QIcon(add_xpm));
  b->setToolTip(tr("Add Symbols"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(addButtonPressed()));
  tb->addWidget(b);
  
  _deleteButton = new QToolButton;
  _deleteButton->setIcon(QIcon(delete_xpm));
  _deleteButton->setToolTip(tr("Delete Symbols"));
  connect(_deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteButtonPressed()));
  tb->addWidget(_deleteButton);

  // select all button
  b = new QToolButton;
  b->setIcon(QIcon(select_all_xpm));
  b->setToolTip(tr("Select All"));
  connect(b, SIGNAL(clicked(bool)), _list, SLOT(selectAll()));
  tb->addWidget(b);

  // unselect all button
  b = new QToolButton;
  b->setIcon(QIcon(unselect_all_xpm));
  b->setToolTip(tr("Unselect All"));
  connect(b, SIGNAL(clicked(bool)), _list, SLOT(clearSelection()));
  tb->addWidget(b);

  // clear some unused space
  _message->hide();
}

void GroupEditDialog::selectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (sl.count())
    status = 1;

  _deleteButton->setEnabled(status);
}

void GroupEditDialog::done ()
{
  QStringList l;
  int loop = 0;
  for (; loop < _list->count(); loop++)
    l << _list->item(loop)->text();

  GroupDataBase db;
  if (db.saveAll(_name, l))
  {
    qDebug() << "GroupEditDialog::done: GroupDataBase error";
    cancel();
    return;
  }

  saveSettings();

  accept();
}

void GroupEditDialog::addButtonPressed ()
{
  SymbolDialog *dialog = new SymbolDialog(this);
  connect(dialog, SIGNAL(signalDone(QString, QString, QStringList)), this, SLOT(addButtonPressed2(QString, QString, QStringList)));
  dialog->show();
}

void GroupEditDialog::addButtonPressed2 (QString, QString, QStringList l)
{
  _list->addItems(l);
}

void GroupEditDialog::deleteButtonPressed ()
{
  QList<QListWidgetItem *> l = _list->selectedItems();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QListWidgetItem *item = l.at(loop);
    delete item;
  }
}
