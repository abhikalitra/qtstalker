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
#include <qtooltip.h>

GroupDialog::GroupDialog (Config *c) : EditDialog (c)
{
  flag = FALSE;

  setCaption(tr("Qtstalker: Edit Group"));
  
  setFileSelector();

  list2 = new QListView(this);
  list2->setSelectionMode(QListView::Multi);
  list2->addColumn(tr("Group Items"));
  topBox->addWidget(list2);

  toolbar->expand(1, 6);

  insertButton = new QToolButton(this);
  QToolTip::add(insertButton, tr("Insert"));
  insertButton->setPixmap(QPixmap(insert));
  connect(insertButton, SIGNAL(clicked()), this, SLOT(insertItem()));
  insertButton->setMaximumWidth(30);
  insertButton->setAutoRaise(TRUE);
  toolbar->addWidget(insertButton, 0, 3);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteItem()));
  deleteButton->setMaximumWidth(30);
  deleteButton->setAutoRaise(TRUE);
  toolbar->addWidget(deleteButton, 0, 4);
  
  updateFileList();
}

GroupDialog::~GroupDialog ()
{
}

void GroupDialog::setGroup (QStringList l)
{
  list2->clear();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    item = new QListViewItem(list2, l[loop]);

  item = 0;
}

QStringList GroupDialog::getGroup ()
{
  QStringList l;
  
  QListViewItemIterator it(list2);
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
  QString s = getFileSelection();
  if (! s.length())
    return;

  item = new QListViewItem(list2, s);

  flag = TRUE;
}

void GroupDialog::deleteItem ()
{
  QList<QListViewItem> dl;
  dl.setAutoDelete(TRUE);

  QListViewItemIterator it(list2);
  while(it.current() != 0)
  {
    item = it.current();
    if (item->isSelected())
      dl.append(item);
    ++it;
  }

  dl.clear();
  
  flag = TRUE;
}

bool GroupDialog::getFlag ()
{
  return flag;
}


