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
#include "Doc.h"
#include "Command.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QtDebug>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>

GroupEditDialog::GroupEditDialog (Command *command)
{
  _command = command;
  _name = _command->parm(1);
  _helpFile = "main.html";
  setWindowTitle("QtStalker" + g_session + ": " + tr("Edit Group"));

  createGUI();

  QStringList l;
  int loop = 2;
  for (; loop < _command->count(); loop++)
    l << _command->parm(loop);
  _list->clear();
  _list->addItems(l);

  loadSettings();

  selectionChanged();

  _symbolDialogCommand = new Command("SYMBOL_DIALOG,0");

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

GroupEditDialog::~GroupEditDialog ()
{
  delete _symbolDialogCommand;
}

void GroupEditDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QLabel *label = new QLabel(tr("Group Symbols"));
  vbox->addWidget(label);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  vbox->addLayout(hbox);

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

  // status message
  _message = new QLabel;
  vbox->addWidget(_message);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);
//  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
//  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

  // ok button
  _okButton = bbox->addButton(QDialogButtonBox::Ok);
  connect(_okButton, SIGNAL(clicked()), this, SLOT(done()));
  _okButton->setDefault(TRUE);

  // cancel button
  _cancelButton = bbox->addButton(QDialogButtonBox::Cancel);
  connect(_cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
  _cancelButton->setDefault(TRUE);

  // help button
  QPushButton *b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void GroupEditDialog::selectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (sl.count())
    status = 1;

  _deleteButton->setEnabled(status);
}

void GroupEditDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void GroupEditDialog::done ()
{
  QStringList l;
  int loop = 0;
  for (; loop < _list->count(); loop++)
    l << _list->item(loop)->text();

  _command->setReturnData(l.join(","));

  saveSettings();

  accept();
}

void GroupEditDialog::cancel ()
{
  saveSettings();
  reject();
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
  _list->addItems(_symbolDialogCommand->stringData().split(",", QString::SkipEmptyParts));
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

void GroupEditDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);

  QSize sz = settings.value("group_edit_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("group_edit_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void GroupEditDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("group_edit_dialog_window_size", size());
  settings.setValue("group_edit_dialog_window_position", pos());
  settings.sync();
}
