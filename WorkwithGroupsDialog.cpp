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
#include "done.xpm"
#include <qinputdialog.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qmessagebox.h>

WorkwithGroupsDialog::WorkwithGroupsDialog (Config *c) : QDialog (0, 0, FALSE)
{
  config = c;

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  QGridLayout *grid = new QGridLayout(vbox, 1, 7);
  grid->setSpacing(1);

  openButton = new QToolButton(this);
  QToolTip::add(openButton, tr("Open Group"));
  openButton->setPixmap(QPixmap(open));
  connect(openButton, SIGNAL(clicked()), this, SLOT(openGroup()));
  openButton->setMaximumWidth(30);
  openButton->setAutoRaise(TRUE);
  grid->addWidget(openButton, 0, 0);

  newButton = new QToolButton(this);
  QToolTip::add(newButton, tr("New Group"));
  newButton->setPixmap(QPixmap(newchart));
  connect(newButton, SIGNAL(clicked()), this, SLOT(newGroup()));
  newButton->setMaximumWidth(30);
  newButton->setAutoRaise(TRUE);
  grid->addWidget(newButton, 0, 1);

  editButton = new QToolButton(this);
  QToolTip::add(editButton, tr("Edit Group"));
  editButton->setPixmap(QPixmap(edit));
  connect(editButton, SIGNAL(clicked()), this, SLOT(editGroup()));
  editButton->setMaximumWidth(30);
  editButton->setAutoRaise(TRUE);
  grid->addWidget(editButton, 0, 2);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete Group"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteGroup()));
  deleteButton->setMaximumWidth(30);
  deleteButton->setAutoRaise(TRUE);
  grid->addWidget(deleteButton, 0, 3);

  renameButton = new QToolButton(this);
  QToolTip::add(renameButton, tr("Rename Group"));
  renameButton->setPixmap(QPixmap(renam));
  connect(renameButton, SIGNAL(clicked()), this, SLOT(renameGroup()));
  renameButton->setMaximumWidth(30);
  renameButton->setAutoRaise(TRUE);
  grid->addWidget(renameButton, 0, 4);

  QToolButton *button = new QToolButton(this);
  QToolTip::add(button, tr("Done"));
  button->setPixmap(QPixmap(finished));
  connect(button, SIGNAL(clicked()), this, SLOT(reject()));
  button->setMaximumWidth(30);
  button->setAutoRaise(TRUE);
  grid->addWidget(button, 0, 5);

  QFrame *sep = new QFrame(this);
  sep->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  vbox->addWidget(sep);

  list = new QListView(this);
  list->addColumn(tr("Group"), 200);
  list->setSelectionMode(QListView::Single);
  connect(list, SIGNAL(selectionChanged()), this, SLOT(buttonStatus()));
  connect(list, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(openGroup()));
  vbox->addWidget(list);

  buttonStatus();
}

WorkwithGroupsDialog::~WorkwithGroupsDialog ()
{
}

void WorkwithGroupsDialog::updateList ()
{
  list->clear();
  
  QStringList l = config->getGroupList();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    item = new QListViewItem(list, l[loop]);
}

void WorkwithGroupsDialog::openGroup ()
{
  item = list->selectedItem();
  if (item)
  {
    emit groupOpened (item->text(0));
    reject();
  }
}

void WorkwithGroupsDialog::newGroup()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("New Group"), tr("Enter new group symbol."),
  					    QLineEdit::Normal, tr("New Group"), &ok, this);
  if ((! ok) || (selection.isNull()))
    return;

  Setting *group = config->getGroup(selection);
  int t = group->count();
  delete group;
  if (t != 0)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This group already exists."));
    return;
  }

  GroupDialog *dialog = new GroupDialog(config);

  Setting *set = new Setting;
  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    config->setGroup(selection, set);
    updateList();
  }

  delete set;
  delete dialog;
}

void WorkwithGroupsDialog::editGroup()
{
  GroupDialog *dialog = new GroupDialog(config);

  Setting *group = config->getGroup(item->text(0));
  dialog->setItems(group);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    config->setGroup(item->text(0), group);
  }

  delete group;
  delete dialog;
}

void WorkwithGroupsDialog::deleteGroup()
{
  config->deleteGroup(item->text(0));
  delete item;
  buttonStatus();
}

void WorkwithGroupsDialog::renameGroup ()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("Rename Group"), tr("Enter new group symbol."),
  					    QLineEdit::Normal, item->text(0), &ok, this);
  if ((ok) && (! selection.isNull()))
  {
    Setting *group = config->getGroup(selection);
    int t = group->count();
    delete group;
    if (t != 0)
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This chart group exists."));
      return;
    }

    group = config->getGroup(item->text(0));
    config->deleteGroup(item->text(0));
    config->setGroup(selection, group);
    delete group;

    updateList();
    buttonStatus();
  }
}

void WorkwithGroupsDialog::buttonStatus ()
{
  item = list->selectedItem();
  if (item)
  {
    openButton->setEnabled(TRUE);
    deleteButton->setEnabled(TRUE);
    editButton->setEnabled(TRUE);
    renameButton->setEnabled(TRUE);
  }
  else
  {
    openButton->setEnabled(FALSE);
    deleteButton->setEnabled(FALSE);
    editButton->setEnabled(FALSE);
    renameButton->setEnabled(FALSE);
  }
}



