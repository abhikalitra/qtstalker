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
#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QtDebug>
#include <QSettings>

GroupEditDialog::GroupEditDialog (QWidget *p, Command *command) : Dialog (p)
{
  _command = command;
  _name = _command->parm("NAME");
  _keySize = "group_edit_dialog_window_size";
  _keyPos = "group_edit_dialog_window_position";

  setWindowTitle("QtStalker" + g_session + ": " + tr("Edit Group"));

  createGUI();

  QStringList l;
  GroupDataBase db;
  db.load(_name, l);
  
  _list->clear();
  _list->addItems(l);

  loadSettings();

  selectionChanged();

  _symbolDialogCommand = new Command("PLUGIN=SYMBOL_DIALOG,FLAG=0");
}

GroupEditDialog::~GroupEditDialog ()
{
  delete _symbolDialogCommand;
}

void GroupEditDialog::createGUI ()
{
  int pos = 0;
  QLabel *label = new QLabel(tr("Group Symbols"));
  _vbox->insertWidget(pos++, label);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  _vbox->insertLayout(pos++, hbox);

  _list = new QListWidget;
  _list->setSortingEnabled(TRUE);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  hbox->addWidget(_list);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setSpacing(2);
  hbox->addLayout(tvbox);

  _addButton = new QPushButton;
  _addButton->setIcon(QIcon(add_xpm));
  _addButton->setToolTip(tr("Add Symbols"));
  connect(_addButton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
  tvbox->addWidget(_addButton);
  
  _deleteButton = new QPushButton;
  _deleteButton->setIcon(QIcon(delete_xpm));
  _deleteButton->setToolTip(tr("Delete Symbols"));
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteButtonPressed()));
  tvbox->addWidget(_deleteButton);

  tvbox->addStretch(1);
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

  _command->setReturnCode("0");

  saveSettings();

  accept();
}

void GroupEditDialog::addButtonPressed ()
{
  ScriptPlugin *plug = _factory.plugin(_symbolDialogCommand->plugin());
  if (! plug)
  {
    qDebug() << "GroupEditDialog::addButtonPressed: no plugin" << _symbolDialogCommand->plugin();
    return;
  }

  connect(plug, SIGNAL(signalResume()), this, SLOT(addButtonPressed2()));

  plug->command(_symbolDialogCommand);
}

void GroupEditDialog::addButtonPressed2 ()
{
  _list->addItems(_symbolDialogCommand->returnData("SYMBOL_DIALOG_SYMBOLS").split(";", QString::SkipEmptyParts));
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
