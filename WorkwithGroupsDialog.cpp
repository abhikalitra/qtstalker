/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "WorkwithGroupsDialog.h"
#include "GroupDialog.h"
#include "Setting.h"
#include "open.xpm"
#include "newchart.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include <qinputdialog.h>
#include <qtooltip.h>
#include <qmessagebox.h>

WorkwithGroupsDialog::WorkwithGroupsDialog (Config *c) : EditDialog (c)
{
  list->hide();

  toolbar->expand(1, 7);

  openButton = new QToolButton(this);
  QToolTip::add(openButton, tr("Open Group"));
  openButton->setPixmap(QPixmap(open));
  connect(openButton, SIGNAL(clicked()), this, SLOT(openGroup()));
  openButton->setMaximumWidth(30);
  openButton->setAutoRaise(TRUE);
  toolbar->addWidget(openButton, 0, 0);

  newButton = new QToolButton(this);
  QToolTip::add(newButton, tr("New Group"));
  newButton->setPixmap(QPixmap(newchart));
  connect(newButton, SIGNAL(clicked()), this, SLOT(newGroup()));
  newButton->setMaximumWidth(30);
  newButton->setAutoRaise(TRUE);
  toolbar->addWidget(newButton, 0, 2);

  editButton = new QToolButton(this);
  QToolTip::add(editButton, tr("Edit Group"));
  editButton->setPixmap(QPixmap(edit));
  connect(editButton, SIGNAL(clicked()), this, SLOT(editGroup()));
  editButton->setMaximumWidth(30);
  editButton->setAutoRaise(TRUE);
  toolbar->addWidget(editButton, 0, 3);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete Group"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteGroup()));
  deleteButton->setMaximumWidth(30);
  deleteButton->setAutoRaise(TRUE);
  toolbar->addWidget(deleteButton, 0, 4);

  renameButton = new QToolButton(this);
  QToolTip::add(renameButton, tr("Rename Group"));
  renameButton->setPixmap(QPixmap(renam));
  connect(renameButton, SIGNAL(clicked()), this, SLOT(renameGroup()));
  renameButton->setMaximumWidth(30);
  renameButton->setAutoRaise(TRUE);
  toolbar->addWidget(renameButton, 0, 5);

  list2 = new QListView(this);
  list2->addColumn(tr("Group"), 200);
  list2->setSelectionMode(QListView::Single);
  connect(list2, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(openGroup()));
  baseBox->addWidget(list2);
}

WorkwithGroupsDialog::~WorkwithGroupsDialog ()
{
}

void WorkwithGroupsDialog::updateList ()
{
  list2->clear();
  
  QStringList l = config->getGroupList();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    item = new QListViewItem(list2, l[loop]);
}

void WorkwithGroupsDialog::openGroup ()
{
  item = list2->selectedItem();
  if (item)
  {
    emit groupOpened (item->text(0));
    accept();
  }
}

void WorkwithGroupsDialog::newGroup()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("New Group"), tr("Enter new group symbol."),
  					    QLineEdit::Normal, tr("New Group"), &ok, this);
  if ((! ok) || (selection.isNull()))
    return;

  QStringList l = config->getGroup(selection);
  if (l.count() != 0)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This group already exists."));
    return;
  }

  GroupDialog *dialog = new GroupDialog(config);

  dialog->setGroup(l);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    config->setGroup(selection, dialog->getGroup());
    updateList();
  }

  delete dialog;
}

void WorkwithGroupsDialog::editGroup()
{
  item = list2->selectedItem();
  if (! item)
    return;

  GroupDialog *dialog = new GroupDialog(config);

  dialog->setGroup(config->getGroup(item->text(0)));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    config->setGroup(item->text(0), dialog->getGroup());
    if (dialog->getFlag())
    {
      QString s = config->getData(Config::Group);
      if (! s.compare(item->text(0)))
        emit groupOpened (item->text(0));
    }
  }

  delete dialog;
}

void WorkwithGroupsDialog::deleteGroup()
{
  item = list2->selectedItem();
  if (! item)
    return;

  config->deleteGroup(item->text(0));
  delete item;
}

void WorkwithGroupsDialog::renameGroup ()
{
  item = list2->selectedItem();
  if (! item)
    return;

  bool ok;
  QString selection = QInputDialog::getText(tr("Rename Group"), tr("Enter new group symbol."),
  					    QLineEdit::Normal, item->text(0), &ok, this);
  if ((ok) && (! selection.isNull()))
  {
    QStringList l = config->getGroup(selection);
    if (l.count() != 0)
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This chart group exists."));
      return;
    }

    QStringList group = config->getGroup(item->text(0));
    config->deleteGroup(item->text(0));
    config->setGroup(selection, group);

    updateList();
  }
}



