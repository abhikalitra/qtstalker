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

#include "GroupDialog.h"
#include "insert.xpm"
#include "delete.xpm"
#include "ok.xpm"
#include "stop.xpm"
#include "up.xpm"
#include <qtooltip.h>

GroupDialog::GroupDialog (Config *c) : QDialog (0, "GroupDialog", TRUE)
{
  config = c;
  flag = FALSE;

  setCaption(tr("Qtstalker: Edit Group"));
  
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);
  
  toolbar = new QGridLayout(vbox, 1, 6);
  toolbar->setSpacing(1);

  okButton = new QToolButton(this);
  QToolTip::add(okButton, tr("OK"));
  okButton->setPixmap(QPixmap(ok));
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  okButton->setMaximumWidth(30);
  okButton->setAutoRaise(TRUE);
  toolbar->addWidget(okButton, 0, 0);
  okButton->setEnabled(FALSE);

  cancelButton = new QToolButton(this);
  QToolTip::add(cancelButton, tr("Cancel"));
  cancelButton->setPixmap(QPixmap(stop));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  cancelButton->setMaximumWidth(30);
  cancelButton->setAutoRaise(TRUE);
  toolbar->addWidget(cancelButton, 0, 1);

  insertButton = new QToolButton(this);
  QToolTip::add(insertButton, tr("Insert"));
  insertButton->setPixmap(QPixmap(insert));
  connect(insertButton, SIGNAL(clicked()), this, SLOT(insertItem()));
  insertButton->setMaximumWidth(30);
  insertButton->setAutoRaise(TRUE);
  toolbar->addWidget(insertButton, 0, 2);
  insertButton->setEnabled(FALSE);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteItem()));
  deleteButton->setMaximumWidth(30);
  deleteButton->setAutoRaise(TRUE);
  toolbar->addWidget(deleteButton, 0, 3);
  deleteButton->setEnabled(FALSE);

  upButton = new QToolButton(this);
  QToolTip::add(upButton, tr("Parent Directory"));
  upButton->setPixmap(QPixmap(up));
  connect(upButton, SIGNAL(clicked()), this, SLOT(upDirectory()));
  upButton->setMaximumWidth(30);
  upButton->setAutoRaise(TRUE);
  toolbar->addWidget(upButton, 0, 4);

  fileSelector = new Navigator(this, config->getData(Config::DataPath), TRUE);
  connect(fileSelector, SIGNAL(fileSelected(QString)), this, SLOT(symbolSelected(QString)));
  vbox->addWidget(fileSelector);
  fileSelector->updateList();

  list = new QListView(this);
  connect(list, SIGNAL(clicked(QListViewItem *)), this, SLOT(listSelected(QListViewItem *)));
  list->setSelectionMode(QListView::Multi);
  list->addColumn(tr("Group Items"), -1);
  vbox->addWidget(list);
}

GroupDialog::~GroupDialog ()
{
}

void GroupDialog::setGroup (QStringList l)
{
  list->clear();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    item = new QListViewItem(list, l[loop]);

  item = 0;
}

QStringList GroupDialog::getGroup ()
{
  QStringList l;

  QListViewItemIterator it(list);
  while(it.current() != 0)
  {
    item = it.current();
    l.append(item->text(0));
    ++it;
  }

  return l;
}

void GroupDialog::insertItem ()
{
  QString s = fileSelector->getFileSelection();
  if (! s.length())
    return;

  item = new QListViewItem(list, s);

  flag = TRUE;

  okButton->setEnabled(TRUE);
}

void GroupDialog::deleteItem ()
{
  QList<QListViewItem> dl;
  dl.setAutoDelete(TRUE);

  QListViewItemIterator it(list);
  while(it.current() != 0)
  {
    item = it.current();
    if (item->isSelected())
      dl.append(item);
    ++it;
  }

  dl.clear();

  flag = TRUE;
  
  okButton->setEnabled(TRUE);
}

bool GroupDialog::getFlag ()
{
  return flag;
}

void GroupDialog::symbolSelected (QString d)
{
  if (! d.length())
    insertButton->setEnabled(FALSE);
  else
    insertButton->setEnabled(TRUE);
}

void GroupDialog::listSelected (QListViewItem *)
{
  bool flag = FALSE;
  QListViewItemIterator it(list);
  for (; it.current(); ++it)
  {
    item = it.current();
    if (item->isSelected())
    {
      flag = TRUE;
      break;
    }
  }

  if (! flag)
    deleteButton->setEnabled(FALSE);
  else
    deleteButton->setEnabled(TRUE);
}

void GroupDialog::upDirectory ()
{
  fileSelector->upDirectory();
}


